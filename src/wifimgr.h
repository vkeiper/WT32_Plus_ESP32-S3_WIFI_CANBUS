#ifndef WiFiMgr_h
#define WiFiMgr_h

#include <Arduino.h>
#include <lvgl.h>
#include <ui.h>



void updateFooterPanel(lv_color_t color, String text);
void connectWIFI();


#endif