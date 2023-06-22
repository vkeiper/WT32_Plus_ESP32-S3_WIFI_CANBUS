/*----------------------------------------------------------------------------
 *      RL-ARM - CAN
 *----------------------------------------------------------------------------
 *      Name:    CAN_Ex1.c
 *      Purpose: RTX CAN Driver usage example
 *      Rev.:    V4.70
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2013 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include "cmsis_os.h"                      	/* RTX kernel functions & defines      */
#include "stm32f4xx.h"                			/* STM32F40x register definitions      */
#include "RTX_CAN.h"                  			/* CAN Generic functions & defines     */ 
#include "Board_GLCD.h"                     /* GLCD functions prototypes           */
#include "GLCD_Config.h"               
#include "Board_ADC.h"                  
#include "stm32f4xx_hal.h"
#include "string.h"
#include "CAN_Ex1.h"               
#include "iccp_canfuncs.h"               
#include "iccpfuncs.h"

ADC_HandleTypeDef       hADC3;
DAC_HandleTypeDef       hDAC1;
ADC_ChannelConfTypeDef  sConfigADC;
DAC_ChannelConfTypeDef  sConfigDAC;

volatile  uint16_t   address[2];

// External resources vk,
extern uint8_t b_CANTXFlag;
extern    GLCD_FONT GLCD_Font_16x24;     
extern    uint32_t os_time;

uint32_t canbuad = 500000; //CANBaud rate of 500K

/*=============================================================================
 *
 *===========================================================================*/
uint32_t HAL_GetTick(void) {
  return os_time;
}
/*=============================================================================
 *
 *===========================================================================*/
osThreadId tid_send_CAN;                  /* Thread id of thread: phase_b      */
osThreadId tid_rece_CAN;                  /* Thread id of thread: phase_c      */
osThreadId tid_disp;                      /* Thread id of thread: phase_c      */

void task_send_CAN (void  const *argument);
void task_rece_CAN (void  const *argument);
void task_disp     (void  const *argument);


CAN_msg cantx_msg;

uint32_t Tx_val = 0, Rx_val = 0;           /* Global variables used for display   */

unsigned char hex_chars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

/*=============================================================================
 *  Function for converting 1 byte to string in hexadecimal notation
 *===========================================================================*/
void Hex_Str (unsigned char hex, unsigned char *str) {
  *str++ = '0';
  *str++ = 'x';
  *str++ = hex_chars[hex >>  4];
  *str++ = hex_chars[hex & 0xF];
}
/*=============================================================================
 *  Function to getting input value
 *===========================================================================*/
uint16_t In_Get (void)
{
  int16_t val;
  
  switch (sConfigADC.Channel) {
    case ADC_CHANNEL_10:
      val = SysData.RefCell_0.collection[M_ATZERO];
    break;
    case ADC_CHANNEL_11:
      val = SysData.RefCell_1.collection[M_ATZERO];
    break;
    case ADC_CHANNEL_12:
      val = SysData.RefCell_2.collection[M_ATZERO];
    break;
    case ADC_CHANNEL_13:
      val = SysData.RefCell_3.collection[M_ATZERO];
    break;
    default:
    break;
  }
  return (val >> 4);           /* Scale analog value to 8 bits      */
}
/*=============================================================================
 *  Functions for init and output of value on visual element
 *===========================================================================*/
void Out_Init (void) {

  /* Setup LCD                                                               */
  GLCD_Initialize();                    /* Initialize the GLCD               */
  GLCD_SetBackgroundColor (GLCD_COLOR_BLUE);
  GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
  GLCD_ClearScreen        ();
  GLCD_SetFont            (&GLCD_Font_16x24);
  GLCD_DrawString         (0, 0*24, " MCBSTM32F400 Demo ");
  GLCD_DrawString         (0, 1*24, " CAN_Ex1 ");
  GLCD_DrawString         (0, 9*24, "    www.keil.com    ");
  GLCD_DrawString(0, 4*24, " CAN at 500kbit/s   ");
  GLCD_DrawString(0, 8*24, " AD value:          ");

}
void Out_Val (void) {
  uint32_t val1, val2;
  static unsigned char disp_buf[] = " Tx:    , Rx:    ";

  val1 = Tx_val;                        /* Read values to local variable     */
  val2 = Rx_val;    

  Hex_Str(val1, &disp_buf[ 4]);         /* Display Tx and Rx values to LCD   */ 
  Hex_Str(val2, &disp_buf[13]);
  GLCD_DrawString(0, 7*24, (char *)disp_buf); /* Print string to LCD */

  GLCD_DrawBargraph (160, 8*24, 140, 20, Tx_val*100/255); /* Disp tx val on bargraph */
}
/*=============================================================================
 *  Task 1: Sends message with input value in data[0] over CAN periodically
 *===========================================================================*/
void task_send_CAN (void  const *argument)  {
  
	// Init ICCP CAN API
	IccpCanApiInit();
	
	
	/* Initialize message  = { ID, {data[0] .. data[7]}, LEN, CHANNEL, FORMAT, TYPE } */
  //CAN_msg cantx_msg       = { 33, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   
   //                           1, 2, STANDARD_FORMAT, DATA_FRAME };

  CAN_init (1, 500000);               /* CAN controller 1 init, 500 kbit/s   */

  CAN_rx_object (1, 2,  33, DATA_TYPE | STANDARD_TYPE); /* Enable reception  */
                                      /* of message on controller 1, channel */
                                      /* is not used for STM32 (can be set to*/
                                      /* whatever value), data frame with    */
                                      /* standard id 33                      */

#if defined(CAN_LOOPBACK)

  /* The activation of test mode in line below is used for enabling 
     self-testing mode when CAN controller receives the message it sends so 
     program functionality can be tested without another CAN device          */
  /* COMMENT THE LINE BELOW TO ENABLE DEVICE TO PARTICIPATE IN CAN NETWORK   */
  CAN_hw_testmode(1, CAN_BTR_SILM | CAN_BTR_LBKM); /* Loopback and           */
                                                   /* Silent Mode (self-test)*/
#endif

  CAN_start (1);    /* Start controller 1                  */
	b_CANTXFlag =0;
	
	
  for (;;)  
	{
			CANDoCAN();// check if CAN cmds are in API buffer and execute all in buffer
			
			 /*##- Transmit CAN1 if Flag SET ###############################*/
			 if(b_CANTXFlag)
			 {
					b_CANTXFlag = 0; //clear flag for next TX
					CAN_send (1, &cantx_msg, 0x0F00);  /* Send cantx_msg on CAN1       */
			 }
				
    osDelay (50);  /* Do nothing for 50mS to allow other tasks time to run    */
  }
}
/* Task 2: Received CAN message -------------------------------------------- */
void task_rece_CAN (void  const *argument)  {
  CAN_msg canrx_msg;
	uint8_t lclCanBuff[8];
	int8_t i;
  
	for (;;)  
	{
			/* When message arrives store received value to global variable Rx_val   */
			if (CAN_receive (1, &canrx_msg, 0x00FF) == CAN_OK)  
			{
					memset(lclCanBuff,0,sizeof(lclCanBuff));//clear buffer for population
					for ( i = 0; i < 8; i++ ) 
					{
							lclCanBuff[i] = canrx_msg.data[i];	
					}
					CanApiEnqueue(canrx_msg.id, lclCanBuff,canrx_msg.len); // move data from CANFIFO to CANAPI ring buffer
			}
  }
}
/* Task 3: Run ICCP state machine also in debug mode Activate visual outputs ------------------------------------------*/
void task_disp (void  const *argument)  {
  for (;;)  
	{
			IccpDoIccp();   // run iccp state machine*
		
			Out_Val ();                       /* Output info on visual outputs       */
			
			osDelay (250);                     /* Wait 100 ms and display again, was 10      */
  }
}
/*=============================================================================
 *  Task 4: DAC output
 *===========================================================================*/
osThreadId tid_dac_write;

static void task_dac_write(void const *argument)
{
  uint16_t iDAC = 0;

  for (;;) {
      HAL_DAC_SetValue(&hDAC1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, iDAC);
      if (iDAC < 4095)
        iDAC += 65;
      else
        iDAC = 0;
      
    HAL_Delay(1000);
  }
}
/*=============================================================================
 *  CMSIS-RTX RTOS Timers
 *===========================================================================*/
osTimerId TmrId_GetADC_Samples;

osTimerDef(Timer1, GetADC_Samples);

void GetADC_Samples(void const *argument)
{
  if (SysData.IndexCounter <= M_INDEX) {
    SysData.RefCell_0.collection[SysData.IndexCounter]  = SysData.RefCell_0.collection[M_ATZERO];
    SysData.RefCell_1.collection[SysData.IndexCounter]  = SysData.RefCell_1.collection[M_ATZERO];
    SysData.RefCell_2.collection[SysData.IndexCounter]  = SysData.RefCell_2.collection[M_ATZERO];
    SysData.RefCell_3.collection[SysData.IndexCounter]  = SysData.RefCell_3.collection[M_ATZERO];
    SysData.Irdbk_0.collection[SysData.IndexCounter]    = SysData.Irdbk_0.collection[M_ATZERO];
    SysData.Irdbk_1.collection[SysData.IndexCounter]    = SysData.Irdbk_1.collection[M_ATZERO];
    SysData.Irdbk_2.collection[SysData.IndexCounter]    = SysData.Irdbk_2.collection[M_ATZERO];
    SysData.Irdbk_3.collection[SysData.IndexCounter]    = SysData.Irdbk_3.collection[M_ATZERO];
    SysData.Vrdbk.collection[SysData.IndexCounter]      = SysData.Vrdbk.collection[M_ATZERO];
                              
    SysData.RefCell_0.sum += SysData.RefCell_0.collection[SysData.IndexCounter];
    SysData.RefCell_1.sum += SysData.RefCell_1.collection[SysData.IndexCounter];
    SysData.RefCell_2.sum += SysData.RefCell_2.collection[SysData.IndexCounter];
    SysData.RefCell_3.sum += SysData.RefCell_3.collection[SysData.IndexCounter];
    SysData.Irdbk_0.sum   += SysData.Irdbk_0.collection[SysData.IndexCounter];
    SysData.Irdbk_1.sum   += SysData.Irdbk_1.collection[SysData.IndexCounter];
    SysData.Irdbk_2.sum   += SysData.Irdbk_2.collection[SysData.IndexCounter];
    SysData.Irdbk_3.sum   += SysData.Irdbk_3.collection[SysData.IndexCounter];
    SysData.Vrdbk.sum     += SysData.Vrdbk.collection[SysData.IndexCounter];

    SysData.IndexCounter++;
  }
  
  if (SysData.IndexCounter > M_INDEX) {
    SysData.RefCell_0.avg = (SysData.RefCell_0.sum >> 7);
    SysData.RefCell_1.avg = (SysData.RefCell_1.sum >> 7);
    SysData.RefCell_2.avg = (SysData.RefCell_2.sum >> 7);
    SysData.RefCell_3.avg = (SysData.RefCell_3.sum >> 7);
    SysData.Irdbk_0.avg   = (SysData.Irdbk_0.sum >> 7);
    SysData.Irdbk_1.avg   = (SysData.Irdbk_1.sum >> 7);
    SysData.Irdbk_2.avg   = (SysData.Irdbk_2.sum >> 7);
    SysData.Irdbk_3.avg   = (SysData.Irdbk_3.sum >> 7);
    SysData.Vrdbk.avg     = (SysData.Vrdbk.sum >> 7);
    SysData.IndexCounter  = 1;
  }

/* Timer instantiations -----------------------------------------------------*/  
TmrId_GetADC_Samples = osTimerCreate(osTimer(Timer1), osTimerPeriodic, NULL);
}
/*=============================================================================
 *  DAC Configuration
 *===========================================================================*/
static void DAC_Init(void)
{
  hDAC1.Instance               = DAC;
  
  if (HAL_DAC_Init(&hDAC1) != HAL_OK)
    nError_Handler();
  
  sConfigDAC.DAC_OutputBuffer =  DAC_OUTPUTBUFFER_ENABLE;
  sConfigDAC.DAC_Trigger      =  DAC_TRIGGER_NONE;

  if (HAL_DAC_ConfigChannel(&hDAC1, &sConfigDAC, DAC_CHANNEL_1) != HAL_OK)
    nError_Handler();

  if (HAL_DAC_Start(&hDAC1, DAC_CHANNEL_1) != HAL_OK)
    nError_Handler();
}
/*=============================================================================
 *  HAL ADC conversion complete callback 
 *===========================================================================*/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hADC3)
{
  switch (sConfigADC.Channel) {
    case ADC_CHANNEL_10:
      SysData.RefCell_0.collection[M_ATZERO]  = HAL_ADC_GetValue(hADC3);
    break;

    case ADC_CHANNEL_11:
      SysData.RefCell_1.collection[M_ATZERO]  = HAL_ADC_GetValue(hADC3);
    break;

    case ADC_CHANNEL_12:
      SysData.RefCell_2.collection[M_ATZERO]  = HAL_ADC_GetValue(hADC3);
    break;

    case ADC_CHANNEL_13:
      SysData.RefCell_3.collection[M_ATZERO]  = HAL_ADC_GetValue(hADC3);
    break;

    case ADC_CHANNEL_4:
      SysData.Irdbk_0.collection[M_ATZERO]    = HAL_ADC_GetValue(hADC3);
    break;

    case ADC_CHANNEL_5:
      SysData.Irdbk_1.collection[M_ATZERO]    = HAL_ADC_GetValue(hADC3);
    break;

    case ADC_CHANNEL_6:
      SysData.Vrdbk.collection[M_ATZERO]      = HAL_ADC_GetValue(hADC3);
    break;

    case ADC_CHANNEL_7:
      SysData.Irdbk_2.collection[M_ATZERO]    = HAL_ADC_GetValue(hADC3);
    break;

    case ADC_CHANNEL_8:
      SysData.Irdbk_3.collection[M_ATZERO]    = HAL_ADC_GetValue(hADC3);
    break;

    case ADC_CHANNEL_9:
      SysData.Temperature                       = HAL_ADC_GetValue(hADC3);
    break;

    case ADC_CHANNEL_14:
      address[M_ATZERO]                       = HAL_ADC_GetValue(hADC3);
    break;

    case ADC_CHANNEL_15:
      address[M_ATZERO + 1]                   = HAL_ADC_GetValue(hADC3);
    break;

    default:
    break;
  }
}
/*=============================================================================
 *  ADC Configuration (Interrupts)
 *===========================================================================*/
static void ADC_Init(void)
{
  /* Configure the global features of the ADC
    (Clock, Resolution, Data Alignment and number of conversion) */
  hADC3.Instance                      = ADC3;
  hADC3.Init.ClockPrescaler           = ADC_CLOCKPRESCALER_PCLK_DIV4;
  hADC3.Init.Resolution               = ADC_RESOLUTION12b;
  hADC3.Init.ScanConvMode             = ENABLE;
  hADC3.Init.ContinuousConvMode       = ENABLE;
  hADC3.Init.DiscontinuousConvMode    = DISABLE;
  hADC3.Init.NbrOfDiscConversion      = 0;
  hADC3.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hADC3.Init.ExternalTrigConv         = ADC_EXTERNALTRIGCONV_T1_CC1;
  hADC3.Init.DataAlign                = ADC_DATAALIGN_RIGHT;
  hADC3.Init.NbrOfConversion          = 12;
  hADC3.Init.DMAContinuousRequests    = DISABLE;
  hADC3.Init.EOCSelection             = EOC_SINGLE_CONV;
  
  if (HAL_ADC_Init(&hADC3) != HAL_OK)
    nError_Handler();

  /* Configure ADC channel PC0, REF_CELL_0 */
  sConfigADC.Channel       = ADC_CHANNEL_10;
  sConfigADC.Rank          = 3;
  sConfigADC.SamplingTime  = ADC_SAMPLETIME_15CYCLES;
  sConfigADC.Offset        = 0;
  
  if (HAL_ADC_ConfigChannel(&hADC3, &sConfigADC) != HAL_OK)
    nError_Handler();

  /* Configure ADC channel PC1, REF_CELL_1 */
  sConfigADC.Channel       = ADC_CHANNEL_11;
  sConfigADC.Rank          = 4;
  sConfigADC.SamplingTime  = ADC_SAMPLETIME_15CYCLES;
  sConfigADC.Offset        = 0;

  if (HAL_ADC_ConfigChannel(&hADC3, &sConfigADC) != HAL_OK)
    nError_Handler();

  /* Configure ADC channel PC2, REF_CELL_2 */
  sConfigADC.Channel       = ADC_CHANNEL_12;
  sConfigADC.Rank          = 5;
  sConfigADC.SamplingTime  = ADC_SAMPLETIME_15CYCLES;
  sConfigADC.Offset        = 0;

  if (HAL_ADC_ConfigChannel(&hADC3, &sConfigADC) != HAL_OK)
    nError_Handler();

  /* Configure ADC channel PC3, REF_CELL_3 */
  sConfigADC.Channel       = ADC_CHANNEL_13;
  sConfigADC.Rank          = 6;
  sConfigADC.SamplingTime  = ADC_SAMPLETIME_15CYCLES;
  sConfigADC.Offset        = 0;

  if (HAL_ADC_ConfigChannel(&hADC3, &sConfigADC) != HAL_OK)
    nError_Handler();

  /* Configure ADC channel PF6, IMON_0 */
  sConfigADC.Channel       = ADC_CHANNEL_4;
  sConfigADC.Rank          = 7;
  sConfigADC.SamplingTime  = ADC_SAMPLETIME_15CYCLES;
  sConfigADC.Offset        = 0;

  if (HAL_ADC_ConfigChannel(&hADC3, &sConfigADC) != HAL_OK)
    nError_Handler();

  /* Configure ADC channel PF7, IMON_1 */
  sConfigADC.Channel       = ADC_CHANNEL_5;
  sConfigADC.Rank          = 8;
  sConfigADC.SamplingTime  = ADC_SAMPLETIME_15CYCLES;
  sConfigADC.Offset        = 0;

  if (HAL_ADC_ConfigChannel(&hADC3, &sConfigADC) != HAL_OK)
    nError_Handler();

  /* Configure ADC channel PF8, VMON */
  sConfigADC.Channel       = ADC_CHANNEL_6;
  sConfigADC.Rank          = 9;
  sConfigADC.SamplingTime  = ADC_SAMPLETIME_15CYCLES;
  sConfigADC.Offset        = 0;

  if (HAL_ADC_ConfigChannel(&hADC3, &sConfigADC) != HAL_OK)
    nError_Handler();

  /* Configure ADC channel PF9, IMON_2 */
  sConfigADC.Channel       = ADC_CHANNEL_7;
  sConfigADC.Rank          = 10;
  sConfigADC.SamplingTime  = ADC_SAMPLETIME_15CYCLES;
  sConfigADC.Offset        = 0;

  if (HAL_ADC_ConfigChannel(&hADC3, &sConfigADC) != HAL_OK)
    nError_Handler();

  /* Configure ADC channel PF10, IMON_3 */
  sConfigADC.Channel       = ADC_CHANNEL_8;
  sConfigADC.Rank          = 11;
  sConfigADC.SamplingTime  = ADC_SAMPLETIME_15CYCLES;
  sConfigADC.Offset        = 0;

  if (HAL_ADC_ConfigChannel(&hADC3, &sConfigADC) != HAL_OK)
    nError_Handler();

  /* Configure ADC channel PF10, Temperature */
  sConfigADC.Channel       = ADC_CHANNEL_9;
  sConfigADC.Rank          = 12;
  sConfigADC.SamplingTime  = ADC_SAMPLETIME_15CYCLES;
  sConfigADC.Offset        = 0;

  if (HAL_ADC_ConfigChannel(&hADC3, &sConfigADC) != HAL_OK)
    nError_Handler();
  
   /* Configure ADC channel PF3, DC_ADDR */
  sConfigADC.Channel       = ADC_CHANNEL_9;
  sConfigADC.Rank          = 2;
  sConfigADC.SamplingTime  = ADC_SAMPLETIME_15CYCLES;
  sConfigADC.Offset        = 0;

  if (HAL_ADC_ConfigChannel(&hADC3, &sConfigADC) != HAL_OK)
    nError_Handler();

   /* Configure ADC channel PF4, NODE_ADDR */
  sConfigADC.Channel       = ADC_CHANNEL_9;
  sConfigADC.Rank          = 1;
  sConfigADC.SamplingTime  = ADC_SAMPLETIME_15CYCLES;
  sConfigADC.Offset        = 0;

  if (HAL_ADC_ConfigChannel(&hADC3, &sConfigADC) != HAL_OK)
    nError_Handler();

  if (HAL_ADC_Start_IT(&hADC3) != HAL_OK)
    nError_Handler();
}
/*=============================================================================
 * DC and Node address
 *===========================================================================*/
static void GetAddress(void)
{
  SysData.DC_ADDR   = address[M_ATZERO];
  SysData.NODE_ADDR = address[M_ATZERO + 1];
}
/*=============================================================================
 *  System Clock Configuration
 *===========================================================================*/
static void SystemClock_Config(void) 
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  /* Enable Power Control clock */
  __PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the
     device is clocked below the maximum system frequency (see datasheet). */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType 		= RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState 					= RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState 			= RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource 		= RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM 					= 25;
  RCC_OscInitStruct.PLL.PLLN 					= 336;
  RCC_OscInitStruct.PLL.PLLP 					= RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ 					= 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType 				= RCC_CLOCKTYPE_SYSCLK;
	RCC_ClkInitStruct.ClockType					|= RCC_CLOCKTYPE_PCLK1;													
  RCC_ClkInitStruct.ClockType         |= RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource 			= RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider 		= RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider 		= RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider 		= RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}
/*=============================================================================
 *  Error_Handler: User defined
 *===========================================================================*/
void nError_Handler(void)
{
  while (1) {
  }
}
/*=============================================================================
 *  Main: Initialize and start RTX Kernel
 *===========================================================================*/
osThreadDef(task_send_CAN, osPriorityNormal, 1, 0);
osThreadDef(task_rece_CAN, osPriorityNormal, 1, 0);
osThreadDef(task_disp    , osPriorityNormal, 1, 0);
osThreadDef(task_dac_write, osPriorityNormal, 1, 0);

int main (void)                             /* Program execution starts here       */
{
  HAL_Init();                               /* Initialize the HAL Library          */
  SystemClock_Config();                     /* Configure the System Clock          */
	ADC_Init();

  SysData.IndexCounter  = 1;
  SysData.RefCell_0.sum = 0;
  SysData.RefCell_1.sum = 0;
  SysData.RefCell_2.sum = 0;
  SysData.RefCell_3.sum = 0;
  SysData.Irdbk_0.sum   = 0;
  SysData.Irdbk_1.sum   = 0;
  SysData.Irdbk_2.sum   = 0;
  SysData.Irdbk_3.sum   = 0;
  SysData.Vrdbk.sum     = 0;
  
  osTimerStart(TmrId_GetADC_Samples, 1);
  GetAddress();                             /* Addresses for DC and Node          */
  DAC_Init();
  Out_Init();                               /* Initialize visual outputs           */


  tid_send_CAN  = osThreadCreate(osThread(task_send_CAN ), NULL);
  tid_rece_CAN  = osThreadCreate(osThread(task_rece_CAN ), NULL);
  tid_disp      = osThreadCreate(osThread(task_disp     ), NULL);
  tid_dac_write = osThreadCreate(osThread(task_dac_write), NULL);

  osDelay(osWaitForever);
}

/** @Author Vince Keiper
  * @brief CAN TX Buffer Mutator for Keil RTX CAN library.  
  *        This function sets the CAN_msg struct type id,DLC, and tx data buffer:
  *          - Also sets boolean CANTx Flag which triggers CANThread to TX 
	*            the tx buffer
  *         
  * @param ptrBuff: pointer to lclbuffer in caller
  * @param len: length of CANTX buffer, qty bytes to send 0-8
  * @param id: function ID portion of the CANID to be TX'd 
  * @retval None
  */
void CanTxBufferMutator(uint8_t * ptrBuff,uint8_t len, uint32_t id)
{
		int8_t i;
	
		b_CANTXFlag = 1;
		
		if(len >8){len = 8;}
		
		for ( i = 0; i < len; i++ ) 
		{
				cantx_msg.data[i] = *ptrBuff++;
    }
		cantx_msg.id = id;
		cantx_msg.len = len;
		cantx_msg.format = EXTENDED_FORMAT;
		cantx_msg.type = DATA_FRAME; 
}

/*=============================================================================
 *  end of file
 *===========================================================================*/

