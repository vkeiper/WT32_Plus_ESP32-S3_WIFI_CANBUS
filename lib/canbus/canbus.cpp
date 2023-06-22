#include "canbus.h"


//#include "ui.h"
//#include "ui_helpers.h"

// Pins used to connect to CAN bus transceiver:
#define CAN_RX_PIN 10
#define CAN_TX_PIN 11

// Intervall:
#define CANBUS_POLLING_RATE_MS 1000

static bool canbus_driver_installed = false;


//External resources
extern uint8_t b_CANTXFlag;


void setupCANbus() {
  
  // Initialize configuration structures using macro initializers
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)CAN_TX_PIN, (gpio_num_t)CAN_RX_PIN, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();  //Look in the api-reference for other speed sets.
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  // Install TWAI driver
  if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
    Serial.println("CANbus Driver installed");
  } else {
    Serial.println("Failed to install CANbus driver");
    return;
  }

  // Start TWAI driver
  if (twai_start() == ESP_OK) {
    Serial.println("CANbus Driver started");
  } else {
    Serial.println("Failed to start CANbus driver");
    return;
  }

  // Reconfigure alerts to detect frame receive, Bus-Off error and RX queue full states
  uint32_t alerts_to_enable = TWAI_ALERT_RX_DATA | TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_ERROR | TWAI_ALERT_RX_QUEUE_FULL;
  if (twai_reconfigure_alerts(alerts_to_enable, NULL) == ESP_OK) {
    Serial.println("CANbus Alerts reconfigured");
  } else {
    Serial.println("Failed to reconfigure CANbus alerts");
    return;
  }

  // TWAI driver is now successfully installed and started
  canbus_driver_installed = true;
}

/** @Author Vince Keiper
  * @brief CAN TX Buffer Mutator for HAL library.  
  *        This function sets the CanHandle.pTX eid,DLC, and tx data buffer:
  *          - Also sets boolean CANTx Flag which triggers CANThread to TX 
	*            the tx buffer
  *         
  * @param ptrBuff: pointer to lclbuffer in caller
  * @param len: length of CANTX buffer, qty bytes to send 0-8
  * @param id: function ID portion of the CANID to be TX'd 
  * @retval None
  */
bool CanTxBufferMutator(uint8_t * ptrBuff,uint8_t len, uint32_t id)
{
		int8_t i;
		bool retval = false;

    printf("CANbus - Transmiting message...\n");

    //Setup message
    twai_message_t CanTxMessage;
    CanTxMessage.identifier = id;		//11 or 29 bit identifier
    CanTxMessage.extd = 1;	
                    //extd = Extended Frame Format (29bit ID)
    CanTxMessage.rtr = 0;		//Not a Remote Transmission Request (this needs to be set to 0 to ensure this)
    CanTxMessage.data_length_code = len;

    //The data to send
    for (int i = 0; i < 8; i++)
       CanTxMessage.data[i] = *ptrBuff++;

    //SEND MESSAGE
    esp_err_t CanTxErr;
    if(b_CANTXFlag == true)
    {
      
      if ((CanTxErr = twai_transmit(&CanTxMessage, pdMS_TO_TICKS(0))) != ESP_OK)		//ESP_OK: Transmission successfully queued/initiated. <<<If the TX queue is full function will block until more space becomes available or until it times out
      {
        printf("CAN BUS - Failed to queue message for tx 0x%04X\n", CanTxErr);
        retval = false;
      }else
      {
        retval = true;
      }
    }
  return retval;
} 

//----------------------------
//----- TRANSMIT MESSAGE -----
//----------------------------
bool CanDoMessageTxTestMessage()
{
  bool retval =false;

  printf("CANbus - Transmiting message...\n");

  //Setup message
  twai_message_t CanTxMessage;
  CanTxMessage.identifier = 0x00000201;		//11 or 29 bit identifier
  CanTxMessage.extd = 1;	
                  //extd = Extended Frame Format (29bit ID)
  CanTxMessage.rtr = 0;		//Not a Remote Transmission Request (this needs to be set to 0 to ensure this)
  CanTxMessage.data_length_code = 8;

  //The data to send
  for (int i = 0; i < 8; i++)
      CanTxMessage.data[i] = i;

  //SEND MESSAGE
  esp_err_t CanTxErr;
  if ((CanTxErr = twai_transmit(&CanTxMessage, pdMS_TO_TICKS(0))) != ESP_OK)		//ESP_OK: Transmission successfully queued/initiated. <<<If the TX queue is full function will block until more space becomes available or until it times out
  {
      printf("CAN BUS - Failed to queue message for tx 0x%04X\n", CanTxErr);
      retval = false;
  }else
  {
      retval = true;
  }

  return retval;
} //if (CanDoMessageTx)

void handle_canbus_rx_message(twai_message_t& message) {
  // Process received message
  if (message.extd) {
    Serial.println("CANbus Message is in Extended Format");
  } else {
    Serial.println("CANbus Message is in Standard Format");
  }
  Serial.printf("CANbus ID: %x\nByte:", message.identifier);
  if (!(message.rtr)) {
    for (int i = 0; i < message.data_length_code; i++) {
      Serial.printf(" %d = %02x,", i, message.data[i]);
    }
    Serial.println("");
  }

  if (message.identifier == 0x00000202){
    

    Serial.printf("Pitch - %3d.%03d Deg",(int8_t)message.data[1],(int8_t)message.data[2]);
    Serial.println("");
    //sprintf(charLblPitchTxt,"%3d.%03d Deg",(int8_t)message.data[1],(int8_t)message.data[2]);

    Serial.printf("Roll - %3d.%03d Deg",(int8_t)message.data[3],(int8_t)message.data[4]);
    Serial.println("");
    //sprintf(charLblRollTxt,"%3d.%03d Deg",(int8_t)message.data[3],(int8_t)message.data[4]);
        
    // lv_label_set_text(ui_lblPitchTxt, charLblPitchTxt);
    // lv_label_set_text(ui_lblRollTxt, charLblRollTxt);

    // if outrigger is delpoyed hide red, unhide green
    // Left Rear
    if(message.data[5] &0x01){
 //     lv_obj_add_flag(ui_imgORLRRed, LV_OBJ_FLAG_HIDDEN);
 //     lv_obj_clear_flag(ui_imgORLRGrn, LV_OBJ_FLAG_HIDDEN);
    }
    else{
 //     lv_obj_add_flag(ui_imgORLRGrn, LV_OBJ_FLAG_HIDDEN);
 //     lv_obj_clear_flag(ui_imgORLRRed, LV_OBJ_FLAG_HIDDEN);
    }

    // Right Rear
    if(message.data[5] &0x02){
//      lv_obj_add_flag(ui_imgORRRRed, LV_OBJ_FLAG_HIDDEN);
//      lv_obj_clear_flag(ui_imgORRRGrn, LV_OBJ_FLAG_HIDDEN);
    }
    else{
//      lv_obj_add_flag(ui_imgORRRGrn, LV_OBJ_FLAG_HIDDEN);
//      lv_obj_clear_flag(ui_imgORRRRed, LV_OBJ_FLAG_HIDDEN);
    }

  }
}

void CANbus_RxTask() {
  if (!canbus_driver_installed) {
    // Driver not installed
    //delay(1000);
    return;
  }
  // Check if alert happened
  uint32_t alerts_triggered;
  twai_read_alerts(&alerts_triggered, pdMS_TO_TICKS(1));
  twai_status_info_t twaistatus;
  twai_get_status_info(&twaistatus);

  // Handle alerts
  if (alerts_triggered & TWAI_ALERT_ERR_PASS) {
    Serial.println("Alert: TWAI controller has become error passive.");
  }
  if (alerts_triggered & TWAI_ALERT_BUS_ERROR) {
    Serial.println("Alert: A (Bit, Stuff, CRC, Form, ACK) error has occurred on the bus.");
    Serial.printf("Bus error count: %d\n", twaistatus.bus_error_count);
  }
  if (alerts_triggered & TWAI_ALERT_RX_QUEUE_FULL) {
    Serial.println("Alert: The RX queue is full causing a received frame to be lost.");
    Serial.printf("RX buffered: %d\t", twaistatus.msgs_to_rx);
    Serial.printf("RX missed: %d\t", twaistatus.rx_missed_count);
    Serial.printf("RX overrun %d\n", twaistatus.rx_overrun_count);
  }

  // Check if message is received
  if (alerts_triggered & TWAI_ALERT_RX_DATA) {
    // One or more messages received. Handle all.
    twai_message_t message;
    while (twai_receive(&message, 0) == ESP_OK) {
      handle_canbus_rx_message(message);
    }
  }
}