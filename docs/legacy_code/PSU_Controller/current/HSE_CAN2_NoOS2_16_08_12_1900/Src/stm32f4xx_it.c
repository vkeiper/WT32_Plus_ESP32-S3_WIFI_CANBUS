/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "selftest.h"
#include "iccpfuncs.h"
#include "iccp_canfuncs.h"

/* USER CODE BEGIN 0 */
extern CAN_HandleTypeDef CanHandle;
extern ADC_HandleTypeDef hADC1;
extern DMA_HandleTypeDef hDMA_ADC1;
extern TIM_HandleTypeDef hTIM3;
/* USER CODE END 0 */


/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

uint16_t    milliseconds = 0;
uint8_t     seconds = 0, minutes = 0, hours = 0;

/**
 *  @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
    /* USER CODE BEGIN SysTick_IRQn 0 */

    /* USER CODE END SysTick_IRQn 0 */
    HAL_IncTick();
//    HAL_SYSTICK_IRQHandler();
    /* USER CODE BEGIN SysTick_IRQn 1 */
    milliseconds++;

    if (milliseconds > 999)
    {
        seconds++;
        milliseconds = 0;
    }
    else
    {
        __NOP();
    }

    if (seconds > 59)
    {
        minutes++;
        seconds = 0;
    }
    else
    {
        __NOP();
    }    

    if (minutes > 59)
    {
        hours++;
        minutes = 0;
    }
    else
    {
        __NOP();
    }

////    if (hours > 7)
////    {
////        if (AppState == TMRSELFTEST)
////        {
////            __NOP();
////        }
////        else
////        {
////            AppState = TMRSELFTEST;
////        }
////        hours = 0;
////    }
////    else
////    {
////        __NOP();
////    }
    /* USER CODE END SysTick_IRQn 1 */
}
void CAN1_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&CanHandle);
}
/**
 *  @brief  This function handles CAN2 RX0 interrupt request.
 *  @param  None
 *  @retval None
 */
void CAN2_RX0_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&CanHandle);
}
/**
 *  @brief  This function handles CAN1 RX1 interrupt request.
 *  @param  None
 *  @retval None
 */
void CAN1_RX1_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&CanHandle);
}
/**
 *  @brief  This function handles CAN2 RX1 interrupt request.
 *  @param  None
 *  @retval None
 */
void CAN2_RX1_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&CanHandle);
}
/**
 *  @brief  This function handles CAN1 TX interrupt request.
 *  @param  None
 *  @retval None
 */
void CAN1_TX_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&CanHandle);
}

/**
 *  @brief  This function handles CAN2 TX interrupt request.
 *  @param  None
 *  @retval None
 */
void CAN2_TX_IRQHandler(void)
{
    HAL_CAN_IRQHandler(&CanHandle);
}
/**
 *  @brief  This function handles ADC interrupt request
 *  @param  None
 *  @retval None
 */
void ADC_IRQHandler(void)
{
    HAL_ADC_IRQHandler(&hADC1);
}
/**
 *  @brief  This function handles DMA interrupt request
 *  @param  None
 *  @retval None
 */
void DMA2_Stream0_IRQHandler(void)
{
  HAL_DMA_IRQHandler(hADC1.DMA_Handle);
}
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */

  /* USER CODE END TIM3_IRQn 0 */
  HAL_TIM_IRQHandler(&hTIM3);
  /* USER CODE BEGIN TIM3_IRQn 1 */

  /* USER CODE END TIM3_IRQn 1 */
}
/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/* ================================================================================================
 *  FINIS
 * ==============================================================================================*/

