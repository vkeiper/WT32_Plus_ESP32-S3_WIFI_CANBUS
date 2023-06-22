/**
  ******************************************************************************
  * @file    benchmark_res.c
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    13-March-2015   
  * @brief   CPU benchmark resources
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include "GUI.h"

/** @addtogroup BENCHMARK_MODULE
  * @{
  */

/** @defgroup BENCHMARK_RES
 * @brief benchmark resources
 * @{
 */

/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifndef GUI_CONST_STORAGE
  #define GUI_CONST_STORAGE const
#endif

// vk, put this in any source file that you need to access this bmp
//extern GUI_CONST_STORAGE GUI_BITMAP bmdefender_shield_trans;

//extern const unsigned long _acdefender_shield_trans[2744];
//GUI_CONST_STORAGE GUI_BITMAP bmdefender_shield_trans = {
//  49, // xSize
//  56, // ySize
//  196, // BytesPerLine
//  32, // BitsPerPixel
//  (unsigned char *)_acdefender_shield_trans,  // Pointer to picture data
//  NULL,  // Pointer to palette
//  GUI_DRAW_BMP8888
//};



//extern const unsigned long _acAtdi_logo_small_trans[11186];
//extern GUI_CONST_STORAGE GUI_BITMAP bmAtdi_logo_small_trans = {
//  238, // xSize
//  47, // ySize
//  952, // BytesPerLine
//  32, // BitsPerPixel
//  (unsigned char *)_acAtdi_logo_small_trans,  // Pointer to picture data
//  NULL,  // Pointer to palette
//  GUI_DRAW_BMP8888
//};


/* Private functions ---------------------------------------------------------*/

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
