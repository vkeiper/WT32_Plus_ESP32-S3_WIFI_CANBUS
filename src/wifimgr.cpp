#include "FS.h"
#include "SPIFFS.h"
#include "WiFi.h"
#include <ESP32Ping.h>
#include "wifiserver.h"
#include "wifimgr.h"


#define USE_WIFI_MGR (1)

#if USE_WIFI_MGR

#ifdef USE_WIFISCN_TSK
TaskHandle_t ntScanTaskHandler;
#endif

static void beginWIFISoftAPTask(void *pvParameters);
static void beginWIFITask(void *pvParameters);
TaskHandle_t ntConnectTaskHandler;
TaskHandle_t ntWifiServerTaskHandler;

const char* ssidName = "eLink2 WiFi LCD";
const char* password = "vin31NAT91";
String ssidNameStr = "eLink2 WiFi LCD";
String passwordStr = "1234";
const char* remote_host = "192.168.68.1";
const char* local_host = "vgk3";  // instead of ip address you can hit url http://el2wifilcd.local/xxxxxx where x's are the poage or resource you want to access
unsigned long timeout = 10000; // 10sec
int pingCount = 2; 


#ifdef USE_WIFISCN_TSK
void networkScanner(){
  vTaskDelay(1000);
  xTaskCreate(scanWIFITask,
                "ScanWIFITask",
                4096,
                NULL,
                1,
                &ntScanTaskHandler);
}
#endif


#ifdef USE_WIFI_DD_EVT
static void wifidd_event_handler(lv_obj_t * obj, lv_event_t event){
  
  if(event == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
        ssidName = String(buf);
        
        for (int i = 0; i < ssidName.length()-1; i++) {
          if (ssidName.substring(i, i+2) == " (") {
              ssidName = ssidName.substring(0, i);
            break;
          }
        }
        
        popupPWMsgBox();
    }
}
#endif

#ifdef USE_PING_GAUGE
static void makeGaugeIndicator(void){

    bargauge = lv_bar_create(panel_ping);
    lv_obj_set_size(bargauge, 200, 20);
    lv_obj_align(bargauge, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_bar_set_anim_time(bargauge, 1000);
    lv_bar_set_range(bargauge, 0, 1000);
    lv_bar_set_value(bargauge, 500, LV_ANIM_ON);
    
    lv_obj_t * label = lv_label_create(bargauge);
    lv_obj_align(label, bargauge, LV_ALIGN_CENTER, 0, 70);
    lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_title());

    setGaugeValue(0, 250);

}

static void setGaugeValue(float prev, float curt){

    // lv_bar_anim_t a;
    Serial.print("Bar Float Value: ");
    Serial.println(curt);
    Serial.print("Bar Int Value: ");
    Serial.println((int)curt);
    
    //if(curt <1) { curt =1;}
    static char buf[64];
    lv_snprintf(buf, sizeof(buf), "Ping %d mS", (int)curt);
    lv_obj_t * label = lv_obj_get_child(bargauge, NULL);
    lv_label_set_text(label, buf);
    lv_obj_align(label, bargauge, LV_ALIGN_CENTER, 0, 0);
 
}

static void gauge_anim(lv_obj_t * bargauge, lv_anim_value_t value){
    lv_gauge_set_value(bargauge, value, LV_ANIM_ON);

}

#endif

#ifdef USE_KBOBJ

static void makeKeyboard(){
  kb = lv_keyboard_create(lv_scr_act());
  lv_obj_set_size(kb,  LV_HOR_RES, LV_VER_RES / 2);
  lv_keyboard_set_cursor_manage(kb, true);
  
  lv_keyboard_set_textarea(kb, ta_password);
  lv_obj_set_event_cb(kb, keyboard_event_cb);
  lv_obj_move_background(kb);
}

static void keyboard_event_cb(lv_obj_t * kb, lv_event_t event){
  lv_keyboard_def_event_cb(kb, event);

  if(event == LV_EVENT_APPLY){
    lv_obj_move_background(kb);
    
  }else if(event == LV_EVENT_CANCEL){
    lv_obj_move_background(kb);
  }
}
#endif

#ifdef USE_FULLSCN_EVT
static void fullscreen_event(lv_obj_t * fc_btn, lv_event_t event){
  lv_obj_move_background(bg_top);
  lv_obj_move_background(panel_ping);
  lv_obj_move_background(bg_bottom);
  lv_obj_move_foreground(eLink_win);

}
#endif

#ifdef USE_PW_MSGBX
static void makePWMsgBox(){
  mbox_connect = lv_msgbox_create(lv_scr_act());
  static const char * btns[] ={"Connect", "Cancel", ""};
  
  ta_password = lv_textarea_create(mbox_connect);
  lv_obj_set_size(ta_password, 200, 40);
  lv_textarea_set_text(ta_password, "");


  lv_msgbox_add_btns(mbox_connect, btns);
  lv_obj_set_width(mbox_connect, 200);
  lv_obj_set_event_cb(mbox_connect, mbox_event_handler);
  lv_obj_align(mbox_connect, NULL, LV_ALIGN_CENTER, 0, -90);
  lv_obj_move_background(mbox_connect);
}

static void mbox_event_handler(lv_obj_t * obj, lv_event_t event){
    if(event == LV_EVENT_VALUE_CHANGED) {
      lv_obj_move_background(kb);
      lv_obj_move_background(mbox_connect);
      
          if(strcmp(lv_msgbox_get_active_btn_text(obj), "Connect")==0){
            password = lv_textarea_get_text(ta_password);
            password.trim();
            connectWIFI();
          }
    
    }
}

static void popupPWMsgBox(){
  if(ssidName == NULL || ssidName.length() == 0){
    return;
  }

    lv_textarea_set_text(ta_password, "honestevent368"); 
    lv_msgbox_set_text(mbox_connect, ssidName.c_str());
    lv_obj_move_foreground(mbox_connect);
    
    lv_obj_move_foreground(kb);
    lv_keyboard_set_textarea(kb, ta_password);
}
#endif

#ifdef USE_WIFISCN_TSK
/*
 * NETWORK TASKS
 */

void scanWIFITask(void *pvParameters) {  

  vTaskDelay(1000); 
  while (1) {
    updateFooterPanel(LV_PALETTE_ORANGE, "::: Searching Available WIFI :::");        
    int n = WiFi.scanNetworks();
    if (n <= 0) {
      updateFooterPanel(LV_PALETTE_RED, "Sorry no networks found!");        
    }else{
      lv_dropdown_clear_options(ddlist);  
      vTaskDelay(10);
      for (int i = 0; i < n; ++i) {
                           
        String item = WiFi.SSID(i) + " (" + WiFi.RSSI(i) +") " + ((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
        lv_dropdown_add_option(ddlist,item.c_str(),LV_DROPDOWN_POS_LAST);
        vTaskDelay(10);
      }

     updateFooterPanel(LV_PALETTE_GREEN, String(n) + " networks found!");                     
    }

    vTaskDelay(10000); 
   } 
}
#endif

void updateFooterPanel(lv_color_t color, String text){
    lv_label_set_text(ui_lblFltD, text.c_str());
    //lv_obj_set_style_text_color(ui_lblFltD, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_bg_color(bg_bottom, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,color);
}




static void beginWIFISoftAPTask(void *pvParameters) {
  bool bConn = false;

  //updateFooterPanel(lv_palette_main(LV_PALETTE_TEAL),"Connecting WIFI: ");// + ssidName);
  
  WiFi.softAP(ssidName, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // starts server and endless loops
  doWiFiServerTask(pvParameters);
}

void connectWIFI(void){
  /*
  if(ssidName == NULL || ssidName.length() <1 || password == NULL || password.length() <1){
     Serial.println("SSID Params Error");
    return;
  }
  #ifdef USE_WIFISCN_TSK
    vTaskDelete(ntScanTaskHandler);
    vTaskDelay(500);
  #endif
  */
  xTaskCreatePinnedToCore(beginWIFISoftAPTask,
                "BeginWIFISoftAPTask",
                4000,
                NULL,
                0,
                &ntConnectTaskHandler,
                1
  );  

}

static void beginWIFITask(void *pvParameters) {
  bool bConn = false;
  
  updateFooterPanel(lv_palette_main(LV_PALETTE_TEAL),"Connecting WIFI: " + ssidNameStr);

  unsigned long startingTime = millis();

  WiFi.begin(ssidNameStr.c_str(), passwordStr.c_str());
  while (WiFi.status() != WL_CONNECTED && (millis() - startingTime) < timeout)
  {
     Serial.println("WiFi Begin Failed");
    vTaskDelay(250);

  }

  if(WiFi.status() != WL_CONNECTED) {
     Serial.println("WiFi Still not connected ERROR");
    updateFooterPanel(lv_palette_main(LV_PALETTE_RED), "Please check your wifi password and try again.");
   
    #ifdef USE_WIFISCN_TSK
    vTaskDelay(2500);
    networkScanner();
    vTaskDelete(NULL);
    #endif
    
  }else{
    Serial.println("WiFi Connected");

  }
  
  updateFooterPanel(lv_palette_main(LV_PALETTE_GREEN), "WIFI is Connected! Local IP: " +  WiFi.localIP().toString());
  
  /*
  if(Ping.ping(WiFi.localIP())) {
      bConn = true;
      updateFooterPanel(lv_palette_main(LV_PALETTE_GREY), ">>> Check the network latency. Ping to " + String(remote_host));
      
      float prev = 0;
      float totalPing = 0;
      
      for(int i=0; i<pingCount; i++){
        Ping.ping(remote_host, 1);  
        float avgTime = Ping.averageTime();
      
        if(avgTime <=0){
          i--;
          continue;
        }
        
        totalPing += avgTime;
        
        #ifdef USE_PING_GAUGE
        setGaugeValue(prev, avgTime);
        lv_bar_set_value(bargauge, avgTime, LV_ANIM_ON);
        #endif

        prev = avgTime;
        vTaskDelay(500);
      }
      
      float totalAvg = totalPing / pingCount; 
      #ifdef USE_PING_GAUGE
        setGaugeValue(prev, totalAvg);
        lv_bar_set_value(bargauge, totalAvg, LV_ANIM_ON);
      #endif  
      

      updateFooterPanel(lv_color_black(), "The average delay time is " + String(totalAvg) + " ms.");
      //updateHeaderPanel(LV_PALETTE_GREEN, "Avg Delay " + String(totalAvg) + " mS ping from " + WiFi.localIP().toString()  + " to " + String(remote_host));
  
      vTaskDelay(500);    
    
    }else{
      updateFooterPanel(lv_palette_main(LV_PALETTE_RED), "This WIFI you chose is not connected. Please select one of them again.");
    }
    */

    if(bConn){
       Serial.println("Creating WiFi server task");
      xTaskCreate(doWiFiServerTask,
                "doWiFiServerTask",
                4096,
                NULL,
                0,
                &ntWifiServerTaskHandler); 
    }else{
      #ifdef USE_WIFISCN_TSK
      networkScanner();
      #endif
    }
    vTaskDelete(NULL);// delete calling task
}

#endif