/**
  ******************************************************************************
  * @file    ICCP CAN1 iccp_can-stm32f429.h 
  * @author  Vince Keiper
  * @version V1.0.0
  * @date    24-June-2015
  * @brief   Header for iccp-can_stm32f429.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 AsrodyneTDI</center></h2>
  *
  ******************************************************************************
  */

#ifndef __ICCP_CAN_STM32F446_H
#define __ICCP_CAN_STM32F446_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm324x9i_eval.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor CANx instance used and associated 
   resources */
/* Definition for CANx clock resources */
#define CANx                            CAN1
#define CANx_CLK_ENABLE()               __HAL_RCC_CAN1_CLK_ENABLE()
#define CANx_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()
     
#define CANx_FORCE_RESET()              __HAL_RCC_CAN1_FORCE_RESET()
#define CANx_RELEASE_RESET()            __HAL_RCC_CAN1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define CANx_TX_PIN                    GPIO_PIN_9
#define CANx_TX_GPIO_PORT              GPIOB  
#define CANx_TX_AF                     GPIO_AF9_CAN1
#define CANx_RX_PIN                    GPIO_PIN_8
#define CANx_RX_GPIO_PORT              GPIOB 
#define CANx_RX_AF                     GPIO_AF9_CAN1

/* Definition for USARTx's NVIC */
#define CANx_RX_IRQn                   CAN1_RX0_IRQn
#define CANx_RX_IRQHandler             CAN1_RX0_IRQHandler

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/// int createCAN1THREAD(void);
void CAN1_Init(void);
void CAN_Run(void);
void CanTxBufferMutator(uint8_t *ptrBuff, uint8_t len, uint32_t id);

#endif /* ICCP_CAN_STM32F446_H */

/************************ (C) COPYRIGHT AstrodyneTDI *****END OF FILE****/
