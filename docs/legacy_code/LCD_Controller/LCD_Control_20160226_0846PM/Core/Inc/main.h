/**
  ******************************************************************************
  * @file    main.h 
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    13-March-2015
  * @brief   Header for main.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif
   
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"

/* EVAL includes component */
#include "stm324x9i_eval.h"
#include "stm324x9i_eval_sdram.h"
#include "stm324x9i_eval_ts.h"
#include "stm324x9i_eval_io.h"
#include "stm324x9i_eval_sd.h"
#include "stm324x9i_eval_audio.h"
#include "stm324x9i_eval_camera.h"
#include "stm324x9i_eval_nor.h"

/* FatFs includes component */
#include "ff_gen_drv.h"
#include "usbh_diskio.h"
#include "sd_diskio.h"
#include "gui.h"

/*LwIP Includes ------------------------------------------------------------------*/
#include "stm324x9i_eval_lcd.h"


/* Exported types ------------------------------------------------------------*/
typedef union
{
  uint32_t d32;
  struct
  {
    uint32_t enable_sprite     : 1;
    uint32_t enable_background : 1;
    uint32_t use_180Mhz        : 1;
    uint32_t disable_flex_skin : 1;  
  }b;
}
SystemSettingsTypeDef;

/* Exported variables --------------------------------------------------------*/
//extern GUI_CONST_STORAGE GUI_BITMAP bmSTLogo40x20;
extern GUI_CONST_STORAGE GUI_BITMAP bmatdi_bg_greywood;

/* Exported constants --------------------------------------------------------*/
#define NUM_DISK_UNITS       2
#define USB_DISK_UNIT        0
#define MSD_DISK_UNIT        1
#define AUDIO_DEFAULT_VOLUME 70

#define CALIBRATION_BKP0                    RTC_BKP_DR0
#define CALIBRATION_BKP1                    RTC_BKP_DR1
#define CALIBRATION_GENERAL_SETTINGS_BKP    RTC_BKP_DR2
#define CALIBRATION_IMAGE_SETTINGS_BKP      RTC_BKP_DR3
#define CALIBRATION_AUDIOPLAYER_SETTING_BKP RTC_BKP_DR4
#define CALIBRATION_CAMERA_SETTING_BKP      RTC_BKP_DR5
#define CALIBRATION_BENCH_SETTING_BKP       RTC_BKP_DR6
#define CALIBRATION_VIDEOPLAYER_SETTING_BKP RTC_BKP_DR7

#define CAMERA_SAVE_PATH                    BKPSRAM_BASE + 0x000
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 



/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define USE_LCD        /* enable LCD  */  
#define USE_DHCP       /* enable DHCP, if disabled static address is used*/
 
/*Static IP ADDRESS*/
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   1 //2
#define IP_ADDR3   77
   
/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/*Gateway Address*/
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   1 //2
#define GW_ADDR3   1




#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
