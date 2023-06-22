/**
  ******************************************************************************
  * @file    LwIP/LwIP_HTTP_Server_Socket_RTOS/Src/httpserver-socket.c
  * @author  MCD Application Team
  * @version V1.2.2
  * @date    25-May-2015  
  * @brief   Basic http server implementation using LwIP socket API   
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
  
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "fs.h"
#include "fsdata.h"
#include "string.h"
#include "httpserver-socket.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "stdlib.h"
#include "iccpfuncs.h"
#include "iccp_canfuncs.h"

/* External variables  */
char dbgstrweb[32];

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define WEBSERVER_THREAD_PRIO    ( tskIDLE_PRIORITY + 4 )

#define REFELEC_ACTIVE	GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_active

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u32_t nPageHits = 0;
portCHAR PAGE_BODY[512];
char http_file[MAX_HTTP_HTM_SIZE] = { 0 };
char xmlfile[MAX_HTTP_XML_SIZE] = { 0 };
char inc_file[MAX_HTTP_INC_SIZE] = { 0 };
char buffer[MAX_HTTP_HTM_SIZE] = { 0 };
char *file_temp2 = NULL;

/* Format of dynamic web page: the page header */
static const unsigned char PAGE_START[] = {
0x3c,0x21,0x44,0x4f,0x43,0x54,0x59,0x50,0x45,0x20,0x68,0x74,0x6d,0x6c,0x20,0x50,
0x55,0x42,0x4c,0x49,0x43,0x20,0x22,0x2d,0x2f,0x2f,0x57,0x33,0x43,0x2f,0x2f,0x44,
0x54,0x44,0x20,0x48,0x54,0x4d,0x4c,0x20,0x34,0x2e,0x30,0x31,0x2f,0x2f,0x45,0x4e,
0x22,0x20,0x22,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x77,0x77,0x2e,0x77,0x33,
0x2e,0x6f,0x72,0x67,0x2f,0x54,0x52,0x2f,0x68,0x74,0x6d,0x6c,0x34,0x2f,0x73,0x74,
0x72,0x69,0x63,0x74,0x2e,0x64,0x74,0x64,0x22,0x3e,0x0d,0x0a,0x3c,0x68,0x74,0x6d,
0x6c,0x3e,0x0d,0x0a,0x3c,0x68,0x65,0x61,0x64,0x3e,0x0d,0x0a,0x20,0x20,0x3c,0x74,
0x69,0x74,0x6c,0x65,0x3e,0x53,0x54,0x4d,0x33,0x32,0x46,0x34,0x78,0x78,0x54,0x41,
0x53,0x4b,0x53,0x3c,0x2f,0x74,0x69,0x74,0x6c,0x65,0x3e,0x0d,0x0a,0x20,0x20,0x3c,
0x6d,0x65,0x74,0x61,0x20,0x68,0x74,0x74,0x70,0x2d,0x65,0x71,0x75,0x69,0x76,0x3d,
0x22,0x43,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x2d,0x54,0x79,0x70,0x65,0x22,0x0d,0x0a,
0x20,0x63,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x3d,0x22,0x74,0x65,0x78,0x74,0x2f,0x68,
0x74,0x6d,0x6c,0x3b,0x20,0x63,0x68,0x61,0x72,0x73,0x65,0x74,0x3d,0x77,0x69,0x6e,
0x64,0x6f,0x77,0x73,0x2d,0x31,0x32,0x35,0x32,0x22,0x3e,0x0d,0x0a,0x20,0x20,0x3c,
0x6d,0x65,0x74,0x61,0x20,0x68,0x74,0x74,0x70,0x2d,0x65,0x71,0x75,0x69,0x76,0x3d,
0x22,0x72,0x65,0x66,0x72,0x65,0x73,0x68,0x22,0x20,0x63,0x6f,0x6e,0x74,0x65,0x6e,
0x74,0x3d,0x22,0x31,0x22,0x3e,0x0d,0x0a,0x20,0x20,0x3c,0x6d,0x65,0x74,0x61,0x20,
0x63,0x6f,0x6e,0x74,0x65,0x6e,0x74,0x3d,0x22,0x4d,0x53,0x48,0x54,0x4d,0x4c,0x20,
0x36,0x2e,0x30,0x30,0x2e,0x32,0x38,0x30,0x30,0x2e,0x31,0x35,0x36,0x31,0x22,0x20,
0x6e,0x61,0x6d,0x65,0x3d,0x22,0x47,0x45,0x4e,0x45,0x52,0x41,0x54,0x4f,0x52,0x22,
0x3e,0x0d,0x0a,0x20,0x20,0x3c,0x73,0x74,0x79,0x6c,0x65,0x20,0x3d,0x22,0x66,0x6f,
0x6e,0x74,0x2d,0x77,0x65,0x69,0x67,0x68,0x74,0x3a,0x20,0x6e,0x6f,0x72,0x6d,0x61,
0x6c,0x3b,0x20,0x66,0x6f,0x6e,0x74,0x2d,0x66,0x61,0x6d,0x69,0x6c,0x79,0x3a,0x20,
0x56,0x65,0x72,0x64,0x61,0x6e,0x61,0x3b,0x22,0x3e,0x3c,0x2f,0x73,0x74,0x79,0x6c,
0x65,0x3e,0x0d,0x0a,0x3c,0x2f,0x68,0x65,0x61,0x64,0x3e,0x0d,0x0a,0x3c,0x62,0x6f,
0x64,0x79,0x3e,0x0d,0x0a,0x3c,0x68,0x34,0x3e,0x3c,0x73,0x6d,0x61,0x6c,0x6c,0x20,
0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,0x6e,0x74,0x2d,0x66,0x61,0x6d,0x69,
0x6c,0x79,0x3a,0x20,0x56,0x65,0x72,0x64,0x61,0x6e,0x61,0x3b,0x22,0x3e,0x3c,0x73,
0x6d,0x61,0x6c,0x6c,0x3e,0x3c,0x62,0x69,0x67,0x3e,0x3c,0x62,0x69,0x67,0x3e,0x3c,
0x62,0x69,0x67,0x0d,0x0a,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,0x6e,
0x74,0x2d,0x77,0x65,0x69,0x67,0x68,0x74,0x3a,0x20,0x62,0x6f,0x6c,0x64,0x3b,0x22,
0x3e,0x3c,0x62,0x69,0x67,0x3e,0x3c,0x73,0x74,0x72,0x6f,0x6e,0x67,0x3e,0x3c,0x65,
0x6d,0x3e,0x3c,0x73,0x70,0x61,0x6e,0x0d,0x0a,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,
0x22,0x66,0x6f,0x6e,0x74,0x2d,0x73,0x74,0x79,0x6c,0x65,0x3a,0x20,0x69,0x74,0x61,
0x6c,0x69,0x63,0x3b,0x22,0x3e,0x53,0x54,0x4d,0x33,0x32,0x46,0x34,0x78,0x78,0x20,
0x4c,0x69,0x73,0x74,0x20,0x6f,0x66,0x20,0x74,0x61,0x73,0x6b,0x73,0x20,0x61,0x6e,
0x64,0x0d,0x0a,0x74,0x68,0x65,0x69,0x72,0x20,0x73,0x74,0x61,0x74,0x75,0x73,0x3c,
0x2f,0x73,0x70,0x61,0x6e,0x3e,0x3c,0x2f,0x65,0x6d,0x3e,0x3c,0x2f,0x73,0x74,0x72,
0x6f,0x6e,0x67,0x3e,0x3c,0x2f,0x62,0x69,0x67,0x3e,0x3c,0x2f,0x62,0x69,0x67,0x3e,
0x3c,0x2f,0x62,0x69,0x67,0x3e,0x3c,0x2f,0x62,0x69,0x67,0x3e,0x3c,0x2f,0x73,0x6d,
0x61,0x6c,0x6c,0x3e,0x3c,0x2f,0x73,0x6d,0x61,0x6c,0x6c,0x3e,0x3c,0x2f,0x68,0x34,
0x3e,0x0d,0x0a,0x3c,0x68,0x72,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x77,0x69,
0x64,0x74,0x68,0x3a,0x20,0x31,0x30,0x30,0x25,0x3b,0x20,0x68,0x65,0x69,0x67,0x68,
0x74,0x3a,0x20,0x32,0x70,0x78,0x3b,0x22,0x3e,0x3c,0x73,0x70,0x61,0x6e,0x0d,0x0a,
0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,0x6e,0x74,0x2d,0x77,0x65,0x69,
0x67,0x68,0x74,0x3a,0x20,0x62,0x6f,0x6c,0x64,0x3b,0x22,0x3e,0x0d,0x0a,0x3c,0x2f,
0x73,0x70,0x61,0x6e,0x3e,0x3c,0x73,0x70,0x61,0x6e,0x20,0x73,0x74,0x79,0x6c,0x65,
0x3d,0x22,0x66,0x6f,0x6e,0x74,0x2d,0x77,0x65,0x69,0x67,0x68,0x74,0x3a,0x20,0x62,
0x6f,0x6c,0x64,0x3b,0x22,0x3e,0x0d,0x0a,0x3c,0x74,0x61,0x62,0x6c,0x65,0x20,0x73,
0x74,0x79,0x6c,0x65,0x3d,0x22,0x77,0x69,0x64,0x74,0x68,0x3a,0x20,0x39,0x36,0x31,
0x70,0x78,0x3b,0x20,0x68,0x65,0x69,0x67,0x68,0x74,0x3a,0x20,0x33,0x30,0x70,0x78,
0x3b,0x22,0x20,0x62,0x6f,0x72,0x64,0x65,0x72,0x3d,0x22,0x31,0x22,0x0d,0x0a,0x20,
0x63,0x65,0x6c,0x6c,0x70,0x61,0x64,0x64,0x69,0x6e,0x67,0x3d,0x22,0x32,0x22,0x20,
0x63,0x65,0x6c,0x6c,0x73,0x70,0x61,0x63,0x69,0x6e,0x67,0x3d,0x22,0x32,0x22,0x3e,
0x0d,0x0a,0x20,0x20,0x3c,0x74,0x62,0x6f,0x64,0x79,0x3e,0x0d,0x0a,0x20,0x20,0x20,
0x20,0x3c,0x74,0x72,0x3e,0x0d,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x3c,0x74,0x64,
0x0d,0x0a,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,0x6e,0x74,0x2d,0x66,
0x61,0x6d,0x69,0x6c,0x79,0x3a,0x20,0x56,0x65,0x72,0x64,0x61,0x6e,0x61,0x3b,0x20,
0x66,0x6f,0x6e,0x74,0x2d,0x77,0x65,0x69,0x67,0x68,0x74,0x3a,0x20,0x62,0x6f,0x6c,
0x64,0x3b,0x20,0x66,0x6f,0x6e,0x74,0x2d,0x73,0x74,0x79,0x6c,0x65,0x3a,0x20,0x69,
0x74,0x61,0x6c,0x69,0x63,0x3b,0x20,0x62,0x61,0x63,0x6b,0x67,0x72,0x6f,0x75,0x6e,
0x64,0x2d,0x63,0x6f,0x6c,0x6f,0x72,0x3a,0x20,0x72,0x67,0x62,0x28,0x35,0x31,0x2c,
0x20,0x35,0x31,0x2c,0x20,0x32,0x35,0x35,0x29,0x3b,0x20,0x74,0x65,0x78,0x74,0x2d,
0x61,0x6c,0x69,0x67,0x6e,0x3a,0x20,0x63,0x65,0x6e,0x74,0x65,0x72,0x3b,0x22,0x3e,
0x3c,0x73,0x6d,0x61,0x6c,0x6c,0x3e,0x3c,0x61,0x0d,0x0a,0x20,0x68,0x72,0x65,0x66,
0x3d,0x22,0x2f,0x53,0x54,0x4d,0x33,0x32,0x46,0x34,0x78,0x78,0x2e,0x68,0x74,0x6d,
0x6c,0x22,0x3e,0x3c,0x73,0x70,0x61,0x6e,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,
0x63,0x6f,0x6c,0x6f,0x72,0x3a,0x20,0x77,0x68,0x69,0x74,0x65,0x3b,0x22,0x3e,0x48,
0x6f,0x6d,0x65,0x0d,0x0a,0x70,0x61,0x67,0x65,0x3c,0x2f,0x73,0x70,0x61,0x6e,0x3e,
0x3c,0x2f,0x61,0x3e,0x3c,0x2f,0x73,0x6d,0x61,0x6c,0x6c,0x3e,0x3c,0x2f,0x74,0x64,
0x3e,0x0d,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x3c,0x74,0x64,0x0d,0x0a,0x20,0x73,
0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,0x6e,0x74,0x2d,0x66,0x61,0x6d,0x69,0x6c,
0x79,0x3a,0x20,0x56,0x65,0x72,0x64,0x61,0x6e,0x61,0x3b,0x20,0x66,0x6f,0x6e,0x74,
0x2d,0x77,0x65,0x69,0x67,0x68,0x74,0x3a,0x20,0x62,0x6f,0x6c,0x64,0x3b,0x20,0x66,
0x6f,0x6e,0x74,0x2d,0x73,0x74,0x79,0x6c,0x65,0x3a,0x20,0x69,0x74,0x61,0x6c,0x69,
0x63,0x3b,0x20,0x62,0x61,0x63,0x6b,0x67,0x72,0x6f,0x75,0x6e,0x64,0x2d,0x63,0x6f,
0x6c,0x6f,0x72,0x3a,0x20,0x72,0x67,0x62,0x28,0x35,0x31,0x2c,0x20,0x35,0x31,0x2c,
0x20,0x32,0x35,0x35,0x29,0x3b,0x20,0x74,0x65,0x78,0x74,0x2d,0x61,0x6c,0x69,0x67,
0x6e,0x3a,0x20,0x63,0x65,0x6e,0x74,0x65,0x72,0x3b,0x22,0x3e,0x3c,0x61,0x0d,0x0a,
0x20,0x68,0x72,0x65,0x66,0x3d,0x22,0x53,0x54,0x4d,0x33,0x32,0x46,0x34,0x78,0x78,
0x41,0x44,0x43,0x2e,0x68,0x74,0x6d,0x6c,0x22,0x3e,0x3c,0x73,0x70,0x61,0x6e,0x20,
0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,0x6e,0x74,0x2d,0x77,0x65,0x69,0x67,
0x68,0x74,0x3a,0x20,0x62,0x6f,0x6c,0x64,0x3b,0x22,0x3e,0x3c,0x2f,0x73,0x70,0x61,
0x6e,0x3e,0x3c,0x2f,0x61,0x3e,0x3c,0x73,0x6d,0x61,0x6c,0x6c,0x3e,0x3c,0x61,0x0d,
0x0a,0x20,0x68,0x72,0x65,0x66,0x3d,0x22,0x2f,0x53,0x54,0x4d,0x33,0x32,0x46,0x34,
0x78,0x78,0x54,0x41,0x53,0x4b,0x53,0x2e,0x68,0x74,0x6d,0x6c,0x22,0x3e,0x3c,0x73,
0x70,0x61,0x6e,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x63,0x6f,0x6c,0x6f,0x72,
0x3a,0x20,0x77,0x68,0x69,0x74,0x65,0x3b,0x22,0x3e,0x4c,0x69,0x73,0x74,0x0d,0x0a,
0x6f,0x66,0x20,0x74,0x61,0x73,0x6b,0x73,0x3c,0x2f,0x73,0x70,0x61,0x6e,0x3e,0x3c,
0x2f,0x61,0x3e,0x3c,0x2f,0x73,0x6d,0x61,0x6c,0x6c,0x3e,0x3c,0x2f,0x74,0x64,0x3e,
0x0d,0x0a,0x20,0x20,0x20,0x20,0x3c,0x2f,0x74,0x72,0x3e,0x0d,0x0a,0x20,0x20,0x3c,
0x2f,0x74,0x62,0x6f,0x64,0x79,0x3e,0x0d,0x0a,0x3c,0x2f,0x74,0x61,0x62,0x6c,0x65,
0x3e,0x0d,0x0a,0x3c,0x62,0x72,0x3e,0x0d,0x0a,0x3c,0x2f,0x73,0x70,0x61,0x6e,0x3e,
0x3c,0x73,0x70,0x61,0x6e,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,0x6e,
0x74,0x2d,0x77,0x65,0x69,0x67,0x68,0x74,0x3a,0x20,0x62,0x6f,0x6c,0x64,0x3b,0x22,
0x3e,0x3c,0x2f,0x73,0x70,0x61,0x6e,0x3e,0x3c,0x73,0x6d,0x61,0x6c,0x6c,0x3e,0x3c,
0x73,0x70,0x61,0x6e,0x0d,0x0a,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x66,0x6f,
0x6e,0x74,0x2d,0x66,0x61,0x6d,0x69,0x6c,0x79,0x3a,0x20,0x56,0x65,0x72,0x64,0x61,
0x6e,0x61,0x3b,0x22,0x3e,0x4e,0x75,0x6d,0x62,0x65,0x72,0x20,0x6f,0x66,0x20,0x70,
0x61,0x67,0x65,0x20,0x68,0x69,0x74,0x73,0x3a,0x0d,0x0a,0x00};

/* Private function prototypes -----------------------------------------------*/
char *fs_open2(const char *name);
void get_http_file(const char *httpFileName);
void trim_html_file(char *html_file);
void insert_inc_file(const char *incKey, const char *incFileName);
char *repl_str(const char *str, const char *old, const char *new);
//void repl_str(char *str, const char *old, const char *new);
err_t http_getlinevalue( char* lpszValueName, char* lpszLine, char* lpszBuffer, u32_t dwBufferSize);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief serve tcp connection  
  * @param conn: connection socket 
  * @retval None
  */
void http_server_serve(int conn) 
{
  int buflen = 1500;
  int ret;
  struct fs_file * file;
  unsigned char recv_buffer[1500];

	memset( recv_buffer, 0, buflen);//EAJ 2/19/2016 - clear out the crap first!!!
	
  /* Read in the request */
  ret = read(conn, recv_buffer, buflen); 
  if(ret < 0) return;
	
  /* Check if request to get ST.gif */
  if (strncmp((char *)recv_buffer,"GET /STM32F4xx_files/ST.gif",27)==0)
  {
    file = fs_open("/STM32F4xx_files/ST.gif"); 
    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
    if(file) fs_close(file);
  }
  /* Check if request to get stm32.jpeg */
  else if (strncmp((char *)recv_buffer,"GET /STM32F4xx_files/stm32.jpg",30)==0)
  {
    file = fs_open("/STM32F4xx_files/stm32.jpg"); 
    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
    if(file) fs_close(file);
  }
  /* Check if request to get ST logo.jpeg */
  else if (strncmp((char *)recv_buffer,"GET /STM32F4xx_files/logo.jpg", 29) == 0)
  {
    file = fs_open("/STM32F4xx_files/logo.jpg"); 
    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
    if(file) fs_close(file);
  }
  /* Check if request to get tdi.gif */
  else if (strncmp((char *)recv_buffer,"GET /tdi.gif", 12) == 0)
  {
    file = fs_open("/tdi.gif"); 
    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
    if(file) fs_close(file);
  }
	/* Check if request to get astrodynetdi_logo.gif */
  else if (strncmp((char *)recv_buffer,"GET /STM32F4xx_files/astrodynetdi_logo.gif", 42) == 0)
  {
    file = fs_open("/STM32F4xx_files/astrodynetdi_logo.gif"); 
    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
    if(file) fs_close(file);
  }
  else if (strncmp((char *)recv_buffer,"GET /logo_astrodyne.png", 23) == 0)
  {
    file = fs_open("/logo_astrodyne.png"); 
    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
    if(file) fs_close(file);
  }
  else if (strncmp((char *)recv_buffer,"GET /logo_astrodyne.jpg", 23) == 0)
  {
    file = fs_open("/logo_astrodyne.jpg"); 
    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
    if(file) fs_close(file);
  }
  else if (strncmp((char *)recv_buffer,"GET /logo_astrodyne.gif", 23) == 0)
  {
    file = fs_open("/logo_astrodyne.gif"); 
    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
    if(file) fs_close(file);
  }
  else if (strncmp((char *)recv_buffer,"GET /chk0.bmp", 13) == 0)
  {
    file = fs_open("/chk0.bmp"); 
    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
    if(file) fs_close(file);
  }
  else if (strncmp((char *)recv_buffer,"GET /chk1.bmp", 13) == 0)
  {
    file = fs_open("/chk1.bmp"); 
    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
    if(file) fs_close(file);
  }
  else if (strncmp((char *)recv_buffer,"GET /liquabladedynavars.js", 26) == 0)
  {
    file = fs_open("/liquabladedynavars.js"); 
    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
    if(file) fs_close(file);
  }
  else if (strncmp((char *)recv_buffer,"GET /iccpdynavars.js", 20) == 0)
  {
    file = fs_open("/iccpdynavars.js"); 
    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
    if(file) fs_close(file);
  }
  else if (strncmp((char *)recv_buffer,"GET /status.xml", 15) == 0)
  {
		//char tempfile[20000] = { 0 };
		//char search_str[16] = { 0 };
		//char replacement_str[16] = { 0 };
		static int analog=0;
		static int led=0;
		static int zonestat=0;
		int psu_mode = 0;
		char str_psu_mode[16];
		char str_e_desc[32];
		char str_e_en[16];
		char read_e_en[4][16];
		uint8_t  refelec_active;
		
		uint16_t i_led[7];
		uint16_t almreg;
		
		memcpy(xmlfile, "\0", sizeof(xmlfile));
		
    //file = fs_open("/status.xml"); //Read requested file

		//strncpy(tempfile,file->data,(size_t)file->len); //Copy file to local file buffer
		
		sprintf(xmlfile,"%s%s\r\n",xmlfile,"<response>");

		int i;
		uint8_t  node_can_onoff=0;
		
//		for(i = 0; i < 5; i++)
//		{	
//			sprintf(xmlfile,"%s<zonestat%d>%d</zonestat%d>\r\n",xmlfile,i,zonestat,i);
//			zonestat++;
//			if(zonestat>3){zonestat=0;}
//		}

//		for(i = 0; i < 6; i++)
//		{
////			sprintf(search_str,"~led(%d)~",i);
////			sprintf(replacement_str,"%d",led);
////			str_replace(tempfile, search_str, replacement_str);
//			
//			sprintf(xmlfile,"%s<led%d>%d</led%d>\r\n",xmlfile,i,led,i);
//			led++;
//			if(led>3){led=0;}
//		}
		
//				case 0:
//					guiobj_id = ID_IMAGE_1; // ACOK LED
//				  guiobj_text = ID_TEXT_AC;
//					shiftqty = 2;
//				break;
//				
//				case 1:
//					guiobj_id = ID_IMAGE_2; // DCOK LED
//					guiobj_text = ID_TEXT_DC;
//					shiftqty = 1;
//				break;
//				
//				case 2:
//					guiobj_id = ID_IMAGE_3; // RECT SUM FLT LED
//					guiobj_text = ID_TEXT_RCT;
//					shiftqty = 3;
//				break;

//				case 3:
//					guiobj_id = ID_IMAGE_4; // SURGE FLT LED
//					guiobj_text = ID_TEXT_SRG;
//					shiftqty = 8;
//				break;
//				
//				case 4:
//					guiobj_id = ID_IMAGE_5; // REF ELECTRODE FLT LED
//					guiobj_text = ID_TEXT_REF;
//					shiftqty = 6;
//				break;
//				
//				case 5:
//					guiobj_id = ID_IMAGE_6; // COM FLT FLT LED
//					guiobj_text = ID_TEXT_COM;
//					shiftqty = 7;
//				break;
		
		almreg=GuiState.SysDataPSU[GuiState.psu_viewing].AlmReg;
		
		i_led[1] = (almreg>>2 & 0x0001)+1;
		i_led[2] = 0; //(almreg>>1 & 0x0001)+1;
		i_led[3] = (almreg>>3 & 0x0001)+1;
		i_led[4] = 0; //(almreg>>8 & 0x0001)+1;
		i_led[5] = (almreg>>6 & 0x0001)+1;
		i_led[6] = (almreg>>7 & 0x0001)+1;
		
//		for(i = 0; i < 4; i++)
//		{
////			sprintf(search_str,"~analog(%d)~",i); //Set search string
////			sprintf(replacement_str,"%d",analog+i); //Set replacement string
////			str_replace(tempfile, search_str, replacement_str);//Replace search string with replacement string
//			
//			switch (i)
//			{
//				case 0: //Volts
//					{
//						//sprintf(dbgstr,"%2.1f",(float)GuiState.SysDataPSU[GuiState.psu_viewing].Vrdbk/10);
//						sprintf(xmlfile,"%s<analog%d>%2.1f</analog%d>\r\n",xmlfile,i,(float)GuiState.SysDataPSU[GuiState.psu_viewing].Vrdbk/10,i);
//						break;
//					}
//				case 1: //Amps
//					{
//						//sprintf(dbgstr,"%2.1f",(float)GuiState.SysDataPSU[GuiState.psu_viewing].Irdbk/10);
//						sprintf(xmlfile,"%s<analog%d>%2.1f</analog%d>\r\n",xmlfile,i,(float)GuiState.SysDataPSU[GuiState.psu_viewing].Irdbk/10,i);
//						break;
//					}
//				default:
//					{
//						sprintf(xmlfile,"%s<analog%d>%d</analog%d>\r\n",xmlfile,i,analog,i);
//						break;
//					}
//			}
//			//sprintf(xmlfile,"%s<analog%d>%d</analog%d>\r\n",xmlfile,i,analog,i);
//		}

		psu_mode = GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.psu_mode;
//		if((psu_mode != 0)&&(psu_mode != 1)&&(psu_mode != 2))
//		{
//			psu_mode = 3;
//		}
		
		i_led[0]=1;
		sprintf(str_psu_mode,"%s","TBD");
		
		switch (psu_mode)
		{
			case 0:
			{
				sprintf(str_psu_mode,"%s","CV");
				break;
			}
			case 1:
			{
				sprintf(str_psu_mode,"%s","CI");
				break;
			}
			case 2:
			{
				sprintf(str_psu_mode,"%s","CPOT");
				break;
			}
			case 3:
			{
				sprintf(str_psu_mode,"%s","OFF");
				i_led[0]=0;
				break;
			}
		}
		
		refelec_active = GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_active;
		
		strcpy(read_e_en[0],"INACTIVE");
		strcpy(read_e_en[1],"INACTIVE");
		strcpy(read_e_en[2],"INACTIVE");
		strcpy(read_e_en[3],"INACTIVE");
		
		if(refelec_active>>0 & 0x0001){strcpy(read_e_en[0],"ACTIVE");}
		if(refelec_active>>1 & 0x0001){strcpy(read_e_en[1],"ACTIVE");}
		if(refelec_active>>2 & 0x0001){strcpy(read_e_en[2],"ACTIVE");}
		if(refelec_active>>3 & 0x0001){strcpy(read_e_en[3],"ACTIVE");}
		
if((GuiState.gui_scn_state == G_CV)||(GuiState.gui_scn_state == G_CI)||(GuiState.gui_scn_state == G_CP))
{
	sprintf(xmlfile,"%s<config_state>%s</config_state>\r\n",xmlfile,"1");
	//sprintf(xmlfile,"%s<config_msg>%s</config_msg>\r\n",xmlfile,"Configuration being edited locally. Please wait...");
}
else
{
	sprintf(xmlfile,"%s<config_state>%s</config_state>\r\n",xmlfile,"0");
	//sprintf(xmlfile,"%s<config_msg>%s</config_msg>\r\n",xmlfile,"Ready.");
}
		
		sprintf(xmlfile,"%s<node>%d</node>\r\n",xmlfile,GuiState.psu_viewing);

		for(i=0;i<4;i++)
		{			
				if(GuiState.SysDataPSU[i].PSU_ADDR == 0x00ff) //OFF
				{
					node_can_onoff = 0;
				}
				else //ON
				{
					node_can_onoff = 1;
				}
				
				sprintf(xmlfile,"%s<node_%d_can>%d</node_%d_can>\r\n",xmlfile,i,node_can_onoff,i);
		}
						
		sprintf(xmlfile,"%s<mode>%s</mode>\r\n",xmlfile,str_psu_mode);
		sprintf(xmlfile,"%s<set_volts>%2.1f</set_volts>\r\n",xmlfile,(float)GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.rec_vset/10);
		sprintf(xmlfile,"%s<set_amps>%2.1f</set_amps>\r\n",xmlfile,(float)GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.rec_iset/10);
		sprintf(xmlfile,"%s<read_volts>%2.1f</read_volts>\r\n",xmlfile,(float)GuiState.SysDataPSU[GuiState.psu_viewing].Vrdbk/10);
		sprintf(xmlfile,"%s<read_amps>%2.1f</read_amps>\r\n",xmlfile,(float)GuiState.SysDataPSU[GuiState.psu_viewing].Irdbk/10);
		
		sprintf(xmlfile,"%s<led_0>%d</led_0>\r\n",xmlfile,i_led[0]);
		sprintf(xmlfile,"%s<led_1>%d</led_1>\r\n",xmlfile,i_led[1]);
		sprintf(xmlfile,"%s<led_2>%d</led_2>\r\n",xmlfile,i_led[2]);
		sprintf(xmlfile,"%s<led_3>%d</led_3>\r\n",xmlfile,i_led[3]);
		sprintf(xmlfile,"%s<led_4>%d</led_4>\r\n",xmlfile,i_led[4]);
		sprintf(xmlfile,"%s<led_5>%d</led_5>\r\n",xmlfile,i_led[5]);
		sprintf(xmlfile,"%s<led_6>%d</led_6>\r\n",xmlfile,i_led[6]);
		
		sprintf(xmlfile,"%s<read_e_dmd>%2.3f</read_e_dmd>\r\n",xmlfile,(float)GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_dmd_setpt/1000);
		
//str_replace(http_file, strKey, memberValue[j]);
		strcpy(str_e_desc,GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refeleclongname[0]);
//		str_replace(str_e_desc, "<", "&lt;");
//		str_replace(str_e_desc, ">", "&gt;");
		str_replace(str_e_desc, "<", "_");
		str_replace(str_e_desc, ">", "_");
		sprintf(xmlfile,"%s<read_e1_desc>%s</read_e1_desc>\r\n",xmlfile,str_e_desc);
		
		strcpy(str_e_desc,GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refeleclongname[1]);
		str_replace(str_e_desc, "<", "_");
		str_replace(str_e_desc, ">", "_");
		sprintf(xmlfile,"%s<read_e2_desc>%s</read_e2_desc>\r\n",xmlfile,str_e_desc);
		
		strcpy(str_e_desc,GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refeleclongname[2]);
		str_replace(str_e_desc, "<", "_");
		str_replace(str_e_desc, ">", "_");
		sprintf(xmlfile,"%s<read_e3_desc>%s</read_e3_desc>\r\n",xmlfile,str_e_desc);
		
		strcpy(str_e_desc,GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refeleclongname[3]);
		str_replace(str_e_desc, "<", "_");
		str_replace(str_e_desc, ">", "_");
		sprintf(xmlfile,"%s<read_e4_desc>%s</read_e4_desc>\r\n",xmlfile,str_e_desc);

//		sprintf(xmlfile,"%s<read_e1_desc>%s</read_e1_desc>\r\n",xmlfile,GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refeleclongname[0]);
//		sprintf(xmlfile,"%s<read_e2_desc>%s</read_e2_desc>\r\n",xmlfile,GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refeleclongname[1]);
//		sprintf(xmlfile,"%s<read_e3_desc>%s</read_e3_desc>\r\n",xmlfile,GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refeleclongname[2]);
//		sprintf(xmlfile,"%s<read_e4_desc>%s</read_e4_desc>\r\n",xmlfile,GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refeleclongname[3]);
		
		sprintf(xmlfile,"%s<read_e1_dmd>%2.3f</read_e1_dmd>\r\n",xmlfile,(float)GuiState.SysDataPSU[GuiState.psu_viewing].RefCells[0]/1000);
		sprintf(xmlfile,"%s<read_e2_dmd>%2.3f</read_e2_dmd>\r\n",xmlfile,(float)GuiState.SysDataPSU[GuiState.psu_viewing].RefCells[1]/1000);
		sprintf(xmlfile,"%s<read_e3_dmd>%2.3f</read_e3_dmd>\r\n",xmlfile,(float)GuiState.SysDataPSU[GuiState.psu_viewing].RefCells[2]/1000);
		sprintf(xmlfile,"%s<read_e4_dmd>%2.3f</read_e4_dmd>\r\n",xmlfile,(float)GuiState.SysDataPSU[GuiState.psu_viewing].RefCells[3]/1000);
		
		sprintf(xmlfile,"%s<read_e1_en>%s</read_e1_en>\r\n",xmlfile,read_e_en[0]);
		sprintf(xmlfile,"%s<read_e2_en>%s</read_e2_en>\r\n",xmlfile,read_e_en[1]);
		sprintf(xmlfile,"%s<read_e3_en>%s</read_e3_en>\r\n",xmlfile,read_e_en[2]);
		sprintf(xmlfile,"%s<read_e4_en>%s</read_e4_en>\r\n",xmlfile,read_e_en[3]);
		
		sprintf(xmlfile,"%s%s\r\n",xmlfile,"</response>");
		
		analog++;		
    //write(conn, (const unsigned char*)(file->data), (size_t)file->len);
		write(conn, (const unsigned char*)(xmlfile), (size_t)strlen(xmlfile));
    //if(file) fs_close(file);
  }
  else if ((strncmp((char *)recv_buffer,"GET /zonecfg.htm", 16) == 0)||(strncmp((char *)recv_buffer,"POST /zonecfg.htm", 17) == 0))
  {

		//char *ns = NULL;
		int i = 0;
		int j = 0;
		
		err_t err_post;
		char cmpValue[16] = "TEST";
		char postName[16] = "TEST";
		char strKey[16] = "";
		int zoneID = -1;
		char zoneValue[16] = { 0 };
		char memberValue[10][16] = { 0 };

		if(strncmp((char *)recv_buffer,"POST", 4) == 0)
		{
			strcpy(postName,"zoneid");
			err_post = http_getlinevalue( postName, (char *)recv_buffer, zoneValue, (u32_t)sizeof(zoneValue));
			if(err_post == ERR_OK)
			{
				for (i = 0; i < 6; i++)
				{
					sprintf(cmpValue,"zone%d",i);
					if(strncmp(cmpValue,zoneValue, 5) == 0)
					{
						zoneID = i;
					}
				}
			}
			else
			{
				zoneID = -1;
			}
			
			for (i = 0; i < 10; i++)
			{
				sprintf(postName,"blade%d",i);
				err_post = http_getlinevalue( postName, (char *)recv_buffer, memberValue[i], (u32_t)sizeof(memberValue[i]));
				if(err_post != ERR_OK)
				{
					strcpy(memberValue[i],"ERROR");
				}
			}
		}
		
		get_http_file("/zonecfg.htm");
				
		insert_inc_file("~inc:header.inc~","/header.inc");
		
		if(zoneID>=0)
		{
			for (i = 0; i < 6; i++)
			{
				for (j = 0; j < 10; j++)
				{
					sprintf(strKey,"~zone(%d,%d)~",i,j);
					if(i==zoneID)
					{
						str_replace(http_file, strKey, memberValue[j]);
					}
					else
					{
						str_replace(http_file, strKey, "?");
					}
				}
			}
		}
		
		insert_inc_file("~inc:footer.inc~","/footer.inc");

		trim_html_file(http_file); //Hack - for html files only - see function for more details

		write(conn, (const unsigned char*)(http_file), (size_t)(strlen(http_file)));

  }
  else if (strncmp((char *)recv_buffer,"GET /atdi_iccp.css", 18) == 0)
  {
    file = fs_open("/atdi_iccp.css"); 
    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
    if(file) fs_close(file);
  }
  else if(strncmp((char *)recv_buffer, "GET /STM32F4xxTASKS.html", 24) == 0)
  {
    /* Load dynamic page */
    DynWebPage(conn);
  }
//  else if((strncmp((char *)recv_buffer, "GET /index.htm", 14) == 0)||(strncmp((char *)recv_buffer, "GET / ", 6) == 0)||(strncmp((char *)recv_buffer,"POST /index.htm", 15) == 0))
//  {
	else if((strncmp((char *)recv_buffer, "GET /index.htm", 14) == 0)||(strncmp((char *)recv_buffer, "GET / ", 6) == 0)||(strncmp((char *)recv_buffer,"POST /index.htm", 15) == 0))
  {
			err_t err_post;
		int psu_mode = -1; //TBD - Invalid
		//char cmpValue[16] = { 0 };
		char postName[16] = { 0 };

		char postValue[16] = { 0 };
		
		//Return page first - process POST(s) second.
		//NOTE: JavaScript in page processing status.xml will fill in data.
		file = fs_open("/index.htm"); 
    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
    if(file) fs_close(file);
		
		//Process POST(s)

		if(strncmp((char *)recv_buffer,"POST", 4) == 0)
		{
			//strcpy(postName,"submode");
			//strcpy(cmpValue,"SUBMIT");
						
			//err_post = http_getlinevalue( postName, (char *)recv_buffer, postValue, (u32_t)sizeof(postValue));
			err_post = http_getlinevalue( "submitCfg", (char *)recv_buffer, postValue, (u32_t)sizeof(postValue));
			if(err_post == ERR_OK)
			{
					//if(strncmp(cmpValue,postValue, sizeof(postValue)) == 0)
					if(strcmp(postValue,"SUBMIT") == 0)
					{
						err_post = http_getlinevalue( "selectCfg", (char *)recv_buffer, postValue, (u32_t)sizeof(postValue));
						if(err_post == ERR_OK)
						{
							if(strcmp(postValue,"NODE") == 0)
							{
									err_post = http_getlinevalue( "selectNode", (char *)recv_buffer, postValue, (u32_t)sizeof(postValue));
									if(err_post == ERR_OK)
									{
										if(strncmp(postValue,"0",1) == 0)
										{
											GuiState.psu_viewing = 0;
										}										
										else if(strncmp(postValue,"1",1) == 0)
										{
											GuiState.psu_viewing = 1;
										}										
										else if(strncmp(postValue,"2",1) == 0)
										{
											GuiState.psu_viewing = 2;
										}										
										else if(strncmp(postValue,"3",1) == 0)
										{
											GuiState.psu_viewing = 3;
										}
									}
							}
							
							else if(strcmp(postValue,"OUTPUT") == 0)
							{						
								err_post = http_getlinevalue( "outputEnable", (char *)recv_buffer, postValue, (u32_t)sizeof(postValue));
								if(err_post == ERR_OK)
								{
									if(strcmp(postValue,"ON") == 0)
									{
										GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.psu_mode = GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.last_psu_mode;
									}
									
									else if(strcmp(postValue,"OFF") == 0)
									{
											GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.last_psu_mode = GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.psu_mode;
											GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.psu_mode = P_OFF;
									}
								}
							}
							
							else if(strcmp(postValue,"MODE") == 0)
							{
								err_post = http_getlinevalue( "selectMode", (char *)recv_buffer, postValue, (u32_t)sizeof(postValue));
								if(err_post == ERR_OK)
								{
									if(strcmp(postValue,"CV") == 0)
									{
										err_post = http_getlinevalue( "volts", (char *)recv_buffer, postValue, (u32_t)sizeof(postValue));
										if(err_post == ERR_OK)
										{
											GuiState.SysDataPSU[GuiState.psu_viewing].rec_vset   = (unsigned int)(atof(postValue)*10);
											GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.rec_vset = (unsigned int)(atof(postValue)*10);
											GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.rec_iset = 500; // set I to 50.0 amps when in CI mode
											GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.psu_mode   = P_CV;
										}
									}
									else if(strcmp(postValue,"CI") == 0)
									{
										err_post = http_getlinevalue( "amps", (char *)recv_buffer, postValue, (u32_t)sizeof(postValue));
										if(err_post == ERR_OK)
										{
											GuiState.SysDataPSU[GuiState.psu_viewing].rec_iset   = (unsigned int)(atof(postValue)*10);
											GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.rec_iset = (unsigned int)(atof(postValue)*10);
											GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.rec_vset = 500;//Set to 50.V when in CI mode
											GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.psu_mode   = P_CI;
										}
									}
									else if(strcmp(postValue,"CPOT") == 0)
									{
										GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.psu_mode   = P_CPOT;
										
										err_post = http_getlinevalue( "eset", (char *)recv_buffer, postValue, (u32_t)sizeof(postValue));
										if(err_post == ERR_OK)
										{
											GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.refelec_dmd_setpt = (unsigned int)(atof(postValue)*1000);
										}
										
										err_post = http_getlinevalue( "e1_en", (char *)recv_buffer, postValue, (u32_t)sizeof(postValue));
										if(err_post != ERR_OK)
										{
											REFELEC_ACTIVE &= 0xfe;//and off bit0
										}
										else
										{
											REFELEC_ACTIVE |= 0x01;//or on bit0
										}

										err_post = http_getlinevalue( "e2_en", (char *)recv_buffer, postValue, (u32_t)sizeof(postValue));
										if(err_post != ERR_OK)
										{
												REFELEC_ACTIVE &= 0xfd;//and off bit1
										}
										else
										{
												REFELEC_ACTIVE |= 0x02;//or on bit1
										}

										err_post = http_getlinevalue( "e3_en", (char *)recv_buffer, postValue, (u32_t)sizeof(postValue));
										if(err_post != ERR_OK)
										{
												REFELEC_ACTIVE &= 0xfb;//and off bit2
										}
										else
										{
												REFELEC_ACTIVE |= 0x04;//or on bit2
										}

										err_post = http_getlinevalue( "e4_en", (char *)recv_buffer, postValue, (u32_t)sizeof(postValue));
										if(err_post != ERR_OK)
										{
												REFELEC_ACTIVE &= 0xf7;//and off bit3
										}
										else
										{
												REFELEC_ACTIVE |= 0x08;//or on bit3
										}
									}
									
									if(!GuiState.demo){while(!(BITCHK(GuiState.SysDataPSU[GuiState.psu_viewing].Status,3)));}//Wait for status update to complete
									CANWrite(GuiState.psu_viewing,0x00,CMD_CAN_204,(uint8_t*)postValue,0);// send profile to PSU RAM
									//GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.psu_mode = psu_mode;
								}
							//GuiState.SysDataPSU[GuiState.psu_viewing].pflparams.psu_mode = 3;
							}
						}
					}
			}
		}
			
//    file = fs_open("/index.htm"); 
//    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
//    if(file) fs_close(file);
  }
//  else if((strncmp((char *)recv_buffer, "GET /index.htm", 14) == 0)||(strncmp((char *)recv_buffer, "GET / ", 6) == 0)||(strncmp((char *)recv_buffer,"POST /index.htm", 15) == 0))
//  {
//    /* Load index page */
////    file = fs_open("/index.htm"); 
////    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
////    if(file) fs_close(file);
//		
//		get_http_file("/index.htm");
//				
//		insert_inc_file("~inc:header.inc~","/header.inc");
//				
//		insert_inc_file("~inc:footer.inc~","/footer.inc");

//		trim_html_file(http_file); //Hack - for html files only - see function for more details

//		write(conn, (const unsigned char*)(http_file), (size_t)(strlen(http_file)));
//  }
  else if(strncmp((char *)recv_buffer, "GET /cfgci.htm", 14) == 0)
  {
		get_http_file("/cfgci.htm");
				
		insert_inc_file("~inc:header.inc~","/header.inc");
				
		insert_inc_file("~inc:footer.inc~","/footer.inc");

		trim_html_file(http_file); //Hack - for html files only - see function for more details

		write(conn, (const unsigned char*)(http_file), (size_t)(strlen(http_file)));
  }
  else if(strncmp((char *)recv_buffer, "GET /cfgcp.htm", 14) == 0)
  {
//		file = fs_open("/cfgcp.htm"); 
//    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
//    if(file) fs_close(file);
		
		get_http_file("/cfgcp.htm");
				
		insert_inc_file("~inc:header.inc~","/header.inc");
				
		insert_inc_file("~inc:footer.inc~","/footer.inc");

		trim_html_file(http_file); //Hack - for html files only - see function for more details

		write(conn, (const unsigned char*)(http_file), (size_t)(strlen(http_file)));
  }
  else if(strncmp((char *)recv_buffer, "GET /cfgcv.htm", 14) == 0)
  {
		get_http_file("/cfgcv.htm");
				
		insert_inc_file("~inc:header.inc~","/header.inc");
				
		insert_inc_file("~inc:footer.inc~","/footer.inc");

		trim_html_file(http_file); //Hack - for html files only - see function for more details

		write(conn, (const unsigned char*)(http_file), (size_t)(strlen(http_file)));
  }
  else
  {
    /* Load 404 page */
    file = fs_open("/404.html"); 
    write(conn, (const unsigned char*)(file->data), (size_t)file->len);
    if(file) fs_close(file);
  }
  /* Close connection socket */
  close(conn);
}

/**
  * @brief  http server thread 
  * @param arg: pointer on argument(not used here) 
  * @retval None
  */
static void http_server_socket_thread(void *arg)
{
  int sock, newconn, size;
  struct sockaddr_in address, remotehost;

 /* create a TCP socket */
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
  {
    return;
  }
  
  /* bind to port 80 at any interface */
  address.sin_family = AF_INET;
  address.sin_port = htons(80);
  address.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0)
  {
    return;
  }
  
  /* listen for incoming connections (TCP listen backlog = 5) */
  listen(sock, 5);
  
  size = sizeof(remotehost);
  
  while (1) 
  {
    newconn = accept(sock, (struct sockaddr *)&remotehost, (socklen_t *)&size);
    http_server_serve(newconn);
  }
}

/**
  * @brief  Initialize the HTTP server (start its thread) 
  * @param  none
  * @retval None
  */
void http_server_socket_init()
{
  sys_thread_new("HTTP", http_server_socket_thread, NULL, DEFAULT_THREAD_STACKSIZE * 2, WEBSERVER_THREAD_PRIO);
}


/**
  * @brief  Create and send a dynamic Web Page. This page contains the list of 
  *         running tasks and the number of page hits. 
  * @param  conn connection socket
  * @retval None
  */
void DynWebPage(int conn)
{
	xTaskStatusType  ProcessStatus[16] ={{0}};
	int      Idx;
  //char     str[3];
  int16_t  tasks_nbr;
  
  portCHAR pagehits[10];

  memset(PAGE_BODY, 0,512);
	
	

  /* Update the hit count */
  nPageHits++;
  sprintf( pagehits, "%d", (int)nPageHits );
  strcat(PAGE_BODY, pagehits);
  strcat((char *) PAGE_BODY, "<pre><br>Name          State  Priority  Stack   Num" );
  strcat((char *) PAGE_BODY, "<br>---------------------------------------------<br>"); 
    
  /* The list of tasks and their status */
  //osThreadList((signed char *)(PAGE_BODY + strlen(PAGE_BODY)));
  tasks_nbr = uxTaskGetSystemState( ProcessStatus, 16, NULL );
  
  /*Limit view size */
  if(tasks_nbr > 16) { tasks_nbr = 16;}
  
  for (Idx = 0; Idx < tasks_nbr; Idx ++)
  {
		sprintf( dbgstrweb, "%s %i %i %i %i \n",ProcessStatus[Idx].pcTaskName,(int)ProcessStatus[Idx].eCurrentState,
							(int)ProcessStatus[Idx].uxBasePriority,(int)ProcessStatus[Idx].uxCurrentPriority,(int)ProcessStatus[Idx].usStackHighWaterMark );
		strcat(PAGE_BODY, dbgstrweb);
	}
	
	strcat((char *) PAGE_BODY, "<br><br>---------------------------------------------"); 
  strcat((char *) PAGE_BODY, "<br>B : Blocked, R : Ready, D : Deleted, S : Suspended<br>");
  
  /* Send the dynamically generated page */
  write(conn, PAGE_START, strlen((char*)PAGE_START));
  write(conn, PAGE_BODY, strlen(PAGE_BODY));
}

void trim_html_file(char *html_file)
{
	char tempchar = 0x00;
	int x = -1;
	
		do {
		//Hack to check that ">" is last character in tempfile.
		//Otherwise, garbage could show up at end of file.
		//Not foolproof as garbage might include undesired ">"
		//Shuld really rewrite str_replace()
		x++;
		tempchar = html_file[(strlen(html_file)-x)]; //Check "last" character and work our way back
	} while (tempchar != '>');
	
	x--;
	
	html_file[(strlen(html_file)-x)] = 0x00;
}

char *fs_open2(const char *name)
{
  struct fs_file *file;
	struct fs_file *file_temp;
	//char *file_temp2 = NULL;
	char strtmp[16] = { 0 };
	static int x = 0;
	int sz = 0;
	
	//file_temp = fs_open("/temp_do_not_open.htm");
	//(size_t)file->len
//	file_temp2 = malloc(file_temp->len + 1);
//	
//	memset(file_temp2, '\0', file_temp->len + 1);
	x++;
	
	sz = 1024 * x;
	
	file_temp2 = malloc(sz);
	
	if(file_temp2 == NULL)
	{
		x=1;

		sz = 1024 * x;

		file_temp2 = malloc(sz);
	}
	
	memset(file_temp2, '\0', sz);
	
	//if(file_temp) fs_close(file_temp);
	
//	file = fs_open(name);
//	memcpy(file_temp2, file->data, file->len);
//	//file_temp->len = file->len;
//	if(file) fs_close(file);
	sprintf(strtmp,"SIZE: %d",(sz));
	
	strcpy(file_temp2,strtmp);
	
	return file_temp2;
	//ret = malloc(retlen + 1);
}

void get_http_file(const char *httpFileName)
{
		struct fs_file * file;
		
		memset(http_file, '\0', sizeof(http_file));

		file = fs_open(httpFileName);
		strncpy(http_file,file->data,(size_t)file->len); //Copy file to local file buffer
		if(file) fs_close(file);
}

void insert_inc_file(const char *incKey, const char *incFileName)
{
		struct fs_file * file;
		//This length seems to be consistent for HTTP Headers in include files - see fsdata.c
		int inc_http_hdr_len = (17+63+28);
		int inc_len = 0;
	
		file = fs_open(incFileName);
		memcpy(inc_file, "\0", sizeof(inc_file));
		inc_len = (file->len-inc_http_hdr_len);
		memcpy(inc_file,file->data+inc_http_hdr_len,(size_t)(inc_len));//Copy file to local file buffer
		inc_file[inc_len]=0x00;
    if(file) fs_close(file);
		str_replace(http_file, incKey, inc_file);//Replace search string with replacement string
}

void str_replace(char *target, const char *needle, const char *replacement)
{
    //char buffer[20000] = { 0 };
		memcpy(buffer, "\0", sizeof(buffer));
    char *insert_point = &buffer[0];
    const char *tmp = target;
    size_t needle_len = strlen(needle);
    size_t repl_len = strlen(replacement);
	
//		int x = -1;
//		char tempchar;

    while (1) {
        const char *p = strstr(tmp, needle);

        // walked past last occurrence of needle; copy remaining part
        if (p == NULL) {
            strcpy(insert_point, tmp);
            break;
        }

        // copy part before needle
        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;

        // copy replacement string
        memcpy(insert_point, replacement, repl_len);
        insert_point += repl_len;

        // adjust pointers, move on
        tmp = p + needle_len;
    }

    // write altered string back to target
    strcpy(target, buffer);
	
//	x = sizeof(target);
//	tempchar = target[x];
////	do {
////		//Hack to check that ">" is last character in tempfile.
////		//Otherwise, garbage could show up at end of file.
////		//Should probably be handled in str_replace()
////		x++;
////		tempchar = target[(strlen(target)-x)];
////	} while (tempchar != '>');
//	
//	while((unsigned char)(tempchar)>128)
//	{
//		target[x]=0x00;
//		x--;
//		tempchar = target[x];
//	}
	
	//x--;
	
	//target[(strlen(target)-x)] = '\0';
		
		//target[strlen(target)]=0x00;
}
//From: http://creativeandcritical.net/str-replace-c
char *repl_str(const char *str, const char *old, const char *new) {
//void repl_str(char *str, const char *old, const char *new) {

	/* Adjust each of the below values to suit your needs. */

	/* Increment positions cache size initially by this number. */
	size_t cache_sz_inc = 16;
	/* Thereafter, each time capacity needs to be increased,
	 * multiply the increment by this factor. */
	const size_t cache_sz_inc_factor = 3;
	/* But never increment capacity by more than this number. */
	const size_t cache_sz_inc_max = 1048576;

	char *pret, *ret = NULL;
	const char *pstr2, *pstr = str;
	size_t i, count = 0;
	ptrdiff_t *pos_cache = NULL;
	size_t cache_sz = 0;
	size_t cpylen, orglen, retlen, newlen, oldlen = strlen(old);

	/* Find all matches and cache their positions. */
	while ((pstr2 = strstr(pstr, old)) != NULL) {
		count++;

		/* Increase the cache size when necessary. */
		if (cache_sz < count) {
			cache_sz += cache_sz_inc;
			pos_cache = realloc(pos_cache, sizeof(*pos_cache) * cache_sz);
			if (pos_cache == NULL) {
				goto end_repl_str;
			}
			cache_sz_inc *= cache_sz_inc_factor;
			if (cache_sz_inc > cache_sz_inc_max) {
				cache_sz_inc = cache_sz_inc_max;
			}
		}

		pos_cache[count-1] = pstr2 - str;
		pstr = pstr2 + oldlen;
	}

	orglen = pstr - str + strlen(pstr);

	/* Allocate memory for the post-replacement string. */
	if (count > 0) {
		newlen = strlen(new);
		retlen = orglen + (newlen - oldlen) * count;
	} else	retlen = orglen;
	ret = malloc(retlen + 1);
	if (ret == NULL) {
		goto end_repl_str;
	}

	if (count == 0) {
		/* If no matches, then just duplicate the string. */
		strcpy(ret, str);
	} else {
		/* Otherwise, duplicate the string whilst performing
		 * the replacements using the position cache. */
		pret = ret;
		memcpy(pret, str, pos_cache[0]);
		pret += pos_cache[0];
		for (i = 0; i < count; i++) {
			memcpy(pret, new, newlen);
			pret += newlen;
			pstr = str + pos_cache[i] + oldlen;
			cpylen = (i == count-1 ? orglen : pos_cache[i+1]) - pos_cache[i] - oldlen;
			memcpy(pret, pstr, cpylen);
			pret += cpylen;
		}
		ret[retlen] = '\0';
	}

end_repl_str:
	/* Free the cache and return the post-replacement string,
	 * which will be NULL in the event of an error. */
	free(pos_cache);
	return ret;
//	strcpy(str, ret);
//	free(ret);
//	return;
}

////char *str_replace ( const char *string, const char *substr, const char *replacement ){
//void str_replace2( char *string, const char *substr, const char *replacement ){
//  char *tok = NULL;
//  char *newstr = NULL;
//  char *oldstr = NULL;
//  char *head = NULL;
// 
//  /* if either substr or replacement is NULL, duplicate string a let caller handle it */
//  if ( substr == NULL || replacement == NULL ) //return strdup (string);
//	{
//		return;
//	}
//  newstr = strdup(string);
//  head = newstr;
//  while ( (tok = strstr ( head, substr ))){
//    oldstr = newstr;
//    newstr = malloc ( strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) + 1 );
//    /*failed to alloc mem, free old string and return NULL */
//    if ( newstr == NULL ){
//      free (oldstr);
//      //return NULL;
//			return;
//    }
//    memcpy ( newstr, oldstr, tok - oldstr );
//    memcpy ( newstr + (tok - oldstr), replacement, strlen ( replacement ) );
//    memcpy ( newstr + (tok - oldstr) + strlen( replacement ), tok + strlen ( substr ), strlen ( oldstr ) - strlen ( substr ) - ( tok - oldstr ) );
//    memset ( newstr + strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) , 0, 1 );
//    /* move back head right after the last replacement */
//    head = newstr + (tok - oldstr) + strlen( replacement );
//    free (oldstr);
//  }
//  //return newstr;
//	strcpy(newstr,string);
//	free (newstr);
//	return;
//}

//From: https://lists.gnu.org/archive/html/lwip-users/2007-07/msg00057.html
err_t http_getlinevalue( char* lpszValueName, char* lpszLine, char* lpszBuffer, 
u32_t dwBufferSize)
{ err_t  bResult      = ERR_ARG;
  char* lpszValue     = NULL;
  char  szValue[256]  = "";
	//int i = 0;
  
  memset( lpszBuffer, 0, dwBufferSize);
  memset( szValue,    0, sizeof(szValue));
     
  lpszValue = strstr( lpszLine, lpszValueName);
  if (lpszValue!=NULL)
   { if (sscanf( lpszValue+strlen(lpszValueName)+1/*=*/, "%[^ ;&\r\n]", 
szValue)>=0)
      { /* Note you have to process special escape sequence, or special chars: 
by example, any "+" have to be replace by a " " */
        /* Can you find in your datas some sequence like "%20". You have to 
replace them by the a matching ASCII value. For "%20", its 0x20, so, a "space"
         etc...*/

				strcpy(lpszBuffer,szValue);
        bResult = ERR_OK;
      }
   }
  return bResult;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
