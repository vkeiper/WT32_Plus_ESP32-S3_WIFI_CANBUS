#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common

extern  ADC_HandleTypeDef       hADC3;

void ADC_IRQHandler(void)
{
  HAL_ADC_IRQHandler(&hADC3);
}

