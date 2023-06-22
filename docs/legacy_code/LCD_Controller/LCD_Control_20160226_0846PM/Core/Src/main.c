/**
  ******************************************************************************
  * @file    main.c 
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    13-March-2015
  * @brief   This file provides main program functions
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

/* Includes ------------------------------------------------------------------*/
#include "main.h" 
#include "k_mem.h" 
#include "k_bsp.h" 
#include "k_log.h"    
#include "k_rtc.h"    
#include "k_calibration.h"    
#include "k_storage.h"   


//lwip includes
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "cmsis_os.h"
#include "ethernetif.h"
#include "app_ethernet.h"
#include "variables.h"
#include "httpserver-socket.h"
#include "iccp_can_stm32f429.h"
#include "iccpfuncs.h"

/** @addtogroup CORE
  * @{
  */

/** @defgroup MAIN
  * @brief main file
  * @{
  */ 

/** @defgroup MAIN_Private_TypesDefinitions
  * @{
  */ 

/** @defgroup MAIN_External_FunctionPrototypes
  * @{
  */ 
extern WM_HWIN GetWinHandleiccp_gph(void);
//extern WM_HWIN GetWinHandleiccp_kybd(void);



/** @defgroup MAIN_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup MAIN_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup MAIN_Private_Variables
  * @{
  */
/**
  * @}
  */ 

/** @defgroup MAIN_Private_FunctionPrototypes
  * @{
  */ 


//LwIP setup
struct netif gnetif; /* network interface structure */
/* Semaphore to signal Ethernet Link state update */
osSemaphoreId Netif_LinkSemaphore = NULL;
/* Ethernet link thread Argument */
struct link_str link_arg;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void LwIPStartThread(void const * argument);
static void ToggleLed4(void const * argument);
static void Netif_Config(void);
// end LwIP setup

static void SystemClock_Config(void);
static void GUIThread(void const * argument);
static void TimerCallback(void const *n);

extern K_ModuleItem_Typedef  system_info;
extern K_ModuleItem_Typedef  iccp_main;
extern K_ModuleItem_Typedef  file_browser;
extern K_ModuleItem_Typedef  usb_device;


uint32_t GUI_FreeMem = 0;


/**
  * @}
  */ 

/** @defgroup MAIN_Private_Functions
  * @{
  */ 

/**
  * @brief  Main program
  * @param  None
  * @retval int
  */
int main(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  uint32_t FLatency;
     
  osTimerId lcd_timer;	
	
	
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
  */
  HAL_Init();
 
  /* Configure the system clock to 168 MHz */
  SystemClock_Config();
  
  /* Initialize Joystick, Touch screen and LEDs */
  k_BspInit();
  k_LogInit(); // currently not being used was USB should be changed to SD
  
  /* Initialize RTC */
  k_CalendarBkupInit();  

  /* Add Modules */
  k_ModuleInit();  

  /* Initialize memory pools */
  k_MemInit();
	
  
	// Init Iccp data structs
	Init_Iccp();
	
	
  /* Get General settings */
    HAL_RCC_GetClockConfig(&RCC_ClkInitStruct, &FLatency);
    /* Select HSE as system clock source */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

    HAL_RCC_GetOscConfig(&RCC_OscInitStruct);  
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 360;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
    
    HAL_PWREx_EnableOverDrive();
    
    /* Select PLL as system clock source */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
  
		
  /* Create Periodic Timer to update the main ICCP state machine  */
  osTimerDef(TS_Timer, TimerCallback);
  lcd_timer =  osTimerCreate(osTimer(TS_Timer), osTimerPeriodic, (void *)0);

  /* Start the Periodic Timer */
  osTimerStart(lcd_timer, 100);
  
	
	/* Create GUI task */
  osThreadDef(GUI_Thread, GUIThread, osPriorityHigh, 0, 20 * configMINIMAL_STACK_SIZE);
  osThreadCreate (osThread(GUI_Thread), NULL); 
  k_ModuleAdd(&iccp_main); 
	k_ModuleOpenLink(&iccp_main, "txt"); 
  k_ModuleOpenLink(&iccp_main, "TXT");
	k_ModuleOpenLink(&iccp_main, "prf"); 
  k_ModuleOpenLink(&iccp_main, "PRF");
	k_ModuleOpenLink(&iccp_main, "log"); 
  k_ModuleOpenLink(&iccp_main, "LOG");
  
	k_ModuleAdd(&system_info);
  
	k_ModuleAdd(&file_browser);  
	k_ModuleOpenLink(&file_browser, "txt"); 
  k_ModuleOpenLink(&file_browser, "TXT");
	k_ModuleOpenLink(&file_browser, "prf"); 
  k_ModuleOpenLink(&file_browser, "PRF");
	k_ModuleOpenLink(&file_browser, "log"); 
  k_ModuleOpenLink(&file_browser, "LOG");
	
  k_ModuleAdd(&usb_device);   
  
  /* Initialize GUI */
  GUI_Init();
  WM_MULTIBUF_Enable(1);
  GUI_SelectLayer(1);  
  
  /* Set General Graphical proprieties */
  k_SetGuiProfile();  

	
	// Start Up Ethernet Thread
	osThreadDef(Start, LwIPStartThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2);
  osThreadCreate (osThread(Start), NULL);
  
	//	Start Up CAN Thread
  createCAN1THREAD();
	
  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  for( ;; );
}


/**
  * @brief This function provides accurate delay (in milliseconds) based 
  *        on SysTick counter flag.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @param Delay: specifies the delay time length, in milliseconds.
  * @retval None
  */

void HAL_Delay (__IO uint32_t Delay)
{
  while(Delay) 
  {
    if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) 
    {
      Delay--;
    }
  }
}

/**
  * @brief  Start task
  * @param  argument: pointer that is passed to the thread function as start argument.
  * @retval None
  */
static void GUIThread(void const * argument)
{ 
  /* Initialize Storage Units */
  k_StorageInit();  
  
  /* Check for calibration */
//  if(k_CalibrationIsDone() == 0)
//  {
//			GUI_SelectLayer(1);
//			k_CalibrationInit();
//  }  
  
  /*vk, Shows ATDI splash screen 5 secs Startup */
  k_StartUp();    
  
	// vk, we will only use layer0 for now
  GUI_SelectLayer(0);  
  
  /* vk. lets Show the main menu let user choose screen */
  k_InitMenu();  
    
  /* Gui background Task */
  int LastTimeGUIgph = 0;
	
	while(1) {
    BSP_Pointer_Update();
		GUI_FreeMem = GUI_ALLOC_GetNumFreeBytes();
		GUI_Exec(); /* Do the background work ... Update windows etc.) */
    
		BSP_LED_Toggle(LED2);
    
		if ((GUI_GetTime() - 100) > LastTimeGUIgph) {
				if(GuiState.gui_scn_state == G_GRAPHS)			
				{
						WM_InvalidateWindow(GetWinHandleiccp_gph());// invoke repaint of iccp_gph oly if it is a valid handle
				}
				LastTimeGUIgph = GUI_GetTime();  // reseed
			
    }	
	
		
		osDelay(100); /* Nothing left to do for the moment ... Idle processing */
	}
}

/**
  * @brief  Timer callback (100 ms)
  * @param  n: Timer index 
  * @retval None
  */
static void TimerCallback(void const *n)
{
		if((GuiState.gui_scn_state == G_RTSTA)||(GuiState.gui_scn_state == G_CP))
		{
					IccpDoIccp();
		}
}

/** @brief  EXTI line detection callbacks
  * @param  GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	/* Get the IT status register value */
    if(BSP_IO_ITGetStatus(MII_INT_PIN))
    {
      osSemaphoreRelease(Netif_LinkSemaphore);
    }
		
//  if(GPIO_Pin == GPIO_PIN_8)
//  {   
//    if (SD_DETECT_PIN)
//    {
//      BSP_SD_DetectIT();
//    } 
//  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  
  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig (&RCC_OscInitStruct);
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}


/**
  * @brief  Toggle Led4 task
  * @param  pvParameters not used
  * @retval None
  */
static void ToggleLed4(void const * argument)
{
  for( ;; )
  {
    /* toggle LED4 each 250ms */
    BSP_LED_Toggle(LED4);
    osDelay(250);
  }
}



// LwIP Startp Thread
/**
  * @brief  Start Thread 
  * @param  argument not used
  * @retval None
  */
static void LwIPStartThread(void const * argument)
{ 
  /* Create tcp_ip stack thread */
  tcpip_init(NULL, NULL);
  
  /* Initilaize the LwIP stack */
  Netif_Config(); 
  
  /* Initialize webserver demo */
  http_server_socket_init();
  
  /* Notify user about the netwoek interface config */
  User_notification(&gnetif);
  
#ifdef USE_DHCP
  /* Start DHCPClient */
  osThreadDef(DHCP, DHCP_thread, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE * 2);
  osThreadCreate (osThread(DHCP), &gnetif);
#endif
  
  /* Start toogleLed4 task : Toggle LED4  every 250ms */
  osThreadDef(LED4, ToggleLed4, osPriorityLow, 0, configMINIMAL_STACK_SIZE);
  osThreadCreate (osThread(LED4), NULL);
  
  for( ;; )
  {
    /* Delete the Init Thread */ 
    osThreadTerminate(NULL);
  }
}

/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
static void Netif_Config(void)
{
  struct ip_addr ipaddr;
  struct ip_addr netmask;
  struct ip_addr gw;	
  
  /* IP address default setting */
  IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
  IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
  IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
  
  /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
  struct ip_addr *netmask, struct ip_addr *gw,
  void *state, err_t (* init)(struct netif *netif),
  err_t (* input)(struct pbuf *p, struct netif *netif))
  
  Adds your network interface to the netif_list. Allocate a struct
  netif and pass a pointer to this structure as the first argument.
  Give pointers to cleared ip_addr structures when using DHCP,
  or fill them with sane numbers otherwise. The state pointer may be NULL.
  
  The init function pointer must point to a initialization function for
  your ethernet netif interface. The following code illustrates it's use.*/
  
  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
  
  /*  Registers the default network interface. */
  netif_set_default(&gnetif);
  
  if (netif_is_link_up(&gnetif))
  {
    /* When the netif is fully configured this function must be called.*/
    netif_set_up(&gnetif);
  }
  else
  {
    /* When the netif link is down this function must be called */
    netif_set_down(&gnetif);
  }

  /* Set the link callback function, this function is called on change of link status*/
  netif_set_link_callback(&gnetif, ethernetif_update_config);
  
  /* create a binary semaphore used for informing ethernetif of frame reception */
  osSemaphoreDef(Netif_SEM);
  Netif_LinkSemaphore = osSemaphoreCreate(osSemaphore(Netif_SEM) , 1 );
  
  link_arg.netif = &gnetif;
  link_arg.semaphore = Netif_LinkSemaphore;
  /* Create the Ethernet link handler thread */
  osThreadDef(LinkThr, ethernetif_set_link, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2);
  osThreadCreate (osThread(LinkThr), &link_arg);
}

// end LwIP start up thread

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
