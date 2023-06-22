/**
  ******************************************************************************
  * @file    iccp_can_stm32f429.c
  * @author  Vince Keiper AstrodyneTDI
  * @version V1.0.0
  * @date    24-June-2015
  * @brief   CAN1 Module.    
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 AstrodyneTDI</center></h2>
  *
  *
  ******************************************************************************  
  */ 

/* Includes ------------------------------------------------------------------*/
#include "iccp_can_stm32f429.h"
#include "cmsis_os.h"
#include "iccpfuncs.h"
#include "iccp_canfuncs.h"
#include "stdio.h"
#include "string.h"
/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @defgroup CAN_Networking
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define KEY_PRESSED     0x00
//#define KEY_NOT_PRESSED 0x01
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
//uint8_t ubKeyNumber = 0x0;
uint8_t b_CANTXFlag;

CAN_HandleTypeDef CanHandle;
CAN_HandleTypeDef CanHandleTx;
/* Private function prototypes -----------------------------------------------*/
static void CAN_Config(void);
//static void LED_Display(uint8_t LedStatus);
static void Can_Error_Handler(void);
static void CAN1Thread(void const * argument);
/* Private functions ---------------------------------------------------------*/

int createCAN1THREAD(void)
{
	/* Create CAN1 task */
	osThreadDef(Start_CAN1_Thread, CAN1Thread, osPriorityHigh, 0, 1 * configMINIMAL_STACK_SIZE);
	osThreadCreate (osThread(Start_CAN1_Thread), NULL); 
	return 1;
}		
static void CAN1Thread(void const * argument)
{
  // INit ICCP CAN API
	IccpCanApiInit();
	
  /* Configure Key Button */ 
  //BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
  
  /*##-1- Configure the CAN peripheral #######################################*/
  CAN_Config();
  
  /*##-2- Start the Reception process and enable reception interrupt #########*/
  if(HAL_CAN_Receive_IT(&CanHandle, CAN_FIFO0) != HAL_OK)
  {
    /* Reception Error */
    Can_Error_Handler();
  }
  
  /* Infinite loop */
  while(1)
  {
		 CANDoCAN();// check if CAN cmds are in API buffer and execute all in buffer
		
		 /*##- Transmit CAN if Flag SET ###############################*/
     if(b_CANTXFlag)
		 {
			  b_CANTXFlag = 0;
				if(HAL_CAN_Transmit(&CanHandle, 10) != HAL_OK)
        {
          /* Transmition Error */
          Can_Error_Handler();
        }
			}

		osDelay(50);
  } 
}

/**
  * @brief  Configures the CAN.
  * @param  None
  * @retval None
  */
static void CAN_Config(void)
{
	
  CAN_FilterConfTypeDef sFilterConfig;
  static CanTxMsgTypeDef TxMessage;
  static CanRxMsgTypeDef RxMessage;
  
	b_CANTXFlag = 0;
	
  /*##-1- Configure the CAN peripheral #######################################*/
  CanHandle.Instance = CAN1;
  CanHandle.pTxMsg = &TxMessage;
  CanHandle.pRxMsg = &RxMessage;
    
  CanHandle.Init.TTCM = DISABLE;
  CanHandle.Init.ABOM = DISABLE;
  CanHandle.Init.AWUM = DISABLE;
  CanHandle.Init.NART = DISABLE;
  CanHandle.Init.RFLM = DISABLE;
  CanHandle.Init.TXFP = DISABLE;
  CanHandle.Init.Mode = CAN_MODE_NORMAL;
  // vk, set CAN1 Baud Rate to 250K with 45Mhz CANCLK //http://www.bittiming.can-wiki.info/
	CanHandle.Init.SJW = CAN_SJW_1TQ;
  CanHandle.Init.BS1 = CAN_BS1_12TQ;
  CanHandle.Init.BS2 = CAN_BS2_2TQ;
  CanHandle.Init.Prescaler = 12;
  
  if(HAL_CAN_Init(&CanHandle) != HAL_OK)
  {
    /* Initialization Error */
    Can_Error_Handler();
  }

  /*##-2- Configure the CAN Filter ###########################################*/
  sFilterConfig.FilterNumber = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = 0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.BankNumber = 14;
  
  if(HAL_CAN_ConfigFilter(&CanHandle, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    Can_Error_Handler();
  }
      
  /*##-3- Configure Transmission process #####################################*/
  CanHandle.pTxMsg->StdId = 0x321;
  CanHandle.pTxMsg->ExtId = 0x01;
  CanHandle.pTxMsg->RTR = CAN_RTR_DATA;
  CanHandle.pTxMsg->IDE = CAN_ID_EXT;
  CanHandle.pTxMsg->DLC = 6;
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Can_Error_Handler(void)
{
//  while(1)
//  {
//  }
}

/**
  * @brief  Turns ON/OFF the dedicated LED.
  * @param  LedStatus: LED number from 0 to 3
  * @retval None
  */
//void LED_Display(uint8_t LedStatus)
//{
//  /* Turn OFF all LEDs */
//  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
//  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_SET);
//  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_SET);
//  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, GPIO_PIN_SET);
//  
//  switch(LedStatus)
//  {
//    case(1):
//      /* Turn ON LED1 */
//      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
//      break;
//   
//    case(2): 
//      /* Turn ON LED2 */
//      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);
//      break;
// 
//    case(3): 
//      /* Turn ON LED3 */
//      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_RESET);
//      break;

//    case(4): 
//      /* Turn ON LED4 */
//      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, GPIO_PIN_RESET);
//      break;
//		case(15): 
//      /* Turn ON ALL 4 LEDS */
//			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7, GPIO_PIN_RESET);
//			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_RESET);
//      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, GPIO_PIN_RESET);
//      break;
//    default:
//      break;
//  }
//}
/**
  * @brief  Transmission complete callback in non blocking mode 
  * @param  CanHandle: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* CanHandle)
{
	uint8_t lclCanBuff[8];
	int8_t i;
	
	
  /* vk, to receive CANRX we must invoke HAL CANRX ISR */
  if(HAL_CAN_Receive_IT(CanHandle, CAN_FIFO0) != HAL_OK)
  {
    /* Reception Error */
    Can_Error_Handler();
  }
	
	memset(lclCanBuff,0,sizeof(lclCanBuff));//clear buffer for population
	for ( i = 0; i < 8; i++ ) 
	{
			lclCanBuff[i] = CanHandle->pRxMsg->Data[i];	
	}
	
	CanApiEnqueue(CanHandle->pRxMsg->ExtId, lclCanBuff,CanHandle->pRxMsg->DLC); // move data from CANFIFO to CANAPI ring buffer
	
	
	
//  if ((CanHandle->pRxMsg->ExtId == 0x00112201)&&(CanHandle->pRxMsg->IDE == CAN_ID_EXT) && (CanHandle->pRxMsg->DLC == 8))
//  {
//    LED_Display(CanHandle->pRxMsg->Data[0]);
//    ubKeyNumber = CanHandle->pRxMsg->Data[0];
//		GuiState.SysDataPSU0.Irdbk = CanHandle->pRxMsg->Data[0];
//		GuiState.SysDataPSU0.Vrdbk = CanHandle->pRxMsg->Data[1];
//		
//		CanHandle->pTxMsg->Data[0] = CanHandle->pRxMsg->Data[0];
//		CanHandle->pTxMsg->Data[1] = CanHandle->pRxMsg->Data[1];		
//		
//		/*##-3- Start the Transmission process ###############################*/
//		if(HAL_CAN_Transmit(CanHandle, 10) != HAL_OK)
//		{
//			/* Transmition Error */
//			Can_Error_Handler();
//		}
//  }
  
}
/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief CAN MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - NVIC configuration for DMA interrupt request enable
  * @param hcan: CAN handle pointer
  * @retval None
  */
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
  GPIO_InitTypeDef   GPIO_InitStruct;
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* CAN1 Periph clock enable */
  CANx_CLK_ENABLE();
  /* Enable GPIO clock ****************************************/
  CANx_GPIO_CLK_ENABLE();
  
  /*##-2- Configure peripheral GPIO ##########################################*/ 
  /* CAN1 TX GPIO pin configuration */
  GPIO_InitStruct.Pin = CANx_TX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Alternate =  CANx_TX_AF;
  
  HAL_GPIO_Init(CANx_TX_GPIO_PORT, &GPIO_InitStruct);
  
  /* CAN1 RX GPIO pin configuration */
  GPIO_InitStruct.Pin = CANx_RX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Alternate =  CANx_RX_AF;
  
  HAL_GPIO_Init(CANx_TX_GPIO_PORT, &GPIO_InitStruct);
  
  /*##-3- Configure the NVIC #################################################*/
  /* NVIC configuration for CAN1 Reception complete interrupt */
  HAL_NVIC_SetPriority(CANx_RX_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(CANx_RX_IRQn);
}

/**
  * @brief CAN MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO to their default state
  * @param hcan: CAN handle pointer
  * @retval None
  */
void HAL_CAN_MspDeInit(CAN_HandleTypeDef *hcan)
{
  /*##-1- Reset peripherals ##################################################*/
  CANx_FORCE_RESET();
  CANx_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks ################################*/
  /* De-initialize the CAN1 TX GPIO pin */
  HAL_GPIO_DeInit(CANx_TX_GPIO_PORT, CANx_TX_PIN);
  /* De-initialize the CAN1 RX GPIO pin */
  HAL_GPIO_DeInit(CANx_RX_GPIO_PORT, CANx_RX_PIN);
  
  /*##-4- Disable the NVIC for CAN reception #################################*/
  HAL_NVIC_DisableIRQ(CANx_RX_IRQn);
}


/** @Author Vince Keiper
  * @brief CAN TX Buffer Mutator for HAL library.  
  *        This function sets the CanHandle.pTX eid,DLC, and tx data buffer:
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
	
		//b_CANTXFlag = 1;
		
		if(len >8){len = 8;}
		
		for ( i = 0; i < len; i++ ) 
		{
		    CanHandle.pTxMsg->Data[i] = *ptrBuff++;
    }
		CanHandle.pTxMsg->ExtId = id;
		CanHandle.pTxMsg->DLC = len;
		
		b_CANTXFlag = 1;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT AstrodyneTDI inc 2015 *****END OF FILE****/
