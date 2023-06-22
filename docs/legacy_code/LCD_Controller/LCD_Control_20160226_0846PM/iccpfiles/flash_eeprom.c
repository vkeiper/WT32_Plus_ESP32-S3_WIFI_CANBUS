/**
  ******************************************************************************
  * @file    flash_eeprom.c
  * @author  Vince Keiper
  * @version V1.0.0
  * @date    20-Spet-2015   
  * @brief   CPU benchmark resources
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 AstrodyneTDI</center></h2>
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"



#pragma location=0x800C000  
volatile const uint8_t flash_data[320];

/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t flash_status;

void save_data_to_flash() {
  
  flash_status = FLASH_COMPLETE;
  
  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR |FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR); 
  flash_status = FLASH_EraseSector(FLASH_Sector_3, VoltageRange_3);
  
  if (flash_status != FLASH_COMPLETE) {
    FLASH_Lock();
    return;
  }
  
  uint8_t* address = &flash_data[0];
  
  //program first run status bit
  flash_status = FLASH_ProgramByte((uint32_t)address, 0x00);
  address += 4;
  if (flash_status != FLASH_COMPLETE) {
    FLASH_Lock();
    return;
  }

}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT 2015 AstrodyneTDI *****END OF FILE****/
