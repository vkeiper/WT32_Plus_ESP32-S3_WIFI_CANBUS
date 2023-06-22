/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
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
/* Includes -----------------------------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <stm32f4xx.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_conf.h>
#include "main.h"
#include "selftest.h"
#include "iccpfuncs.h"
#include "iccp_canfuncs.h"
#include "iccp_can_stm32f446.h"

/* USER CODE END Includes */

#define CURRENTOFFSET 426UL

/* Private variables ----------------------------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* Private variables ----------------------------------------------------------------------------*/
/* USER CODE END PV */

/* Handles --------------------------------------------------------------------------------------*/
/* USER CODE BEGIN Handles */
/* Handles --------------------------------------------------------------------------------------*/

            ADC_HandleTypeDef   hADC1;
    extern  CAN_HandleTypeDef   CanHandle;
            DAC_HandleTypeDef   hDAC1;
            DMA_HandleTypeDef   hDMA_ADC1;
            TIM_HandleTypeDef   hTIM3;
    
/* USER CODE END Handles */

/* Private function prototypes ================================================================= */
/* USER CODE BEGIN PFP */
/* Private function prototypes ================================================================= */
/* USER CODE END PFP */
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

                uint8_t    AppState;
    volatile    uint16_t   ADC_ConvertedData[11] = {0};

/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
void ClearADC_Params(void)
{
    SysData.AdcSmplIndx         = 1;
    SysData.nodeadr_adc         = 0;
    SysData.nodeadr_adc_flag    = 0;
    SysData.RefCell[0].sum      = 0;
    SysData.RefCell[1].sum      = 0;
    SysData.RefCell[2].sum      = 0;
    SysData.RefCell[3].sum      = 0;
    SysData.Vrdbk.sum           = 0;
    SysData.Irdbk[0].sum        = 0;
    SysData.Irdbk[1].sum        = 0;
    SysData.Irdbk[2].sum        = 0;
    SysData.Irdbk[3].sum        = 0;
}
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
void DataCollection(void)
{
    if (SysData.AdcSmplIndx < MAX_SAMPLES)
    {
        /* Reference Cell 1 */
        SysData.RefCell[0].collection[SysData.AdcSmplIndx] = SysData.RefCell[0].collection[0];
        SysData.RefCell[0].sum += SysData.RefCell[0].collection[SysData.AdcSmplIndx];

        /* Reference Cell 2 */
        SysData.RefCell[1].collection[SysData.AdcSmplIndx] = SysData.RefCell[1].collection[0];
        SysData.RefCell[1].sum += SysData.RefCell[1].collection[SysData.AdcSmplIndx];

        /* Reference Cell 3 */
        SysData.RefCell[2].collection[SysData.AdcSmplIndx] = SysData.RefCell[2].collection[0];
        SysData.RefCell[2].sum += SysData.RefCell[2].collection[SysData.AdcSmplIndx];

        /* Reference Cell 4 */
        SysData.RefCell[3].collection[SysData.AdcSmplIndx] = SysData.RefCell[3].collection[0];
        SysData.RefCell[3].sum += SysData.RefCell[3].collection[SysData.AdcSmplIndx];

        /* Voltage monitor */
        SysData.Vrdbk.collection[SysData.AdcSmplIndx] = SysData.Vrdbk.collection[0];
        SysData.Vrdbk.sum += SysData.Vrdbk.collection[SysData.AdcSmplIndx];

        /* Current monitor 1 */
        SysData.Irdbk[0].collection[SysData.AdcSmplIndx] = SysData.Irdbk[0].collection[0];
        SysData.Irdbk[0].sum += SysData.Irdbk[0].collection[SysData.AdcSmplIndx];

        /* Current monitor 2 */
        SysData.Irdbk[1].collection[SysData.AdcSmplIndx] = SysData.Irdbk[1].collection[0];
        SysData.Irdbk[1].sum += SysData.Irdbk[1].collection[SysData.AdcSmplIndx];

        /* Node address */
        
        /* Temperature */
    
        /* Current monitor X */
        SysData.Irdbk[2].collection[SysData.AdcSmplIndx] = SysData.Irdbk[2].collection[0];
        SysData.Irdbk[2].sum += SysData.Irdbk[2].collection[SysData.AdcSmplIndx];

        /* Current monitor Y */
        SysData.Irdbk[3].collection[SysData.AdcSmplIndx] = SysData.Irdbk[3].collection[0];
        SysData.Irdbk[3].sum += SysData.Irdbk[3].collection[SysData.AdcSmplIndx];
    } 
    else 
    {
        if (AppState == RUNAPP) //// Scaling required during normal ICCP operation
        {
            SysData.RefCell[0].avg  = SCALE_CPOT_RDBK(SysData.RefCell[0].sum >> 7);
            SysData.RefCell[1].avg  = SCALE_CPOT_RDBK(SysData.RefCell[1].sum >> 7);
            SysData.RefCell[2].avg  = SCALE_CPOT_RDBK(SysData.RefCell[2].sum >> 7);
            SysData.RefCell[3].avg  = SCALE_CPOT_RDBK(SysData.RefCell[3].sum >> 7);

            SysData.Vrdbk.avg       = SCALE_CV_RDBK(SysData.Vrdbk.sum >> 7);   

            SysData.Irdbk[0].avg    = SCALE_CI_RDBK(SysData.Irdbk[0].sum >> 7);
            SysData.Irdbk[1].avg    = SCALE_CI_RDBK(SysData.Irdbk[1].sum >> 7);
            SysData.Irdbk[2].avg    = SCALE_CI_RDBK(SysData.Irdbk[2].sum >> 7);
            SysData.Irdbk[3].avg    = SCALE_CI_RDBK(SysData.Irdbk[3].sum >> 7);
            
            SysData.IrdNode         = SysData.Irdbk[2].avg + SysData.Irdbk[3].avg;

            ClearADC_Params();
        } 
        else    //// No scaling required during precision voltage self test
        {
            SysData.RefCell[0].avg  = (SysData.RefCell[0].sum >> 7);
            SysData.RefCell[1].avg  = (SysData.RefCell[1].sum >> 7);
            SysData.RefCell[2].avg  = (SysData.RefCell[2].sum >> 7);
            SysData.RefCell[3].avg  = (SysData.RefCell[3].sum >> 7);

            SysData.Vrdbk.avg       = (SysData.Vrdbk.sum >> 7);   

            SysData.Irdbk[0].avg    = (SysData.Irdbk[0].sum >> 7);
            SysData.Irdbk[1].avg    = (SysData.Irdbk[1].sum >> 7);
            SysData.Irdbk[2].avg    = SCALE_CI_RDBK(SysData.Irdbk[2].sum >> 7);
            SysData.Irdbk[3].avg    = SCALE_CI_RDBK(SysData.Irdbk[3].sum >> 7);

            ClearADC_Params();
        }
    } 
}
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim3)
{
    DataCollection();
    SysData.AdcSmplIndx++;
    SelfTest.AdcSmplIndx++;
}
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hADC1)
{
    SysData.nodeadr_adc                 = ADC_ConvertedData[0];
    SysData.nodeadr_adc_flag++;
    
    SysData.RefCell[0].collection[0]    = ADC_ConvertedData[1];
    SysData.RefCell[1].collection[0]    = ADC_ConvertedData[2];
    SysData.RefCell[2].collection[0]    = ADC_ConvertedData[3];
    SysData.RefCell[3].collection[0]    = ADC_ConvertedData[4];

    // No scaling required during Self test!!!
    if ((RUNSELFTEST == AppState) || (TMRSELFTEST == AppState))
    {
        SysData.Irdbk[0].collection[0] = ADC_ConvertedData[5];
        SysData.Irdbk[1].collection[0] = ADC_ConvertedData[6];
    }
    else
    {
        /* Elimination of HALL effect current sensor offset. */
        if (ADC_ConvertedData[5] > CURRENTOFFSET) 
        {
            SysData.Irdbk[0].collection[0] = ADC_ConvertedData[5];
            SysData.Irdbk[0].collection[0] -= CURRENTOFFSET;
        } 
        else 
        {
            SysData.Irdbk[0].collection[0] = 0;
        }

        if (ADC_ConvertedData[6] > CURRENTOFFSET)
        {
            SysData.Irdbk[1].collection[0] = ADC_ConvertedData[6];
            SysData.Irdbk[1].collection[0] -= CURRENTOFFSET; 
        }
        else
        {
            SysData.Irdbk[1].collection[0] = 0;
        }
    }

    SysData.Vrdbk.collection[0]         = ADC_ConvertedData[7];

    SysData.Temperature                 = ADC_ConvertedData[8];

    SysData.Irdbk[2].collection[0]      = ADC_ConvertedData[9];
    SysData.Irdbk[3].collection[0]      = ADC_ConvertedData[10];
}
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hADC1)
{
    HAL_ADC_Stop_DMA(hADC1);
    myErr_Handler();
}
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
void GPIO_Defaults(void)
{
    /* Inhibit Off */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
 
    /* Disable multiplexor */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
    
    SysData.AlmReg                  = 0x0000UL;
    SysData.TestStaReg              = 0x0000UL;
    SysData.pflparams.tsts_active   = 0x00UL;
}
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
void myErr_Handler()
{
    for (;;)
    {   /* If in fault, blink LED every 100ms */
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        HAL_Delay(100);
    }
}
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
int main(void)
{
    uint8_t retval = 0;

    /*  Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configuration of system peripherals */
    SystemClock_Config();
    GPIO_Config();  
    ADC_Config();
    DAC_Config();
    TIM3_Config();

    /* ICCP system state defaults */
    ClearADC_Params();
    GPIO_Defaults();
    
    /* Initialize Iccp */
    Init_Iccp();

    /* Configure the CANBUS peripherals */
    CAN1_Init();

    /* Power up; run ICCP system Self test! */
    AppState = RUNAPP;
//    AppState = RUNSELFTEST;

    while (1)
    {
        CAN_Run();

        switch (AppState)
        {
            case RUNAPP:            /* 0x00UL */
                TaskDispatch();
                break;

            case SAFEMODE:          /* 0x01UL */
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);   // Faulty rectifier
                break;

            default:
                /* Execute if AppState = RUNSELFTEST or TMRSELFTEST */
                retval = RunSelfTest();
                if (RETVAL_DONE == retval) 
                {
                    AppState = RUNAPP;
                } 
                else if (RETVAL_FAILED == retval) 
                {
                    AppState = SAFEMODE;
                } 
                else 
                {   
                    /* Closing else clause */
                    /* RETVAL_NOTDONE {0x02UL} */
                    __NOP();
                }
                break;
        }
    }     
}
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;

    __PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState        = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState    = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM        = 5;
    RCC_OscInitStruct.PLL.PLLN        = 180;
    RCC_OscInitStruct.PLL.PLLP        = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ        = 2;
    RCC_OscInitStruct.PLL.PLLR        = 2;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    HAL_PWREx_ActivateOverDrive();

    RCC_ClkInitStruct.ClockType       = RCC_CLOCKTYPE_SYSCLK 
                                      | RCC_CLOCKTYPE_HCLK
                                      | RCC_CLOCKTYPE_PCLK1
                                      | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider   = RCC_SYSCLK_DIV1;            // 180 MHz
    RCC_ClkInitStruct.APB1CLKDivider  = RCC_HCLK_DIV4;              // 45 MHz
    RCC_ClkInitStruct.APB2CLKDivider  = RCC_HCLK_DIV2;              // 90 MHz   
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
}
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
static void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /**
     *  General purpose I/O configuration Port B
     *  PB0     ---> Inhibit
     */
    GPIO_InitStruct.Pin     = GPIO_PIN_0;
    GPIO_InitStruct.Mode    = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pin     = GPIO_PULLUP;
    GPIO_InitStruct.Speed   = GPIO_SPEED_FAST;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);  

    /**
     *  General purpose I/O configuration Port B
     *  PB1     ---> AC OK
     *  PB10    ---> FRONT DOOR
     *  PB12    ---> AC SURGE
     *  PB13    ---> DC SURGE
     *  PB14    ---> DC OK
     */
    GPIO_InitStruct.Pin     = GPIO_PIN_1 | GPIO_PIN_10 | GPIO_PIN_12
                            | GPIO_PIN_13 | GPIO_PIN_14;
    GPIO_InitStruct.Mode    = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin     = GPIO_NOPULL;
    GPIO_InitStruct.Speed   = GPIO_SPEED_FAST;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);  

    /**
     *  General purpose I/0 configuration Port C
     *  PC6     ---> Multiplexor inhibit
     *  PC7     ---> Multiplexor select A
     *  PC8     ---> Multiplexor select B
     *  PC9     ---> Multiplexor select C
     *  PC13    ---> Heart beat LED
     */
    GPIO_InitStruct.Pin         = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8
                                | GPIO_PIN_9 | GPIO_PIN_13;
    GPIO_InitStruct.Mode        = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull        = GPIO_NOPULL;
    GPIO_InitStruct.Speed       = GPIO_SPEED_FAST;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
static void ADC_Config(void)
{
    ADC_ChannelConfTypeDef  sConfigADC;
  
    /* Configure the global features of the ADC
      (Clock (15 Mhz), Resolution, Data Alignment and number of conversion) */
    hADC1.Instance                      = ADC1;
    hADC1.Init.ClockPrescaler           = ADC_CLOCKPRESCALER_PCLK_DIV8;     // 11.25 MHz
    hADC1.Init.Resolution               = ADC_RESOLUTION12b;
    hADC1.Init.DataAlign                = ADC_DATAALIGN_RIGHT;
    hADC1.Init.ScanConvMode             = ENABLE;
    hADC1.Init.EOCSelection             = DISABLE;
    hADC1.Init.ContinuousConvMode       = ENABLE;
    hADC1.Init.DMAContinuousRequests    = ENABLE;
    hADC1.Init.NbrOfConversion          = 11;
    hADC1.Init.DiscontinuousConvMode    = DISABLE;
    hADC1.Init.NbrOfDiscConversion      = 0;
    hADC1.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hADC1.Init.ExternalTrigConv         = ADC_EXTERNALTRIGCONV_T1_CC1;

    if (HAL_ADC_Init(&hADC1) != HAL_OK)
    {
        myErr_Handler();
    } 
    else 
    {
        __NOP();
    }
    
    /* Configure ADC channel PA14, PSU address */
    sConfigADC.Channel       = ADC_CHANNEL_6;
    sConfigADC.Rank          = 1;
    sConfigADC.SamplingTime  = ADC_SAMPLE_CYCLES;
    sConfigADC.Offset        = 0;

    if (HAL_ADC_ConfigChannel(&hADC1, &sConfigADC) != HAL_OK)
    {
        myErr_Handler();
    } 
    else 
    {
        __NOP();
    }

    /* Configure ADC channel PC0, REF_CELL_1 */
    sConfigADC.Channel       = ADC_CHANNEL_10;
    sConfigADC.Rank          = 2;
    sConfigADC.SamplingTime  = ADC_SAMPLE_CYCLES;
    
    if (HAL_ADC_ConfigChannel(&hADC1, &sConfigADC) != HAL_OK)
    {
        myErr_Handler();
    } 
    else 
    {
        __NOP();
    }

    /* Configure ADC channel PC1, REF_CELL_2 */
    sConfigADC.Channel       = ADC_CHANNEL_11;
    sConfigADC.Rank          = 3;
    sConfigADC.SamplingTime  = ADC_SAMPLE_CYCLES;

    if (HAL_ADC_ConfigChannel(&hADC1, &sConfigADC) != HAL_OK)
    {
        myErr_Handler();
    } 
    else 
    {
        __NOP();
    }

    /* Configure ADC channel PC2, REF_CELL_3 */
    sConfigADC.Channel       = ADC_CHANNEL_12;
    sConfigADC.Rank          = 4;
    sConfigADC.SamplingTime  = ADC_SAMPLE_CYCLES;

    if (HAL_ADC_ConfigChannel(&hADC1, &sConfigADC) != HAL_OK)
    {
        myErr_Handler();
    } 
    else 
    {
        __NOP();
    }

    /* Configure ADC channel PC3, REF_CELL_4 */
    sConfigADC.Channel       = ADC_CHANNEL_13;
    sConfigADC.Rank          = 5;
    sConfigADC.SamplingTime  = ADC_SAMPLE_CYCLES;

    if (HAL_ADC_ConfigChannel(&hADC1, &sConfigADC) != HAL_OK)
    {
        myErr_Handler();
    } 
    else 
    {
        __NOP();
    }

    /* Configure ADC channel PA2, IMON_1 */
    sConfigADC.Channel       = ADC_CHANNEL_2;
    sConfigADC.Rank          = 6;
    sConfigADC.SamplingTime  = ADC_SAMPLE_CYCLES;

    if (HAL_ADC_ConfigChannel(&hADC1, &sConfigADC) != HAL_OK)
    {
        myErr_Handler();
    } 
    else 
    {
        __NOP();
    }

    /* Configure ADC channel PA3, IMON_2 */
    sConfigADC.Channel       = ADC_CHANNEL_3;
    sConfigADC.Rank          = 7;
    sConfigADC.SamplingTime  = ADC_SAMPLE_CYCLES;

    if (HAL_ADC_ConfigChannel(&hADC1, &sConfigADC) != HAL_OK)
    {   
        myErr_Handler();
    } 
    else 
    {
        __NOP();
    }

    /* Configure ADC channel PA1, VMON */
    sConfigADC.Channel       = ADC_CHANNEL_1;
    sConfigADC.Rank          = 8;
    sConfigADC.SamplingTime  = ADC_SAMPLE_CYCLES;

    if (HAL_ADC_ConfigChannel(&hADC1, &sConfigADC) != HAL_OK)
    {
        myErr_Handler();
    } 
    else 
    {
        __NOP();
    }

    /* Configure ADC channel PA14, Temperature */
    sConfigADC.Channel       = ADC_CHANNEL_7;
    sConfigADC.Rank          = 9;
    sConfigADC.SamplingTime  = ADC_SAMPLE_CYCLES;

    if (HAL_ADC_ConfigChannel(&hADC1, &sConfigADC) != HAL_OK)
    {
        myErr_Handler();
    } 
    else 
    {
        __NOP();
    }

    /* Configure ADC channel PC4, IMON X */
    sConfigADC.Channel       = ADC_CHANNEL_14;
    sConfigADC.Rank          = 10;
    sConfigADC.SamplingTime  = ADC_SAMPLE_CYCLES;

    if (HAL_ADC_ConfigChannel(&hADC1, &sConfigADC) != HAL_OK)
    {
        myErr_Handler();
    } 
    else 
    {
        __NOP();
    }

    /* Configure ADC channel PC5, IMON Y */
    sConfigADC.Channel       = ADC_CHANNEL_15;
    sConfigADC.Rank          = 11;
    sConfigADC.SamplingTime  = ADC_SAMPLE_CYCLES;

    if (HAL_ADC_ConfigChannel(&hADC1, &sConfigADC) != HAL_OK)
    {
        myErr_Handler();
    } 
    else 
    {
        __NOP();
    }

    if (HAL_ADC_Start_DMA(&hADC1, (uint32_t*)ADC_ConvertedData, 11) != HAL_OK)
    {
        myErr_Handler();
    } 
    else 
    {
        __NOP();
    }
}
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
static void DAC_Config(void)
{
    DAC_ChannelConfTypeDef  sConfigDAC;

    hDAC1.Instance = DAC;

    if (HAL_DAC_Init(&hDAC1) != HAL_OK)
    {
        myErr_Handler();
    } 
    else 
    {
        __NOP();
    }

    sConfigDAC.DAC_OutputBuffer =  DAC_OUTPUTBUFFER_DISABLE;
    sConfigDAC.DAC_Trigger      =  DAC_TRIGGER_NONE;

    if (HAL_DAC_ConfigChannel(&hDAC1, &sConfigDAC, DAC_CHANNEL_1) != HAL_OK)
    {
        myErr_Handler();
    } 
    else 
    {
        __NOP();
    }

    if (HAL_DAC_Start(&hDAC1, DAC_CHANNEL_1) != HAL_OK)
    {
        myErr_Handler();
    } 
    else 
    {
        __NOP();
    }

    HAL_DAC_SetValue(&hDAC1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);
}
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
static void TIM3_Config(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_MasterConfigTypeDef sMasterConfig;

    hTIM3.Instance                      = TIM3;
    hTIM3.Init.CounterMode              = TIM_COUNTERMODE_UP;
    hTIM3.Init.ClockDivision            = TIM_CLOCKDIVISION_DIV1;
    hTIM3.Init.Prescaler                = 224;
    hTIM3.Init.Period                   = 15624;
    HAL_TIM_Base_Init(&hTIM3);

    sClockSourceConfig.ClockSource      = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&hTIM3, &sClockSourceConfig);

    sMasterConfig.MasterOutputTrigger   = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode       = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&hTIM3, &sMasterConfig);

    HAL_TIM_Base_Start_IT(&hTIM3);
}
/* ================================================================================================
 *  
 * ==============================================================================================*/
/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT
/**
 *  @brief  Reports the name of the source file and the source line number
 *          where the assert_param error has occurred.
 *  @param  file: pointer to the source file name
 *  @param  line: assert_param error line source number
 *  @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
        ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}
#endif
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
/**
 *  @}
 */ 
/**
 *  @}
 */
/* ================================================================================================
 *  FINIS
 * ==============================================================================================*/

