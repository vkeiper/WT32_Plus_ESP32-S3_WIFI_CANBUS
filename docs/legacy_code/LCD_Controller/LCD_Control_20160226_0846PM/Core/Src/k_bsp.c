/**
  ******************************************************************************
  * @file    k_bsp.c
  * @author  Vince Keiper AstrodyneTDI
  * @version V1.0.0
  * @date    24-June-2015
  * @brief   Kernal BSP for the STM32F4291-EVAL pcb.    
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 AstrodyneTDI</center></h2>
  *
  *
  ******************************************************************************  
  */ 

/* Includes ------------------------------------------------------------------*/
#include "k_bsp.h"
#include "k_calibration.h"
#include "GUI.h"
#include "DIALOG.h"
#include "globals.h"

/** @addtogroup CORE
  * @{
  */

/** @defgroup KERNEL_BSP
  * @brief Kernel BSP routines
  * @{
  */


/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes LEDs, SDRAM, touch screen, CRC and SRAM.
  * @param  None 
  * @retval None
  */
void k_BspInit(void)
{
  /* Configure LED1, LED2, LED3 and LED4 */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED3);
  BSP_LED_Init(LED4);  
  
	// Init NOR flash 
	if(BSP_NOR_Init() != NOR_STATUS_OK)
  {
    while(1);
  }
	
  /* Initialize the SDRAM */
  BSP_SDRAM_Init();
  
  /* Initialize the Touch screen */
  BSP_TS_Init(480, 272);
  
  /* Enable CRC to Unlock GUI */
	/*  vk, STemWin is only unlocked for ST micros it does this by having CRC running */
  __HAL_RCC_CRC_CLK_ENABLE();
  
  /* Enable Back up SRAM */
  __HAL_RCC_BKPSRAM_CLK_ENABLE();
	
	/* Enable IO Expander interrupt for ETH MII pin */
//  BSP_IO_ConfigPin(MII_INT_PIN, IO_MODE_IT_FALLING_EDGE);
  
  
}

/**
  * @brief  Initializes BSP Audio
  * @param  None 
  * @retval None
  */
void k_BspAudioInit(void)
{
  BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_AUTO, AUDIO_DEFAULT_VOLUME, I2S_AUDIOFREQ_44K);
  BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);
}


/**
  * @brief  Read the coordinate of the point touched and assign their
  *         value to the variables u32_TSXCoordinate and u32_TSYCoordinate
  * @param  None
  * @retval None
  */
void k_TouchUpdate(void)
{
  GUI_PID_STATE TS_State;
  static TS_StateTypeDef prev_state;
  __IO TS_StateTypeDef  ts;
  uint16_t xDiff, yDiff;  
  
  BSP_TS_GetState((TS_StateTypeDef *)&ts);
  
  TS_State.Pressed = ts.TouchDetected;

  xDiff = (prev_state.x > ts.x) ? (prev_state.x - ts.x) : (ts.x - prev_state.x);
  yDiff = (prev_state.y > ts.y) ? (prev_state.y - ts.y) : (ts.y - prev_state.y);
  
  if((prev_state.TouchDetected != ts.TouchDetected )||
     (xDiff > 3 )||
       (yDiff > 3))
  {
    prev_state.TouchDetected = ts.TouchDetected;
    
    if((ts.x != 0) &&  (ts.y != 0)) 
    {
      prev_state.x = ts.x;
      prev_state.y = ts.y;
    }
      
    if(k_CalibrationIsDone())
    {
      TS_State.Layer = 1;
      TS_State.x = k_CalibrationGetX (prev_state.x);
      TS_State.y = k_CalibrationGetY (prev_state.y);
    }
    else
    {
      TS_State.Layer = 1;
      TS_State.x = prev_state.x;
      TS_State.y = prev_state.y;
    }
    
    GUI_TOUCH_StoreStateEx(&TS_State);
		gbl_xpos = TS_State.x;
		gbl_ypos = TS_State.y;
  }
}
//
// vk, I wrote this to fix a problem with the original not always sensing a release
//
void BSP_Pointer_Update(void)
{
	GUI_PID_STATE TS_State;
  static TS_StateTypeDef prev_state;
  //TS_StateTypeDef  ts;
  //uint16_t xDiff, yDiff;  
  //BSP_TS_GetState(&ts);
	__IO TS_StateTypeDef  ts;
  uint16_t xDiff, yDiff;  
  
  BSP_TS_GetState((TS_StateTypeDef *)&ts);
	
  
  TS_State.Pressed = ts.TouchDetected;
  
  xDiff = (prev_state.x > ts.x) ? (prev_state.x - ts.x) : (ts.x - prev_state.x);
  yDiff = (prev_state.y > ts.y) ? (prev_state.y - ts.y) : (ts.y - prev_state.y);
  
  if(ts.TouchDetected)
  {
    if((prev_state.TouchDetected != ts.TouchDetected )||
       (xDiff > 3 )||
         (yDiff > 3))
    {
      prev_state = ts;
			
			if(k_CalibrationIsDone())
			{
				TS_State.Layer = 1;
				TS_State.x = k_CalibrationGetX (ts.x);
				TS_State.y = k_CalibrationGetY (ts.y);
			}
			else
			{
				TS_State.Layer = 1;
				TS_State.x = ts.x;
				TS_State.y = ts.y;
			}
      
      GUI_TOUCH_StoreStateEx(&TS_State);
    }
  }
	else //if released store NOT PRESSED
	{
			TS_State.Layer = 1;
      TS_State.x = -1;
      TS_State.y = -1;
		  GUI_TOUCH_StoreStateEx(&TS_State);
	}
	gbl_xpos = TS_State.x;
	gbl_ypos = TS_State.y;
}


/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
