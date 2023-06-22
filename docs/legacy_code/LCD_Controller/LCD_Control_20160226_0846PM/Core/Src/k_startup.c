/**
  ******************************************************************************
  * @file    k_startup.c
  * @author  Vince Keiper AstrodyneTDI
  * @version V1.0.0
  * @date    24-June-2015
  * @brief   Kernal Startup Routines.    
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 AstrodyneTDI</center></h2>
  *
  *
  ******************************************************************************  
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dialog.h"
#include "atdi_res.c"

/** @addtogroup CORE
  * @{
  */

/** @defgroup KERNEL_STARTUP
  * @brief Kernel startup routines 
  * @{
  */


/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t idx = 0;

static GUI_CONST_STORAGE unsigned long acWhiteCircle_6x6[] = {
  0xF2FFFFFF, 0x81FFFFFF, 0x26FFFFFF, 0x26FFFFFF, 0x81FFFFFF, 0xF2FFFFFF,
  0x7AFFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x7CFFFFFF,
  0x23FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x22FFFFFF,
  0x23FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x22FFFFFF,
  0x7BFFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x7AFFFFFF,
  0xF1FFFFFF, 0x80FFFFFF, 0x25FFFFFF, 0x25FFFFFF, 0x80FFFFFF, 0xF1FFFFFF
};

static GUI_CONST_STORAGE GUI_BITMAP _bmWhiteCircle_6x6 = {
  6, /* XSize */
  6, /* YSize */
  24, /* BytesPerLine */
  32, /* BitsPerPixel */
  (unsigned char *)acWhiteCircle_6x6,  /* Pointer to picture data */
  NULL,  /* Pointer to palette */
  GUI_DRAW_BMP8888
};

static GUI_CONST_STORAGE unsigned long acWhiteCircle_10x10[] = {
  0xFFFFFFFF, 0xFEFFFFFF, 0xBCFFFFFF, 0x51FFFFFF, 0x1AFFFFFF, 0x1AFFFFFF, 0x51FFFFFF, 0xBCFFFFFF, 0xFEFFFFFF, 0xFFFFFFFF,
  0xFAFFFFFF, 0x85FFFFFF, 0x07FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x07FFFFFF, 0x85FFFFFF, 0xFAFFFFFF,
  0xAEFFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0xAFFFFFFF,
  0x4AFFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x4BFFFFFF,
  0x1BFFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x17FFFFFF,
  0x1BFFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x1AFFFFFF,
  0x4AFFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x4AFFFFFF,
  0xAFFFFFFF, 0x01FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x01FFFFFF, 0xAEFFFFFF,
  0xFAFFFFFF, 0x82FFFFFF, 0x05FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x05FFFFFF, 0x82FFFFFF, 0xFAFFFFFF,
  0xFFFFFFFF, 0xFDFFFFFF, 0xBAFFFFFF, 0x50FFFFFF, 0x19FFFFFF, 0x19FFFFFF, 0x4FFFFFFF, 0xBAFFFFFF, 0xFDFFFFFF, 0xFFFFFFFF
};

static GUI_CONST_STORAGE GUI_BITMAP _bmWhiteCircle_10x10 = {
  10, /* XSize */
  10, /* YSize */
  40, /* BytesPerLine */
  32, /* BitsPerPixel */
  (unsigned char *)acWhiteCircle_10x10,  /* Pointer to picture data */
  NULL,  /* Pointer to palette */
  GUI_DRAW_BMP8888
};

/* Private function prototypes -----------------------------------------------*/
static void BackGroundCallback(WM_MESSAGE * pMsg);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Backbground callbacsk
  * @param  pMsg: pointer to data structure of type WM_MESSAGE
  * @retval None
  */

static void BackGroundCallback(WM_MESSAGE * pMsg) 
{
  uint16_t   xPos, Step = 20, i;
  const GUI_BITMAP * pBm;
  
  switch (pMsg->MsgId) 
  {
  case WM_PAINT:
    GUI_DrawGradientV(0, 0, LCD_GetXSize()- 1, LCD_GetYSize() - 1, GUI_WHITE, GUI_LIGHTBLUE);
    
    GUI_DrawBitmap(&bmAtdi_logo_small_trans, (LCD_GetXSize() - bmAtdi_logo_small_trans.XSize)/2 , (LCD_GetYSize() - bmAtdi_logo_small_trans.YSize)/2); 
    
    for (i = 0, xPos = LCD_GetXSize() / 2 - 2 * Step; i < 5; i++, xPos += Step) 
    {
			// if they are equal show big circle FALSE show little circle
      pBm = (idx == i) ? &_bmWhiteCircle_10x10 : &_bmWhiteCircle_6x6;
      GUI_DrawBitmap(pBm, xPos - pBm->XSize / 2, 250 - pBm->YSize / 2);
    }
    break;
    
  default:
    WM_DefaultProc(pMsg); 
  } 
}

/**
  * @brief  Startup
  * @param  None
  * @retval None
  */
void k_StartUp(void)
{
  uint8_t loop;
  GUI_RECT Rect = {100, 245, 380, 255}; 

  WM_SetCallback(WM_HBKWIN, BackGroundCallback);

  loop = 48;
  while (loop--)
  {
		//16 - 48 
    idx = (16- loop) % 8;

    WM_InvalidateArea(&Rect);
    
    GUI_Delay(100);
  }
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
