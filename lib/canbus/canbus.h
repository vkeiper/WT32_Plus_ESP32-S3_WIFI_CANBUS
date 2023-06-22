#ifndef __ESP32_CAN_H
#define __ESP32_CAN_H

#include <Arduino.h>
#include "driver/twai.h"




extern void setupCANbus();
extern bool CanDoMessageTx();
extern void handle_canbus_rx_message(twai_message_t& message);
extern void CANbus_RxTask();
extern bool CanTxBufferMutator(uint8_t * ptrBuff,uint8_t len, uint32_t id);

#endif