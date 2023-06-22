/*----------------------------------------------------------------------------
 *      RL-ARM - CAN
 *----------------------------------------------------------------------------
 *      Name:    RTX_CAN.h
 *      Purpose: Header for CAN Generic Layer Driver
 *      Rev.:    V4.70
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2013 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <cmsis_os.h>                 /* RTX kernel functions & defines      */
#include "CAN_Cfg.h"                  /* CAN Configuration                   */


#ifndef __RTX_CAN_H
#define __RTX_CAN_H

#ifdef __cplusplus
extern "C"  {
#endif

#if (USE_CAN_CTRL4)
  #define CAN_CTRL_MAX_NUM      4
#elif (USE_CAN_CTRL3)
  #define CAN_CTRL_MAX_NUM      3
#elif (USE_CAN_CTRL2)
  #define CAN_CTRL_MAX_NUM      2
#elif (USE_CAN_CTRL1)
  #define CAN_CTRL_MAX_NUM      1
#else
  #error "No CAN Controller defined"
#endif

/* CAN message object structure                                              */
typedef struct {
  uint32_t id;                 /* 29 bit identifier                               */
  uint8_t  data[8];            /* Data field                                      */
  uint8_t  len;                /* Length of data field in bytes                   */
  uint8_t  ch;                 /* Object channel                                  */
  uint8_t  format;             /* 0 - STANDARD,   1 - EXTENDED IDENTIFIER         */
  uint8_t  type;               /* 0 - DATA FRAME, 1 - REMOTE FRAME                */
} CAN_msg;

/* Externaly declared memory pool for CAN messages, both transmit and receive*/
extern osPoolId rlcan_mpool;

/* Externaly declared mailbox, for CAN transmit messages                     */
extern osMessageQId rlcan_msgq_tx[CAN_CTRL_MAX_NUM];

/* Externaly declared mailbox, for CAN receive messages                      */
extern osMessageQId rlcan_msgq_rx[CAN_CTRL_MAX_NUM];

/* Semaphores used for protecting writing to CAN hardware                    */
extern osSemaphoreId rlcan_wr_sem[CAN_CTRL_MAX_NUM];

/* Symbolic names for formats of CAN message                                 */
typedef enum {STANDARD_FORMAT = 0, EXTENDED_FORMAT} CAN_FORMAT;

/* Symbolic names for type of CAN message                                    */
typedef enum {DATA_FRAME      = 0, REMOTE_FRAME   } CAN_FRAME;

/* Definitions for filter_type                                               */
#define FORMAT_TYPE     (1 << 0)
#define FRAME_TYPE      (1 << 1)
#define STANDARD_TYPE   (0 << 0)
#define EXTENDED_TYPE   (1 << 0)
#define DATA_TYPE       (0 << 1)
#define REMOTE_TYPE     (1 << 1)

/* Error values that functions can return                                    */
typedef enum   
{ CAN_OK = 0,                       /* No error                              */
  CAN_NOT_IMPLEMENTED_ERROR,        /* Function has not been implemented     */
  CAN_MEM_POOL_INIT_ERROR,          /* Memory pool initialization error      */
  CAN_BAUDRATE_ERROR,               /* Baudrate was not set                  */
  CAN_TX_BUSY_ERROR,                /* Transmitting hardware busy            */
  CAN_OBJECTS_FULL_ERROR,           /* No more rx or tx objects available    */
  CAN_ALLOC_MEM_ERROR,              /* Unable to allocate memory from pool   */
  CAN_DEALLOC_MEM_ERROR,            /* Unable to deallocate memory           */
  CAN_TIMEOUT_ERROR,                /* Timeout expired                       */
  CAN_UNEXIST_CTRL_ERROR,           /* Controller does not exist             */
  CAN_UNEXIST_CH_ERROR,             /* Channel does not exist                */
} CAN_ERROR;

/* Functions defined CAN hardware driver (module CAN_chip.c)                 */
extern CAN_ERROR CAN_hw_testmode   (uint32_t ctrl, uint32_t testmode);
extern CAN_ERROR CAN_hw_setup      (uint32_t ctrl);
extern CAN_ERROR CAN_hw_init       (uint32_t ctrl, uint32_t baudrate);
extern CAN_ERROR CAN_hw_start      (uint32_t ctrl);
extern CAN_ERROR CAN_hw_tx_empty   (uint32_t ctrl);
extern CAN_ERROR CAN_hw_wr         (uint32_t ctrl, CAN_msg *msg);
extern CAN_ERROR CAN_hw_set        (uint32_t ctrl, CAN_msg *msg);
extern CAN_ERROR CAN_hw_rx_object  (uint32_t ctrl, uint32_t ch, uint32_t id, uint32_t object_para);
extern CAN_ERROR CAN_hw_tx_object  (uint32_t ctrl, uint32_t ch,         uint32_t object_para);

#define __SVC_0         __svc_indirect(0)

#if (__TARGET_ARCH_6S_M || __TARGET_ARCH_7_M || __TARGET_ARCH_7E_M)
extern CAN_ERROR            __CAN_hw_setup (uint32_t p, uint32_t ctrl) __SVC_0;
#define _CAN_hw_setup(ctrl) __CAN_hw_setup((uint32_t)&CAN_hw_setup, ctrl);
#else
#define _CAN_hw_setup         CAN_hw_setup
#endif

/* Functions defined in module RTX_CAN.c                                     */
CAN_ERROR CAN_init                 (uint32_t ctrl, uint32_t baudrate);
CAN_ERROR CAN_start                (uint32_t ctrl);
CAN_ERROR CAN_send                 (uint32_t ctrl, CAN_msg *msg, uint16_t timeout);
CAN_ERROR CAN_request              (uint32_t ctrl, CAN_msg *msg, uint16_t timeout);
CAN_ERROR CAN_set                  (uint32_t ctrl, CAN_msg *msg, uint16_t timeout);
CAN_ERROR CAN_receive              (uint32_t ctrl, CAN_msg *msg, uint16_t timeout);
CAN_ERROR CAN_rx_object            (uint32_t ctrl, uint32_t ch, uint32_t id, uint32_t object_para);
CAN_ERROR CAN_tx_object            (uint32_t ctrl, uint32_t ch,         uint32_t object_para);

#ifdef __cplusplus
}
#endif

#endif /* __RTX_CAN_H */

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
