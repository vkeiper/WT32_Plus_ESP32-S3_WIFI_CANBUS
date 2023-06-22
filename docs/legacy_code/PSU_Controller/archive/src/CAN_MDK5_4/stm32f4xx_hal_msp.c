#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common

void HAL_MspInit(void)
{
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
  HAL_NVIC_SetPriority(ADC_IRQn, 1, 1);

  HAL_NVIC_EnableIRQ(SysTick_IRQn);
  HAL_NVIC_EnableIRQ(ADC_IRQn);
}
void HAL_ADC_MspInit(ADC_HandleTypeDef *hADC3)
{
    GPIO_InitTypeDef    GPIO_InitStruct;

    if (hADC3->Instance == ADC3) {

    /* GPIO Ports Clock Enable */
    __GPIOC_CLK_ENABLE();

    /* GPIO Ports Clock Enable */
    __GPIOF_CLK_ENABLE();

    /* Peripheral clock enable */
    __ADC3_CLK_ENABLE();

    /* ADC3 GPIO Configuration: {PC0:ADC3_IN10, PC1:ADC3_IN11, PC2:ADC3_IN12, PC3:ADC3_IN13} */
    GPIO_InitStruct.Pin       = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode      = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* ADC3 GPIO Configuration: {PF6:ADC3_IN4, PF7:ADC3_IN5, PF8:ADC3_IN6, PF9:ADC_IN7, 
                                 PF10:ADC3_IN8, PF3:ADC3_IN9} */
    GPIO_InitStruct.Pin       = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 
                              | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode      = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hADC3)
{
  if (hADC3->Instance == ADC3) {
    
    __ADC3_CLK_DISABLE();

    __ADC_FORCE_RESET();
    __ADC_RELEASE_RESET();

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6
                    | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);
  }
}
void HAL_DAC_MspInit(DAC_HandleTypeDef *hDAC)
{
  GPIO_InitTypeDef    GPIO_InitStructDAC;

  if (hDAC->Instance == DAC) {
    
    __GPIOA_CLK_ENABLE();
  
    __DAC_CLK_ENABLE();

    GPIO_InitStructDAC.Pin    = GPIO_PIN_4;
    GPIO_InitStructDAC.Mode   = GPIO_MODE_ANALOG;
    GPIO_InitStructDAC.Pull   = GPIO_NOPULL;
    GPIO_InitStructDAC.Speed  = GPIO_SPEED_MEDIUM;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructDAC);
  }
}
void HAL_DAC_MspDeInit(DAC_HandleTypeDef *hDAC)
{
  if (hDAC->Instance == DAC) {

    __DAC_CLK_DISABLE();

    __DAC_FORCE_RESET();
    __DAC_RELEASE_RESET();

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);
  }
}

