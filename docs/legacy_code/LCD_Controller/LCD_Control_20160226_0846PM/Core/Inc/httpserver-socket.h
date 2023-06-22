/**
  ******************************************************************************
  * @file    LwIP/LwIP_HTTP_Server_Socket_RTOS/Inc/httpserver-socket.h
  * @author  MCD Application Team
  * @version V1.2.2
  * @date    25-May-2015
  * @brief   header file for httpserver-socket.c
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
#ifndef __HTTPSERVER_SOCKET_H__
#define __HTTPSERVER_SOCKET_H__

#define MAX_HTTP_HTM_SIZE	(20*1024)
#define MAX_HTTP_INC_SIZE	(1*1024)
#define MAX_HTTP_XML_SIZE	(1*1024)

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void http_server_socket_init(void);
void DynWebPage(int conn);
void str_replace(char *target, const char *needle, const char *replacement);

#endif /* __HTTPSERVER_SOCKET_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
