#include "FS.h"
#include "SPIFFS.h"
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <lvgl.h>
#include <jsonProcessor.h>
#include "wifiserver.h"
#include "wifimgr.h"
#include <ESPmDNS.h>
#include <Update.h>
#include "otawebpages.h"

#define USE_WIFI_SERVER (1)

#if USE_WIFI_SERVER
void handleOTAUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
void handlespiffsOTAUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
AsyncWebServer server(80);
 
const char* PARAM_MESSAGE1 = "PSU_CHAN";//psu channel, aka PSU CANbus node addressbeing selected
int msgCnt =0;
int apimsgCnt =0;  
String command = "";
int led_status = 255;

//void notFound(AsyncWebServerRequest *request) {
//    request->send(404, "text/plain", "Not found");
//}

/*
JsonObject getStatusAsJson() {
  JsonObject obj = doc.to<JsonObject>();
  
  return obj;
}
*/
/*
void sendStaResp(AsyncWebServerRequest *request) {
  AsyncResponseStream *response = request->beginResponseStream("application/json");
  //getStatusAsJson().printTo(*response);
  serializeJson(doc, *response);
  response->addHeader("Access-Control-Allow-Origin", "*");
  request->send(response);
}
*/
/*
void sendError(AsyncWebServerRequest *request, int httpCode, String error) {
  AsyncResponseStream *response = request->beginResponseStream("application/json");
  //DynamicJsonBuffer jsonBuffer;
  //JsonObject &obj = jsonBuffer.createObject();
  //obj["error"] = error;
  //response->setCode(httpCode);
  //request->send(response);
}
*/

/*
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}
*/

void doSimpleWiFiServerTask(void *pvParameters){
  
  server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello World");
  });
  server.begin();
  Serial.println("Wifi Server Task Running");

  while(1){
       vTaskDelay(5);

       if(Serial.available()){
        command = Serial.readStringUntil('\n');

        Serial.printf("Command received %s \n", command);

        if(command.startsWith("refelc")){
            
        } else if(command.equals("led=off")){
            
            led_status = 0;
        } else if(command.equals("ledstatus")){
            
            led_status = 3;
        } else{
            Serial.println("Invalid command");
        }
    }
  }
}

void doWiFiServerTask(void *pvParameters){
      
  Serial.println("Wifi Server Task Starting");
  updateFooterPanel(lv_palette_main(LV_PALETTE_GREEN), "WiFi Server Running");        
  Serial.println(WiFi.localIP());

/*use mdns for host name resolution*/
  if (!MDNS.begin(local_host)) { //http://el2wifilcd.local
    Serial.println("Error setting up MDNS responder!");
    delay(1000);
  }else{
    Serial.println("mDNS responder started");
  }

  if(!SPIFFS.begin()){
     Serial.println("An Error has occurred while mounting SPIFFS");
     //return;
  }else{
   File root = SPIFFS.open("/");
 
   File file = root.openNextFile();
   if(file){
     while(file){
     
        Serial.print("FILE: ");
        Serial.println(file.name());
     
        file = root.openNextFile();
      }
   }
  }
  
  doCreateJsonDoc();

//<link rel="stylesheet" href="/css/bootstrap-theme.min.css" />  
//<link rel="stylesheet" href="/css/bootstrap.min.css" /> 
//<script src="/js/jquery-3.4.1.min.js"></script>
//<script src="/js/popper.min1.14.7.js"></script> 
//<script src="/js/bootstrap-4.3.1.min.js"></script>
    
  server.on("/css/bootstrap-theme.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/bootstrap-theme.min.css", "text/css");
  });

  server.on("/css/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/bootstrap.min.css", "text/css");
  });
 
  server.on("/js/jquery-3.4.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/js/jquery-3.4.1.min.js", "text/javascript");
  });
  
  server.on("/js/popper.min1.14.7.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/js/popper.min1.14.7.js", "text/javascript");
  });
 
  server.on("/js/bootstrap-4.3.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/js/bootstrap-4.3.1.min.js", "text/javascript");
  });

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
 
  server.on("/index2.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index2.html", "text/html");
  });
 
  server.on("/inc/navbar.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/inc/navbar.html", "text/html");
  });

  server.on("/mode.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/mode.html", "text/html");
  });
   
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/favicon.ico", "text/html");
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      String msgResp;
      msgCnt++;
      request->send(200, "text/plain", "Hello, world");
      msgResp = request->client()->remoteIP().toString() +" Cnt " + String(msgCnt);
      Serial.println("/ from " + msgResp + " Cnt " + String(msgCnt));
      updateFooterPanel(lv_palette_main(LV_PALETTE_GREEN), " Msgs From: " + msgResp); 
  });

  // Send a GET request to <IP>/get?message=<message>
  //VGK, so http://192.168.1.58/get?ELINK_TEST=567 will receive '567' as value
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
      String msgResp;
      msgCnt++;
      if (request->hasParam(PARAM_MESSAGE1)) {
          msgResp = request->getParam(PARAM_MESSAGE1)->value();
      } else {
          msgResp = "No message sent";
      }
      request->send(200, "text/plain", " Got the, GET: with PARAM " + msgResp);

      Serial.println("GET /get " + msgResp);
      updateFooterPanel(lv_palette_main(LV_PALETTE_GREEN), " GET PARAM " + String(PARAM_MESSAGE1) +" " + msgResp);
  });

  server.on("/printIp", HTTP_GET, [](AsyncWebServerRequest *request){ 
    String msgResp;
    msgCnt++;
    msgResp = request->client()->remoteIP().toString() + " Cnt: " +String(msgCnt);
    request->send(200, "text/plain", "Your IP: " + msgResp);
 
    Serial.print("Received request from client with IP: ");
    Serial.println("GET printIp from " + msgResp + " Cnt " + String(msgCnt));
    updateFooterPanel(lv_palette_main(LV_PALETTE_GREEN), " printIp msgs from: " + msgResp); 
  });

  // AsyncCallbackJsonWebHandler* jsonhdlr = new AsyncCallbackJsonWebHandler("/api/set", [](AsyncWebServerRequest *request, JsonVariant &json) {
  //   JsonObject jsonObj = json.as<JsonObject>();

    
  //   if (obj.containsKey("opmode")) {
  //     int opmode = obj.get<int>("opmode_dmd");
  //     Serial.print("SET OPMODE to  ");
  //     Serial.println(opmode);
  //     //
  //     // 0x00 = Auto mode, closed loop
  //     // 0x01 = Manual mode, fixed demand current\voltage\power
  //     // 0x02 = Self Test mode, selft test requested from HMI\web page
  //     // 0x04 = Over-Protect mode
      
  //     apiSetOpmode(opmode);
      
  //   }

  //   if (obj.containsKey("dmdAmps")) {
  //     float dmdAmps = obj.get<float>("dmdAmps");
  //     Serial.print("SET Current Demand  ");
  //     Serial.println(dmdAmps);
  //     setDmdAmps(dmdAmps);
  //   }


  //   sendStatusResp(request);
    
  // });
  // server.addHandler(jsonhdlr);


  server.on("/api/getsta/", HTTP_GET, [] (AsyncWebServerRequest *request) {
      AsyncResponseStream *response = request->beginResponseStream("application/json");
      //obj.get<int>("opmode_dmd");
      //update voltage in json with random number to show webpage\LCD is dynamic
      double vlimlo = doc["panel_1"]["limits"][0][0];// vlim low limit
      double vlimhi = doc["panel_1"]["limits"][0][1];// vlim high limit
      doc["panel_1"]["values"][0] = random(vlimlo*100,vlimhi*100)/100;
      //update current in json with random number to show webpage\LCD is dynamic
      double Ilimlo = doc["panel_1"]["limits"][1][0];// Ilim low limit
      double Ilimhi = doc["panel_1"]["limits"][1][1];// Ilim high limit
      doc["panel_1"]["values"][1] = random(Ilimlo,Ilimhi);

      //updatePanel2(LV_COLOR_GREEN, (const char*)doc["panel_2"]["values"][1]);
      //updatePanel3(LV_COLOR_GREEN, (const char*)doc["panel_3"]["values"][1]);
     
      if(doc["panel_4"]["values"] == "PASSING"){
          doc["panel_4"]["values"] = "OVER PROTECTION 1";
          //updatePanel4(LV_COLOR_RED, (const char*)doc["panel_4"]["values"]);
      }else{
          doc["panel_4"]["values"] = "PASSING";
          //updatePanel4(LV_COLOR_GREEN, (const char*)doc["panel_4"]["values"]);

      }


      
      serializeJson(doc, *response);
      response->addHeader("Access-Control-Allow-Origin", "*");
      request->send(response);
  });
  
  /* Start OTA Booloader req/resp gandlers*/
  /*return index page which is stored in otaloginIndex */
  server.on("/otaindex.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", otaloginIndex);
  });
  
  /*return the otaserverIndex page which is stored in 'otaserverIndex'*/
  server.on("/otasrvrIndex.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", otaserverIndex);
  });

  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request){ 
    request->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, handleOTAUpload);
  /* End OTA Booloader req/resp gandlers*/
  
  /* Start SPIFFS OTA Booloader req/resp handlers*/
  /*return index page which is stored in otaloginIndex */
  server.on("/otaspiffsindex.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", otaspiffsloginIndex);
  });
  
  /*return the otaspiffsserverindex page which is stored in 'otaspiffsserverIndex' var*/
  server.on("/otaspiffssrvrindex.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", otaspiffsserverIndex);
  });

  /*handling uploading spiffs bin image file */
  server.on("/spiffsupdate", HTTP_POST, [](AsyncWebServerRequest *request){ 
    request->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, handlespiffsOTAUpload);
  /* End SPIFFS OTA req/resp handlers*/

  //server.onNotFound(notFound);
  //server.onNotFound((AsyncWebServerRequest *request){
  //    request->send(200, "text/plain", "Page Not Found");
  //});

   
  server.begin();


  vTaskDelay(500);// give a little breathing room before task forever loop takes over
  
   
  while(1){
       vTaskDelay(5);

       if(Serial.available()){
        command = Serial.readStringUntil('\n');

        Serial.printf("Command received %s \n", command);

        if(command.startsWith("refelc")){
            
        } else if(command.equals("led=off")){
            
            led_status = 0;
        } else if(command.equals("ledstatus")){
            
            led_status = 3;
        } else{
            Serial.println("Invalid command");
        }
    }
  }
    
}

// handles uploads to the filserver
void handleOTAUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  const char* FILESIZE_HEADER{"FileSize"};
  static int filesize, fileleft;

  filesize = request->header(FILESIZE_HEADER).toInt();
  // make sure authenticated before allowing upload
  //if (checkUserWebAuth(request)) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
    Serial.println(logmessage);

    if (!index) {
      logmessage = "Upload Start: " + String(filename);
      // open the file on first call and store the file handle in the request object
      Serial.println(logmessage);
     
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    }

    if (len) {
      if(index != 0 && filesize != 0){
        fileleft = index/filesize;
      }
      /* flashing firmware to ESP*/
      if (Update.write(data, len) != len) {
        Update.printError(Serial);
      }
      logmessage = "Writing file: " + String(filename) + " index=" + String(index) + " len=" + String(len) + " , Written: " + String(fileleft);
      Serial.println(logmessage);
    }

    if (final) {
      logmessage = "Upload Complete: " + String(filename) + ",size: " + String(index + len);
      Serial.println(logmessage);
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", filesize );
      } else {
        Update.printError(Serial);
      }
      request->redirect("/");
    }
  //} else {
  //  Serial.println("Auth: Failed");
  //  return request->requestAuthentication();
  //}
}

// handles uploads to the filserver
void handlespiffsOTAUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  const char* FILESIZE_HEADER{"FileSize"};
  static int filesize, fileleft;

  filesize = request->header(FILESIZE_HEADER).toInt();
  // make sure authenticated before allowing upload
  //if (checkUserWebAuth(request)) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
    Serial.println(logmessage);

    if (!index) {
      logmessage = "Upload Start: " + String(filename);
      // open the file on first call and store the file handle in the request object
      Serial.println(logmessage);
     
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    }

    if (len) {
      if(index != 0 && filesize != 0){
        fileleft = index/filesize;
      }
      /* flashing firmware to ESP*/
      if (Update.write(data, len) != len) {
        Update.printError(Serial);
      }
      logmessage = "Writing file: " + String(filename) + " index=" + String(index) + " len=" + String(len) + " , Written: " + String(fileleft);
      Serial.println(logmessage);
    }

    if (final) {
      logmessage = "Upload Complete: " + String(filename) + ",size: " + String(index + len);
      Serial.println(logmessage);
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", filesize );
      } else {
        Update.printError(Serial);
      }
      request->redirect("/");
    }
  //} else {
  //  Serial.println("Auth: Failed");
  //  return request->requestAuthentication();
  //}
}
#endif