/*----------------------------------------------------------------------------
 *      RL-ARM - CAN
 *----------------------------------------------------------------------------
 *      Name:    RTX_CAN.c
 *      Purpose: CAN Generic Layer Driver
 *      Rev.:    V4.70
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2013 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include "cmsis_os.h"                      /* RTX kernel functions & defines      */
#include "RTX_CAN.h"                       /* CAN Generic functions & defines     */

#pragma diag_suppress 550

#if ((CAN_CTRL_MAX_NUM < 1) || (CAN_CTRL_MAX_NUM > 4))
  #error "Error in CAN Controller definition"
#else
  /* Declare memory pool for CAN messages, both transmit and receive           */
  osPoolId rlcan_mpool;
  osPoolDef(rlcan_mpool, CAN_CTRL_MAX_NUM*(CAN_No_SendObjects+CAN_No_ReceiveObjects), CAN_msg); // Define memory pool

  /* Declare mailboxes for CAN transmit messages                                */
  osMessageQId rlcan_msgq_tx[CAN_CTRL_MAX_NUM];
  /* Declare mailboxes for CAN receive messages                                 */
  osMessageQId rlcan_msgq_rx[CAN_CTRL_MAX_NUM];

  #if (CAN_CTRL_MAX_NUM > 0)
  osMessageQDef(rlcan_msgq_tx0, CAN_No_SendObjects,    CAN_msg); // Define mail queue
  osMessageQDef(rlcan_msgq_rx0, CAN_No_ReceiveObjects, CAN_msg); // Define mail queue
  #endif

  #if (CAN_CTRL_MAX_NUM > 1)
  osMessageQDef(rlcan_msgq_tx1, CAN_No_SendObjects,    CAN_msg); // Define mail queue
  osMessageQDef(rlcan_msgq_rx1, CAN_No_ReceiveObjects, CAN_msg); // Define mail queue
  #endif

  #if (CAN_CTRL_MAX_NUM > 2)
  osMessageQDef(rlcan_msgq_tx2, CAN_No_SendObjects,    CAN_msg); // Define mail queue
  osMessageQDef(rlcan_msgq_rx2, CAN_No_ReceiveObjects, CAN_msg); // Define mail queue
  #endif

  #if (CAN_CTRL_MAX_NUM > 3)
  osMessageQDef(rlcan_msgq_tx3, CAN_No_SendObjects,    CAN_msg); // Define mail queue
  osMessageQDef(rlcan_msgq_rx3, CAN_No_ReceiveObjects, CAN_msg); // Define mail queue
  #endif

  /* Semaphores used for protecting writing to CAN hardware                    */
  osSemaphoreId rlcan_wr_sem[CAN_CTRL_MAX_NUM];     // Semaphore ID

  #if (CAN_CTRL_MAX_NUM > 0)
    osSemaphoreDef(rlcan_wr_sem_0);                 // Empty Semaphore constant definition
  #endif

  #if (CAN_CTRL_MAX_NUM > 1)
    osSemaphoreDef(rlcan_wr_sem_1);                 // Empty Semaphore constant definition
  #endif

  #if (CAN_CTRL_MAX_NUM > 2)
    osSemaphoreDef(rlcan_wr_sem_2);                 // Empty Semaphore constant definition
  #endif

  #if (CAN_CTRL_MAX_NUM > 3)
    osSemaphoreDef(rlcan_wr_sem_3);                 // Empty Semaphore constant definition
  #endif
#endif

/*----------------------------------------------------------------------------
 *      CAN RTX Generic Driver Functions
 *----------------------------------------------------------------------------
 *  Functions implemented in this module:
 *           CAN_ERROR CAN_mem_init  (void);
 *           CAN_ERROR CAN_setup     (void)
 *           CAN_ERROR CAN_init      (uint32_t ctrl, uint32_t baudrate)
 *           CAN_ERROR CAN_start     (uint32_t ctrl)
 *    static CAN_ERROR CAN_push      (uint32_t ctrl, CAN_msg *msg, uint16_t timeout)
 *           CAN_ERROR CAN_send      (uint32_t ctrl, CAN_msg *msg, uint16_t timeout)
 *           CAN_ERROR CAN_request   (uint32_t ctrl, CAN_msg *msg, uint16_t timeout)
 *           CAN_ERROR CAN_set       (uint32_t ctrl, CAN_msg *msg, uint16_t timeout)
 *    static CAN_ERROR CAN_pull      (uint32_t ctrl, CAN_msg *msg, uint16_t timeout)
 *           CAN_ERROR CAN_receive   (uint32_t ctrl, CAN_msg *msg, uint16_t timeout)
 *           CAN_ERROR CAN_rx_object (uint32_t ctrl, uint32_t ch, uint32_t id, uint32_t object_para)
 *           CAN_ERROR CAN_tx_object (uint32_t ctrl, uint32_t ch,         uint32_t object_para)
 *---------------------------------------------------------------------------*/

/*--------------------------- CAN_init --------------------------------------
 *
 *  The first time this function is called initialize the memory pool for 
 *  CAN messages and setup CAN controllers hardware
 *
 *  Initialize mailboxes for CAN messages and initialize CAN controller
 *
 *  Parameter:  ctrl:       Index of the hardware CAN controller (1 .. x)
 *              baudrate:   Baudrate
 *
 *  Return:     CAN_ERROR:  Error code
 *---------------------------------------------------------------------------*/

CAN_ERROR CAN_init (uint32_t ctrl, uint32_t baudrate)  {
  static uint8_t first_run_flag = 0;
  CAN_ERROR error_code;

  /* When function is called for the first time it will initialize and setup 
     all of the resources that are common to CAN functionality               */
  if (first_run_flag == 0)  {

    first_run_flag = 1;

    rlcan_mpool = osPoolCreate(osPool(rlcan_mpool)); // create memory pool
    if (rlcan_mpool == NULL)
      return CAN_MEM_POOL_INIT_ERROR;

    #if (CAN_CTRL_MAX_NUM > 0)
    /* Initialize mailboxes before the first use */
    rlcan_msgq_tx[0] = osMessageCreate(osMessageQ(rlcan_msgq_tx0), NULL); // create mail queue
    rlcan_msgq_rx[0] = osMessageCreate(osMessageQ(rlcan_msgq_rx0), NULL); // create mail queue
    /* Initialize semaphore before the first use */
    rlcan_wr_sem[0] = osSemaphoreCreate(osSemaphore(rlcan_wr_sem_0), 1);
    #endif

    #if (CAN_CTRL_MAX_NUM > 1)
    /* Initialize mailboxes before the first use */
    rlcan_msgq_tx[1] = osMessageCreate(osMessageQ(rlcan_msgq_tx1), NULL); // create mail queue
    rlcan_msgq_rx[1] = osMessageCreate(osMessageQ(rlcan_msgq_rx1), NULL); // create mail queue
    /* Initialize semaphore before the first use */
    rlcan_wr_sem[1] = osSemaphoreCreate(osSemaphore(rlcan_wr_sem_1), 1);
    #endif

    #if (CAN_CTRL_MAX_NUM > 2)
    /* Initialize mailboxes before the first use */
    rlcan_msgq_tx[2] = osMessageCreate(osMessageQ(rlcan_msgq_tx2), NULL); // create mail queue
    rlcan_msgq_rx[2] = osMessageCreate(osMessageQ(rlcan_msgq_rx2), NULL); // create mail queue
    /* Initialize semaphore before the first use */
    rlcan_wr_sem[2] = osSemaphoreCreate(osSemaphore(rlcan_wr_sem_2), 1);
    #endif

    #if (CAN_CTRL_MAX_NUM > 3)
    /* Initialize mailboxes before the first use */
    rlcan_msgq_tx[3] = osMessageCreate(osMessageQ(rlcan_msgq_tx3), NULL); // create mail queue
    rlcan_msgq_rx[3] = osMessageCreate(osMessageQ(rlcan_msgq_rx3), NULL); // create mail queue
    /* Initialize semaphore before the first use */
    rlcan_wr_sem[3] = osSemaphoreCreate(osSemaphore(rlcan_wr_sem_3), 1);
    #endif
  }

  error_code = _CAN_hw_setup (ctrl);
  if (error_code != CAN_OK) 
    return error_code;

  return (CAN_hw_init (ctrl, baudrate));
}


/*--------------------------- CAN_start -------------------------------------
 *
 *  Start CAN controller (enable it to participate on CAN network)
 *
 *  Parameter:  ctrl:       Index of the hardware CAN controller (1 .. x)
 *
 *  Return:     CAN_ERROR:  Error code
 *---------------------------------------------------------------------------*/

CAN_ERROR CAN_start (uint32_t ctrl)  {
  return (CAN_hw_start (ctrl));
}


/*--------------------------- CAN_push --------------------------------------
 *
 *  Send CAN_msg if hardware is free for sending, otherwise push message to 
 *  message queue to be sent when hardware becomes free
 *
 *  Parameter:  ctrl:       Index of the hardware CAN controller (1 .. x)
 *              msg:        Pointer to CAN message to be sent
 *              timeout:    Timeout value for message sending
 *
 *  Return:     CAN_ERROR:  Error code
 *---------------------------------------------------------------------------*/

static CAN_ERROR CAN_push (uint32_t ctrl, CAN_msg *msg, uint16_t timeout)  {
  CAN_msg *ptrmsg;
  uint32_t ctrl0 = ctrl-1;                 /* Controller index 0 .. x-1           */
  osEvent evt;

  if (CAN_hw_tx_empty (ctrl) == CAN_OK) { /* Transmit hardware free for send */
    CAN_hw_wr (ctrl, msg);            /* Send message                        */
  }
  else {                              /* If hardware for sending is busy     */
    /* Write the message to send mailbox if there is room for it             */
    ptrmsg = osPoolAlloc(rlcan_mpool); // Allocate memory for the message
    if (ptrmsg != NULL) {
      *ptrmsg = *msg;
      /* If message hasn't been sent but timeout expired, deallocate memory  */
      if (osMessagePut(rlcan_msgq_tx[ctrl0], (uint32_t)ptrmsg, timeout) != osOK) {
        if (osPoolFree(rlcan_mpool, ptrmsg) != osOK)
          return CAN_DEALLOC_MEM_ERROR;
          
        return CAN_TIMEOUT_ERROR;
      } else {
        /* Check once again if transmit hardware is ready for transmission   */
        if (CAN_hw_tx_empty (ctrl) == CAN_OK) { /* Transmit hw free for send */ 
          evt = osMessageGet(rlcan_msgq_tx[ctrl0], 0); // wait for message
          if (evt.status == osEventMessage) {
            if (osPoolFree(rlcan_mpool, evt.value.p) != osOK) { // free memory allocated for message
              osSemaphoreRelease(rlcan_wr_sem[ctrl0]); /* Return a token back to semaphore  */
              return CAN_DEALLOC_MEM_ERROR;
            }
            CAN_hw_wr (ctrl, msg);      /* Send message                        */
          } else {
            osSemaphoreRelease(rlcan_wr_sem[ctrl0]); /* Return a token back to semaphore  */
            return CAN_OK;              /* Message was sent from IRQ already */
          }
        }
      }
    } else
      return CAN_ALLOC_MEM_ERROR;
  }
  return CAN_OK;
}


/*--------------------------- CAN_send --------------------------------------
 *
 *  Send DATA FRAME message, see CAN_push function comment
 *
 *  Parameter:  ctrl:       Index of the hardware CAN controller (1 .. x)
 *              msg:        Pointer to CAN message to be sent
 *              timeout:    Timeout value for message sending
 *
 *  Return:     CAN_ERROR:  Error code
 *---------------------------------------------------------------------------*/

CAN_ERROR CAN_send (uint32_t ctrl, CAN_msg *msg, uint16_t timeout)  {
  msg->type = DATA_FRAME;

  return (CAN_push (ctrl, msg, timeout));
}


/*--------------------------- CAN_request -----------------------------------
 *
 *  Send REMOTE FRAME message, see CAN_push function comment
 *
 *  Parameter:  ctrl:       Index of the hardware CAN controller (1 .. x)
 *              msg:        Pointer to CAN message to be sent
 *              timeout:    Timeout value for message sending
 *
 *  Return:     CAN_ERROR:  Error code
 *---------------------------------------------------------------------------*/

CAN_ERROR CAN_request (uint32_t ctrl, CAN_msg *msg, uint16_t timeout)  {
  msg->type = REMOTE_FRAME;

  return (CAN_push (ctrl, msg, timeout));
}


/*--------------------------- CAN_set ---------------------------------------
 *
 *  Set a message that will automatically be sent as an answer to REMOTE
 *  FRAME message
 *
 *  Parameter:  ctrl:       Index of the hardware CAN controller (1 .. x)
 *              msg:        Pointer to CAN message to be set
 *              timeout:    Timeout value for message to be set
 *
 *  Return:     CAN_ERROR:  Error code
 *---------------------------------------------------------------------------*/

CAN_ERROR CAN_set (uint32_t ctrl, CAN_msg *msg, uint16_t timeout)  {
  int32_t i = timeout;
  CAN_ERROR error_code;

  do {
    if (CAN_hw_tx_empty (ctrl) == CAN_OK)  {  /* Transmit hardware free      */
      error_code = CAN_hw_set (ctrl, msg);    /* Set message                 */
      osSemaphoreRelease (rlcan_wr_sem[ctrl-1]);     /* Return a token back to semaphore  */
      return error_code;
    }
    if (timeout == 0xffff)              /* Indefinite wait                   */
      i++;
    i--;
    osDelay (1);                    /* Wait 1 timer tick                 */
  }  while (i >= 0);

  return CAN_TIMEOUT_ERROR;             /* CAN message not set               */
}


/*--------------------------- CAN_pull --------------------------------------
 *
 *  Pull first received and unread CAN_msg from receiving message queue
 *
 *  Parameter:  ctrl:       Index of the hardware CAN controller (1 .. x)
 *              msg:        Pointer where CAN message will be read
 *              timeout:    Timeout value for message receiving
 *
 *  Return:     CAN_ERROR:  Error code
 *---------------------------------------------------------------------------*/


static CAN_ERROR CAN_pull (uint32_t ctrl, CAN_msg *msg, uint16_t timeout)  {
  CAN_msg *ptrmsg;
  uint32_t ctrl0 = ctrl-1;                 /* Controller index 0 .. x-1           */
  osEvent evt;

  /* Wait for received message in mailbox                                    */
  evt = osMessageGet(rlcan_msgq_rx[ctrl0], timeout); // wait for message
  if (evt.status != osEventMessage)
    return CAN_TIMEOUT_ERROR;

  /* Copy received message from mailbox to address given in function parameter msg */
  ptrmsg = (CAN_msg *)evt.value.p;
  *msg = *ptrmsg;

  /* Free box where message was kept                                         */
  if (osPoolFree(rlcan_mpool, ptrmsg) != osOK)   // free memory allocated for message
    return CAN_DEALLOC_MEM_ERROR;

  return CAN_OK;
}


/*--------------------------- CAN_receive -----------------------------------
 *
 *  Read received message, see CAN_pull function comment
 *
 *  Parameter:  ctrl:       Index of the hardware CAN controller (1 .. x)
 *              msg:        Pointer where CAN message will be read
 *              timeout:    Timeout value for message receiving
 *
 *  Return:     CAN_ERROR:  Error code
 *---------------------------------------------------------------------------*/

CAN_ERROR CAN_receive (uint32_t ctrl, CAN_msg *msg, uint16_t timeout)  {
  return (CAN_pull (ctrl, msg, timeout));
}


/*--------------------------- CAN_rx_object ---------------------------------
 *
 *  Enable reception of messages on specified controller and channel with 
 *  specified identifier
 *
 *  Parameter:  ctrl:       Index of the hardware CAN controller (1 .. x)
 *              ch:         Channel for the message transmission
 *              id:         CAN message identifier
 *              object_para:Object parameters (standard or extended format, 
 *                          data or remote frame)
 *
 *  Return:     CAN_ERROR:  Error code
 *---------------------------------------------------------------------------*/

CAN_ERROR CAN_rx_object (uint32_t ctrl, uint32_t ch, uint32_t id, uint32_t object_para)  {
  return (CAN_hw_rx_object (ctrl, ch, id, object_para));
}


/*--------------------------- CAN_tx_object ---------------------------------
 *
 *  Enable transmission of messages on specified controller and channel with 
 *  specified identifier
 *
 *  Parameter:  ctrl:       Index of the hardware CAN controller (1 .. x)
 *              ch:         Channel for the message transmission
 *              object_para:Object parameters (standard or extended format, 
 *                          data or remote frame)
 *
 *  Return:     CAN_ERROR:  Error code
 *---------------------------------------------------------------------------*/

CAN_ERROR CAN_tx_object (uint32_t ctrl, uint32_t ch, uint32_t object_para)  {
  return (CAN_hw_tx_object (ctrl, ch, object_para));
}


/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

