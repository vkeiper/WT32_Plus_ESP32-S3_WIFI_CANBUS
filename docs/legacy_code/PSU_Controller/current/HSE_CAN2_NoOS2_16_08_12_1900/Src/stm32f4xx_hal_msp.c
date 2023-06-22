/**
  ******************************************************************************
  * File Name          : stm32f4xx_hal_msp.c
  * Description        : This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2015 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"

extern void myErr_Handler(void);

extern  ADC_HandleTypeDef hADC1;
extern  DAC_HandleTypeDef hDAC1;
extern  DMA_HandleTypeDef hDMA_ADC1;
extern  TIM_HandleTypeDef hTIM3;

/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
void HAL_MspInit(void)
{
    /* USER CODE BEGIN MspInit 0 */

    /* USER CODE END MspInit 0 */

    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    /* System interrupt init*/
    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SysTick_IRQn);

    /* USER CODE BEGIN MspInit 1 */

    /* USER CODE END MspInit 1 */
}
/* ================================================================================================
 *  @brief  ADC MSP Initializtion
 *          This function configures the hardware resources used in this example:
 *            - Peripheral's clock enable
 *            - Peripheral's GPIO configuration
 *  @param  hADC: ADC handle pointer
 *  @retval None
 * ==============================================================================================*/
void HAL_ADC_MspInit(ADC_HandleTypeDef *hADC1)
{
    GPIO_InitTypeDef    GPIO_InitStructADC;

    /* Enable ADC(1) clock */
    __HAL_RCC_ADC1_CLK_ENABLE();

    /* Enable DMA(2) clock */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /** ADC1 GPIO Configuration: 
     *  PC0 ---> ADC1_IN10: Reference cell 1
     *  PC1 ---> ADC1_IN11: Reference cell 2
     *  PC2 ---> ADC1_IN12: Reference cell 3
     *  PC3 ---> ADC1_IN13: Reference cell 4
     *  PC4 ---> ADC1_IN14: Current monitor X
     *  PC5 ---> ADC1_IN15: Current monitor Y
     */
    GPIO_InitStructADC.Pin      = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2
                                | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStructADC.Mode     = GPIO_MODE_ANALOG;
    GPIO_InitStructADC.Pull     = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructADC);

    /** ADC1 GPIO Configuration: 
     *  PA1 ---> ADC1_IN1:  Voltage monitor
     *  PA2 ---> ADC1_IN2:  Current monitor 1
     *  PA3 ---> ADC1_IN3:  Current monitor 2
     *  PA6 ---> ADC1_IN6:  SystemID/PSU addr
     *  PA7 ---> ADC1_IN7:  Temperature
     */
    GPIO_InitStructADC.Pin      = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3
                                | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStructADC.Mode     = GPIO_MODE_ANALOG;
    GPIO_InitStructADC.Pull     = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructADC);

    hDMA_ADC1.Instance                  = DMA2_Stream0;
    hDMA_ADC1.Init.Channel              = DMA_CHANNEL_0;
    hDMA_ADC1.Init.Direction            = DMA_PERIPH_TO_MEMORY;
    hDMA_ADC1.Init.PeriphInc            = DMA_PINC_DISABLE;
    hDMA_ADC1.Init.MemInc               = DMA_MINC_ENABLE;
    hDMA_ADC1.Init.PeriphDataAlignment  = DMA_MDATAALIGN_HALFWORD;
    hDMA_ADC1.Init.MemDataAlignment     = DMA_PDATAALIGN_HALFWORD;
    hDMA_ADC1.Init.Mode                 = DMA_CIRCULAR;
    hDMA_ADC1.Init.Priority             = DMA_PRIORITY_LOW;
    hDMA_ADC1.Init.FIFOMode             = DMA_FIFOMODE_DISABLE;
    
    if (HAL_DMA_Init(&hDMA_ADC1) != HAL_OK)
    {
        myErr_Handler();
    } 
    else 
    {
        __NOP();
    }

    __HAL_LINKDMA(hADC1, DMA_Handle, hDMA_ADC1);

    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 2, 0);   
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hADC1)
{
    if (hADC1->Instance == ADC1)
    {
        /* Disable ADC clock */  
        __HAL_RCC_ADC1_CLK_DISABLE();

        /* Disable DMA(2) clock */
        __HAL_RCC_DMA2_CLK_DISABLE();

        /** ADC1 GPIO Configuration: 
         *  PC0 ---> ADC1_IN10:  Reference cell 1
         *  PC1 ---> ADC1_IN11:  Reference cell 2
         *  PC2 ---> ADC1_IN12:  Reference cell 3
         *  PC3 ---> ADC1_IN13:  Reference cell 4 
         */
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | 
                        GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);

        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3
                        | GPIO_PIN_6 | GPIO_PIN_7);
        
        __HAL_RCC_ADC_FORCE_RESET();
        __HAL_RCC_ADC_RELEASE_RESET();

        HAL_DMA_DeInit(&hDMA_ADC1);

        HAL_NVIC_DisableIRQ(DMA2_Stream0_IRQn);
    } 
    else 
    {
        myErr_Handler();
    }
}
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
void HAL_DAC_MspInit(DAC_HandleTypeDef *hDAC)
{
    GPIO_InitTypeDef  GPIO_InitStructDAC;

    if (hDAC->Instance == DAC) 
    {
        /* Enable DAC clock */
        __HAL_RCC_DAC_CLK_ENABLE();

        /* PA4 ---> DAC_OUT1 */
        GPIO_InitStructDAC.Pin    = GPIO_PIN_4;
        GPIO_InitStructDAC.Mode   = GPIO_MODE_ANALOG;
        GPIO_InitStructDAC.Pull   = GPIO_NOPULL;
        GPIO_InitStructDAC.Speed  = GPIO_SPEED_LOW;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStructDAC);
    } 
    else 
    {
        myErr_Handler();
    }
}
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
void HAL_DAC_MspDeInit(DAC_HandleTypeDef *hDAC)
{
    if (hDAC->Instance == DAC) 
    {
        /* Disable DAC clock */
        __HAL_RCC_DAC_CLK_DISABLE();

        /* PA4 ---> DAC_OUT1 */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);

        __HAL_RCC_DAC_FORCE_RESET();
        __HAL_RCC_DAC_RELEASE_RESET();
    } 
    else 
    {
        myErr_Handler();
    }
}
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        /* USER CODE BEGIN 1 */

        /* USER CODE END 1 */
        __HAL_RCC_TIM3_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM3_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(TIM3_IRQn);
    }
}
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspDeInit 0 */

  /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();

    /* Peripheral interrupt DeInit*/
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
  }
  /* USER CODE BEGIN TIM3_MspDeInit 1 */

  /* USER CODE END TIM3_MspDeInit 1 */
}
/**
  * @}
  */
/**
  * @}
  */
/* ================================================================================================
 *  FINIS
 * ==============================================================================================*/


