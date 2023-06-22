#include "stm32f4xx_hal.h"
#include "Board_ADC.h"

#define ADC_RESOLUTION        12        /* Number of A/D converter bits       */

// static ADC_HandleTypeDef hadc3;
static ADC_HandleTypeDef hadc1;
static ADC_HandleTypeDef hadc2;

/**
  \fn          int32_t ADC_Initialize (void)
  \brief       Initialize Analog-to-Digital Converter
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t ADC_NMM_Initialize (void) 
{
  ADC_ChannelConfTypeDef sConfig;
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
	__GPIOA_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
	/* __GPIOF_CLK_ENABLE(); */
	
	
	/* ADC1 GPIO Configuration: PC{0, 1, 2, 3, 4, 5} -> ADC1_IN{10, 11, 12, 13, 14, 15} */
	GPIO_InitStruct.Pin		= GPIO_PIN_0;										// Reference Cell #1
	GPIO_InitStruct.Pin		|= GPIO_PIN_1;									// Reference Cell #2
	GPIO_InitStruct.Pin		|= GPIO_PIN_2;									// Reference Cell #3
	GPIO_InitStruct.Pin		|= GPIO_PIN_3;									// Reference Cell #4
	GPIO_InitStruct.Pin		|= GPIO_PIN_4;									// 
	GPIO_InitStruct.Pin		|= GPIO_PIN_5;
	GPIO_InitStruct.Mode	= GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull	= GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	/* ADC2 GPIO Configuration: PA{1, 2, 3, 6, 7} -> ADC2_IN{1, 2, 3, 6, 7} */
	GPIO_InitStruct.Pin		= GPIO_PIN_1;
	GPIO_InitStruct.Pin		|= GPIO_PIN_2;
	GPIO_InitStruct.Pin		|= GPIO_PIN_3;
	GPIO_InitStruct.Pin		|= GPIO_PIN_6;
	GPIO_InitStruct.Pin		|= GPIO_PIN_7;
	GPIO_InitStruct.Mode	= GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull	= GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
  /* ADC3 GPIO Configuration: PF9 -> ADC3_IN7 */
  /// GPIO_InitStruct.Pin		= GPIO_PIN_9;
  /// GPIO_InitStruct.Mode 	= GPIO_MODE_ANALOG;
  /// GPIO_InitStruct.Pull 	= GPIO_NOPULL;
  /// HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /* Peripheral clock enable */
	/// __ADC3_CLK_ENABLE();
	__ADC1_CLK_ENABLE();
	__ADC2_CLK_ENABLE();

  /* Configure the global features of the ADC
    (Clock, Resolution, Data Alignment and number of conversion) */
	
	hadc1.Instance 										= ADC1;
  hadc1.Init.ClockPrescaler 				= ADC_CLOCKPRESCALER_PCLK_DIV2;
  hadc1.Init.Resolution 						= ADC_RESOLUTION12b;
  hadc1.Init.ScanConvMode 					= DISABLE;
  hadc1.Init.ContinuousConvMode 		= DISABLE;												
  hadc1.Init.DiscontinuousConvMode 	= DISABLE;
  hadc1.Init.NbrOfDiscConversion 		= 1;
  hadc1.Init.ExternalTrigConvEdge 	= ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DataAlign 							= ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion 				= 1;
  hadc1.Init.DMAContinuousRequests 	= DISABLE;
  hadc1.Init.EOCSelection 					= EOC_SINGLE_CONV;
  HAL_ADC_Init(&hadc1);
	
	hadc2.Instance 										= ADC2;
  hadc2.Init.ClockPrescaler 				= ADC_CLOCKPRESCALER_PCLK_DIV2;
  hadc2.Init.Resolution 						= ADC_RESOLUTION12b;
  hadc2.Init.ScanConvMode 					= DISABLE;
  hadc2.Init.ContinuousConvMode 		= DISABLE;
  hadc2.Init.DiscontinuousConvMode 	= DISABLE;
  hadc2.Init.NbrOfDiscConversion 		= 1;
  hadc2.Init.ExternalTrigConvEdge 	= ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.DataAlign 							= ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion 				= 1;
  hadc2.Init.DMAContinuousRequests 	= DISABLE;
  hadc2.Init.EOCSelection 					= EOC_SINGLE_CONV;
  HAL_ADC_Init(&hadc2);
	
	/**
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
  hadc3.Init.Resolution = ADC_RESOLUTION12b;
  hadc3.Init.ScanConvMode = DISABLE;
  hadc3.Init.ContinuousConvMode = DISABLE;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.NbrOfDiscConversion = 1;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc3.Init.NbrOfConversion = 1;
  hadc3.Init.DMAContinuousRequests = DISABLE;
  hadc3.Init.EOCSelection = EOC_SINGLE_CONV;
  HAL_ADC_Init(&hadc3);
	**/
	
  /* Configure the selected ADC channel */
  /// sConfig.Channel = ADC_CHANNEL_7;
  /// sConfig.Rank = 1;
  /// sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  /// HAL_ADC_ConfigChannel(&hadc3, &sConfig);
	
	/* Configure the selected ADC1 channels */
	sConfig.Channel				= ADC_CHANNEL_10;
	sConfig.Rank					= 1;
	sConfig.SamplingTime	= ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
	
	sConfig.Channel				= ADC_CHANNEL_11;
	sConfig.Rank					= 1;
	sConfig.SamplingTime	= ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
	
	sConfig.Channel				= ADC_CHANNEL_12;
	sConfig.Rank					= 1;
	sConfig.SamplingTime	= ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
	
	sConfig.Channel				= ADC_CHANNEL_13;
	sConfig.Rank					= 1;
	sConfig.SamplingTime	= ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
	
	sConfig.Channel				= ADC_CHANNEL_14;
	sConfig.Rank					= 1;
	sConfig.SamplingTime	= ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
	
	sConfig.Channel				= ADC_CHANNEL_15;
	sConfig.Rank					= 1;
	sConfig.SamplingTime	= ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
	
	/* Configure the selected ADC1 channels */
	sConfig.Channel				= ADC_CHANNEL_1;
	sConfig.Rank					= 1;
	sConfig.SamplingTime	= ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&hadc2, &sConfig);
	
	sConfig.Channel				= ADC_CHANNEL_2;
	sConfig.Rank					= 1;
	sConfig.SamplingTime	= ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&hadc2, &sConfig);
	
	sConfig.Channel				= ADC_CHANNEL_3;
	sConfig.Rank					= 1;
	sConfig.SamplingTime	= ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&hadc2, &sConfig);
	
	sConfig.Channel				= ADC_CHANNEL_6;
	sConfig.Rank					= 1;
	sConfig.SamplingTime	= ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&hadc2, &sConfig);

	sConfig.Channel				= ADC_CHANNEL_7;
	sConfig.Rank					= 1;
	sConfig.SamplingTime	= ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&hadc2, &sConfig);
  
	return 0;
}

/**
  \fn          int32_t ADC_Uninitialize (void)
  \brief       De-initialize Analog-to-Digital Converter
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t ADC_NMM_Uninitialize (void) 
{

  /* Peripheral clock disable */
  /// __ADC3_CLK_DISABLE();
	__ADC1_CLK_DISABLE();
	__ADC2_CLK_DISABLE();
  
  /* ADC3 GPIO Configuration: PF9 -> ADC3_IN7 */
  /// HAL_GPIO_DeInit(GPIOF, GPIO_PIN_9);
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0);
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1);
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_2);
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_3);
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_4);
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_5);
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3);
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6);
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_7);

  return 0;
}

/**
  \fn          int32_t ADC_StartConversion (void)
  \brief       Start conversion
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t ADC_NMM_StartConversion (void) 
{
  /// __HAL_ADC_CLEAR_FLAG(&hadc3, ADC_FLAG_EOC);
  /// HAL_ADC_Start(&hadc3);
	__HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_EOC);
	__HAL_ADC_CLEAR_FLAG(&hadc2, ADC_FLAG_EOC);
	HAL_ADC_Start(&hadc1);
	HAL_ADC_Start(&hadc2);
	
  return 0;
}

/**
  \fn          int32_t ADC_ConversionDone (void)
  \brief       Check if conversion finished
  \returns
   - \b  0: conversion finished
   - \b -1: conversion in progress
*/
/**
int32_t ADC_ConversionDone (void) {
  /// HAL_StatusTypeDef status;
	
  /// status = HAL_ADC_PollForConversion(&hadc3, 0);
  /// return ((status == HAL_OK) ? 0 : -1);
}
*/
int32_t ADC1_NMM_ConversionDone (void)
{
	HAL_StatusTypeDef	status;
	
	status = HAL_ADC_PollForConversion(&hadc1, 0);
	
	return ((status == HAL_OK) ? 0 : -1);
}

int32_t ADC2_NMM_Conversion (void)
{
	HAL_StatusTypeDef status;
	
	status = HAL_ADC_PollForConversion(&hadc2, 0);
	
	return ((status == HAL_OK) ? 0 : -1);
}

/**
  \fn          int32_t ADC_GetValue (void)
  \brief       Get converted value
  \returns
   - <b> >=0</b>: converted value
   - \b -1: conversion in progress or failed
*/
/**
int32_t ADC_GetValue (void) {
  HAL_StatusTypeDef status;

  status = HAL_ADC_PollForConversion(&hadc3, 0);
  if (status != HAL_OK) return -1;

  return HAL_ADC_GetValue(&hadc3);
}
*/
int32_t ADC1_NMM_GetValue (void)
{
	HAL_StatusTypeDef status;
	
	status = HAL_ADC_PollForConversion(&hadc1, 0);
	if(status != HAL_OK)
		return -1;
	
	return HAL_ADC_GetValue(&hadc1);
}

int32_t ADC2_NMM_GetValue (void)
{
	HAL_StatusTypeDef status;
	
	status = HAL_ADC_PollForConversion(&hadc2, 0);
	if(status != HAL_OK)
		return -1;
	
	return HAL_ADC_GetValue(&hadc2);
}
/**
  \fn          uint32_t ADC_GetResolution (void)
  \brief       Get resolution of Analog-to-Digital Converter
  \returns     Resolution (in bits)
*/
uint32_t ADC_GetResolution (void) 
{
  return ADC_RESOLUTION;
}
