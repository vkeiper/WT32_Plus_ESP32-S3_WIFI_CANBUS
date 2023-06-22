/**
  ******************************************************************************
  * @file    leds_res.c
  * @author  Vince Keiper
  * @version V1.0.0
  * @date    20-Sept-2015   
  * @brief   Green on\leds Red coming soon
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 AstrodyneTDI </center></h2>
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include "GUI.h"


/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifndef GUI_CONST_STORAGE
  #define GUI_CONST_STORAGE const
#endif


// USER START (Optionally insert additional static data)
/*********************************************************************
*
*       _acImage_0, "C:\Users\keipe_v.TRANSDEV.COM\Pictures\leds\iccp_off_grn_20x20.bmp", ID_IMAGE_0_IMAGE_2
*/
static const U8 _acImg_grnoff[1254] = {
  0x42, 0x4D, 0xE6, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xF2, 0xF0, 0xE2, 0xE7, 0xE3,
  0xE1, 0xE7, 0xE2, 0xE5, 0xEA, 0xE6, 0xF9, 0xFA, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xF3, 0xF4, 0xF3, 0xAB, 0xB2, 0xA9, 0x7C, 0x88, 0x7B, 0x72, 0x82, 0x72, 0x7B, 0x8F, 0x7D, 0x7B, 0x94, 0x7F, 0x78, 0x8D, 0x7A, 0x74, 0x80, 0x73, 0x88, 0x90, 0x86, 0xC9, 0xCD, 0xC9, 0xFC, 0xFD, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFE, 0xC7, 0xC7, 0xC6, 0x72, 0x73, 0x70, 0x87, 0x93, 0x81, 0x87, 0x9A, 0x81, 0x68, 0x80, 0x65, 0x4A, 0x68, 0x4A, 0x43, 0x65, 0x44, 0x51, 0x70, 0x50,
  0x7A, 0x8A, 0x72, 0x90, 0x9E, 0x89, 0x7D, 0x87, 0x79, 0x80, 0x87, 0x7E, 0xE6, 0xE8, 0xE5, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFE, 0xAC, 0xB2, 0xAB, 0x77, 0x7E, 0x70, 0x94, 0x9B, 0x8A,
  0x4E, 0x65, 0x46, 0x1A, 0x40, 0x17, 0x05, 0x3A, 0x08, 0x01, 0x3F, 0x08, 0x02, 0x44, 0x0B, 0x02, 0x3C, 0x06, 0x0B, 0x37, 0x08, 0x27, 0x4A, 0x20, 0x66, 0x7E, 0x5E, 0x94, 0xA0, 0x8D, 0x70, 0x75, 0x6D, 0xD8, 0xDA, 0xD7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xB7, 0xBE, 0xB8, 0x74, 0x83, 0x73, 0x82, 0x97, 0x7C, 0x2B, 0x43, 0x21, 0x0B, 0x3D, 0x0C, 0x04, 0x4C, 0x0E, 0x02, 0x5F, 0x15, 0x02, 0x6B, 0x1B, 0x05, 0x6F, 0x1F, 0x04, 0x66, 0x1A, 0x08, 0x5C, 0x18, 0x06, 0x4C, 0x0E,
  0x0B, 0x3B, 0x0B, 0x40, 0x5B, 0x3B, 0x98, 0xA4, 0x93, 0x6E, 0x76, 0x6B, 0xE8, 0xEA, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE9, 0xEA, 0xEB, 0x6B, 0x74, 0x6C, 0x87, 0x9F, 0x86, 0x20, 0x48, 0x1F, 0x08, 0x3E, 0x08, 0x03, 0x58, 0x0F, 0x02, 0x70, 0x1B,
  0x06, 0x88, 0x2A, 0x06, 0x93, 0x2F, 0x0A, 0x95, 0x34, 0x09, 0x8E, 0x32, 0x07, 0x84, 0x2E, 0x01, 0x6D, 0x1D, 0x05, 0x56, 0x13, 0x0A, 0x3D, 0x0B, 0x46, 0x61, 0x3F, 0x92, 0x9E, 0x8C, 0x86, 0x8D, 0x84, 0xFE, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0x9B, 0x9E, 0x9D,
  0x8F, 0x9B, 0x8D, 0x3F, 0x60, 0x3E, 0x03, 0x3B, 0x04, 0x04, 0x57, 0x10, 0x01, 0x72, 0x1B, 0x05, 0x8A, 0x29, 0x02, 0x99, 0x30, 0x00, 0x9A, 0x2F, 0x00, 0x98, 0x31, 0x01, 0x96, 0x33, 0x02, 0x97, 0x37, 0x03, 0x8C, 0x2F, 0x04, 0x73, 0x20, 0x05, 0x52, 0x10,
  0x0A, 0x3A, 0x09, 0x70, 0x85, 0x6A, 0x73, 0x7D, 0x6F, 0xD3, 0xD8, 0xD4, 0xFC, 0xFD, 0xFC, 0x63, 0x72, 0x66, 0x85, 0x9A, 0x82, 0x12, 0x3A, 0x10, 0x02, 0x4A, 0x0B, 0x01, 0x62, 0x16, 0x06, 0x7B, 0x23, 0x0A, 0x8D, 0x2D, 0x04, 0x96, 0x30, 0x00, 0x96, 0x2F,
  0x02, 0x92, 0x31, 0x04, 0x8F, 0x32, 0x02, 0x92, 0x31, 0x06, 0x93, 0x32, 0x05, 0x82, 0x29, 0x02, 0x63, 0x18, 0x02, 0x46, 0x09, 0x33, 0x4F, 0x2E, 0x8B, 0x99, 0x87, 0x91, 0x9A, 0x92, 0xD8, 0xE0, 0xDA, 0x68, 0x82, 0x6C, 0x5B, 0x78, 0x59, 0x09, 0x38, 0x09,
  0x02, 0x4E, 0x0D, 0x01, 0x67, 0x19, 0x07, 0x78, 0x23, 0x09, 0x83, 0x29, 0x03, 0x87, 0x29, 0x00, 0x86, 0x29, 0x02, 0x82, 0x2B, 0x05, 0x82, 0x2B, 0x06, 0x87, 0x2B, 0x03, 0x88, 0x2A, 0x01, 0x80, 0x26, 0x02, 0x69, 0x1C, 0x01, 0x50, 0x0F, 0x17, 0x3C, 0x15,
  0x83, 0x97, 0x7F, 0x6E, 0x7A, 0x6F, 0xC0, 0xC7, 0xC2, 0x77, 0x8F, 0x7B, 0x42, 0x63, 0x43, 0x05, 0x3A, 0x08, 0x00, 0x53, 0x0D, 0x00, 0x6C, 0x19, 0x02, 0x7A, 0x22, 0x02, 0x7C, 0x24, 0x02, 0x7C, 0x24, 0x02, 0x7C, 0x24, 0x02, 0x7C, 0x24, 0x03, 0x7C, 0x25,
  0x04, 0x7B, 0x25, 0x02, 0x7C, 0x23, 0x03, 0x78, 0x22, 0x03, 0x66, 0x1A, 0x01, 0x51, 0x0E, 0x0A, 0x3C, 0x0C, 0x6C, 0x8E, 0x6C, 0x59, 0x6D, 0x5C, 0xBD, 0xC4, 0xBF, 0x79, 0x8F, 0x7D, 0x3F, 0x5D, 0x3F, 0x06, 0x39, 0x09, 0x01, 0x52, 0x0D, 0x01, 0x6B, 0x19,
  0x00, 0x74, 0x1F, 0x00, 0x74, 0x1F, 0x00, 0x74, 0x1F, 0x00, 0x74, 0x1F, 0x00, 0x74, 0x1F, 0x01, 0x74, 0x1F, 0x02, 0x73, 0x1F, 0x00, 0x75, 0x1E, 0x02, 0x73, 0x1E, 0x04, 0x64, 0x18, 0x01, 0x4D, 0x0D, 0x0B, 0x38, 0x0C, 0x69, 0x87, 0x69, 0x58, 0x6B, 0x5B,
  0xCB, 0xD0, 0xCC, 0x75, 0x86, 0x77, 0x4F, 0x67, 0x4C, 0x0B, 0x38, 0x0A, 0x03, 0x4E, 0x0D, 0x02, 0x68, 0x19, 0x01, 0x6E, 0x1B, 0x01, 0x6E, 0x1B, 0x00, 0x6E, 0x1B, 0x01, 0x6E, 0x1B, 0x00, 0x6E, 0x1B, 0x01, 0x6E, 0x1B, 0x02, 0x6C, 0x1B, 0x00, 0x6D, 0x19,
  0x02, 0x6C, 0x1A, 0x06, 0x5F, 0x16, 0x04, 0x49, 0x0B, 0x12, 0x38, 0x10, 0x79, 0x90, 0x77, 0x62, 0x71, 0x63, 0xED, 0xF0, 0xED, 0x65, 0x72, 0x65, 0x70, 0x84, 0x6B, 0x0E, 0x37, 0x0C, 0x03, 0x4A, 0x0B, 0x06, 0x67, 0x1A, 0x00, 0x67, 0x17, 0x00, 0x66, 0x16,
  0x00, 0x66, 0x15, 0x03, 0x68, 0x17, 0x03, 0x68, 0x17, 0x03, 0x67, 0x18, 0x02, 0x67, 0x18, 0x00, 0x67, 0x16, 0x03, 0x66, 0x18, 0x0B, 0x5D, 0x17, 0x02, 0x40, 0x06, 0x24, 0x43, 0x22, 0x88, 0x9A, 0x87, 0x7E, 0x88, 0x7F, 0xFC, 0xFD, 0xFC, 0x7C, 0x85, 0x7A,
  0x8C, 0x9D, 0x87, 0x25, 0x4A, 0x20, 0x01, 0x41, 0x05, 0x03, 0x60, 0x16, 0x03, 0x67, 0x19, 0x0A, 0x6E, 0x20, 0x14, 0x78, 0x2A, 0x13, 0x77, 0x29, 0x13, 0x77, 0x2A, 0x15, 0x77, 0x2B, 0x13, 0x74, 0x28, 0x05, 0x69, 0x1B, 0x05, 0x64, 0x17, 0x07, 0x54, 0x10,
  0x03, 0x3B, 0x04, 0x4E, 0x67, 0x4A, 0x83, 0x90, 0x81, 0xB5, 0xBC, 0xB6, 0xFF, 0xFF, 0xFF, 0xC2, 0xC6, 0xC0, 0x84, 0x8B, 0x7C, 0x68, 0x7D, 0x62, 0x0D, 0x3D, 0x0E, 0x05, 0x50, 0x11, 0x0F, 0x67, 0x20, 0x34, 0x8B, 0x44, 0x3E, 0x92, 0x4E, 0x3F, 0x8F, 0x51,
  0x42, 0x8E, 0x54, 0x44, 0x8F, 0x59, 0x3D, 0x8C, 0x56, 0x1D, 0x73, 0x34, 0x11, 0x62, 0x1F, 0x04, 0x40, 0x07, 0x1F, 0x43, 0x1A, 0x90, 0x9B, 0x8B, 0x74, 0x78, 0x73, 0xF1, 0xF2, 0xF1, 0xFF, 0xFF, 0xFF, 0xFC, 0xFD, 0xFC, 0x89, 0x8B, 0x86, 0x9E, 0xA7, 0x98,
  0x4D, 0x6B, 0x4B, 0x08, 0x38, 0x0A, 0x1D, 0x5E, 0x2B, 0x5D, 0x9F, 0x6D, 0x7C, 0xBA, 0x8D, 0x80, 0xBA, 0x92, 0x7C, 0xB3, 0x8F, 0x7E, 0xB5, 0x90, 0x75, 0xB3, 0x86, 0x35, 0x79, 0x43, 0x0B, 0x4A, 0x12, 0x15, 0x3F, 0x15, 0x75, 0x8A, 0x6F, 0x96, 0x9B, 0x92,
  0xB9, 0xB9, 0xB8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEC, 0xEC, 0xEC, 0x83, 0x84, 0x7F, 0xA3, 0xAE, 0x9E, 0x5F, 0x76, 0x5A, 0x19, 0x3E, 0x1B, 0x2A, 0x53, 0x30, 0x7A, 0xA1, 0x82, 0xAD, 0xD3, 0xB7, 0xB7, 0xDB, 0xC1, 0x9D, 0xC2, 0xA5,
  0x52, 0x7B, 0x54, 0x13, 0x3D, 0x14, 0x24, 0x4A, 0x24, 0x77, 0x8D, 0x75, 0xA0, 0xA8, 0x9D, 0xA5, 0xA6, 0xA2, 0xFE, 0xFE, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE9, 0xE8, 0xE8, 0x96, 0x97, 0x92, 0xAB, 0xB1, 0xA3,
  0x8E, 0xA0, 0x85, 0x56, 0x6B, 0x4D, 0x33, 0x49, 0x2C, 0x20, 0x37, 0x1A, 0x1B, 0x33, 0x17, 0x22, 0x38, 0x1D, 0x3D, 0x51, 0x36, 0x68, 0x7B, 0x63, 0x9C, 0xAB, 0x99, 0x9E, 0xA5, 0x9C, 0xB4, 0xB3, 0xB3, 0xFC, 0xFC, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFA, 0xFA, 0xFA, 0xB8, 0xBB, 0xB5, 0xA4, 0xA9, 0x9C, 0xC1, 0xC6, 0xB7, 0xC3, 0xCC, 0xBB, 0xBB, 0xC7, 0xB6, 0xB7, 0xC6, 0xB4, 0xBA, 0xC6, 0xB6, 0xC4, 0xC9, 0xBF, 0xB8, 0xB9, 0xB3,
  0xA5, 0xA5, 0xA2, 0xD5, 0xD5, 0xD4, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0xF8, 0xF8, 0xDB, 0xDB, 0xDA,
  0xCC, 0xCD, 0xCC, 0xD3, 0xD6, 0xD5, 0xD5, 0xDA, 0xD7, 0xCE, 0xD2, 0xD0, 0xCF, 0xCE, 0xCD, 0xE1, 0xDF, 0xE0, 0xFE, 0xFD, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

/*********************************************************************
*
*       _acImage_1, "C:\Users\keipe_v.TRANSDEV.COM\Pictures\leds\iccp_on_grn_20x20.bmp", ID_IMAGE_1_IMAGE_0
*        Green ON Led 20x20
*/
static const U8 _acImg_grnon[1254] = {
  0x42, 0x4D, 0xE6, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFD, 0xFB, 0xD4, 0xD8, 0xD0, 0x99, 0xA4, 0x99, 0x76, 0x8D, 0x7C, 0x6C, 0x86, 0x73,
  0x6E, 0x85, 0x72, 0x74, 0x88, 0x77, 0x90, 0x9F, 0x92, 0xC3, 0xD0, 0xC6, 0xF8, 0xFA, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFE,
  0xD1, 0xD9, 0xD2, 0x69, 0x86, 0x70, 0x52, 0x8F, 0x69, 0x6F, 0xBB, 0x91, 0x74, 0xCD, 0x9F, 0x7C, 0xD7, 0xA8, 0x86, 0xDB, 0xAC, 0x83, 0xD6, 0xA6, 0x7A, 0xC4, 0x99, 0x61, 0x98, 0x76, 0x62, 0x81, 0x6B, 0xC8, 0xCB, 0xC5, 0xFE, 0xFC, 0xFD, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xFC, 0xFA, 0xAB, 0xB4, 0xAB, 0x66, 0x78, 0x66, 0x86, 0xA4, 0x86, 0x63, 0x9A, 0x68, 0x2C, 0x73, 0x3B, 0x0B, 0x59, 0x1E, 0x08, 0x54, 0x1A, 0x0B, 0x52, 0x18, 0x0D, 0x59, 0x1C,
  0x1F, 0x71, 0x33, 0x54, 0x97, 0x64, 0x7E, 0xAB, 0x8A, 0x66, 0x7F, 0x6F, 0x94, 0xA2, 0x9E, 0xF6, 0xF9, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFE, 0xAA, 0xB1, 0xAD, 0x74, 0x7C, 0x6F, 0x87, 0x9C, 0x80, 0x38, 0x62, 0x39,
  0x0C, 0x4A, 0x0D, 0x06, 0x59, 0x0E, 0x02, 0x6B, 0x1E, 0x02, 0x7A, 0x30, 0x06, 0x7D, 0x2D, 0x07, 0x71, 0x22, 0x06, 0x60, 0x17, 0x06, 0x4A, 0x07, 0x2D, 0x5D, 0x28, 0x7A, 0x97, 0x79, 0x74, 0x86, 0x7B, 0x95, 0xA1, 0x9B, 0xFC, 0xFD, 0xFC, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xCE, 0xD4, 0xD2, 0x69, 0x78, 0x6E, 0x93, 0xA7, 0x8F, 0x2C, 0x54, 0x2B, 0x04, 0x48, 0x11, 0x00, 0x6D, 0x24, 0x01, 0x9A, 0x41, 0x0B, 0xB8, 0x5F, 0x15, 0xCA, 0x75, 0x0A, 0xD0, 0x71, 0x08, 0xBC, 0x63, 0x0E, 0xA1, 0x53, 0x05, 0x7D, 0x31,
  0x03, 0x53, 0x14, 0x23, 0x4A, 0x20, 0x88, 0x95, 0x7E, 0x74, 0x80, 0x75, 0xB8, 0xBC, 0xB9, 0xFF, 0xFF, 0xFF, 0xFA, 0xFB, 0xFB, 0x67, 0x75, 0x70, 0x91, 0xAA, 0x95, 0x37, 0x5C, 0x36, 0x05, 0x49, 0x0F, 0x04, 0x72, 0x26, 0x01, 0xAA, 0x49, 0x10, 0xDD, 0x80,
  0x49, 0xF9, 0xBF, 0x77, 0xFB, 0xE2, 0x6E, 0xFC, 0xE4, 0x4B, 0xF8, 0xC5, 0x28, 0xE3, 0x96, 0x11, 0xBC, 0x68, 0x06, 0x8B, 0x3C, 0x04, 0x52, 0x15, 0x2C, 0x53, 0x27, 0x90, 0xA2, 0x8E, 0x6C, 0x74, 0x6B, 0xF1, 0xF2, 0xF1, 0xC7, 0xCB, 0xCB, 0x6F, 0x83, 0x75,
  0x69, 0x8C, 0x6C, 0x0B, 0x43, 0x0E, 0x03, 0x63, 0x1D, 0x07, 0x95, 0x3F, 0x06, 0xCA, 0x65, 0x16, 0xF4, 0x92, 0x3D, 0xFF, 0xB9, 0x57, 0xFB, 0xC6, 0x48, 0xF3, 0xB4, 0x39, 0xF6, 0xAE, 0x39, 0xFD, 0xB1, 0x21, 0xED, 0x90, 0x07, 0xB9, 0x5D, 0x01, 0x7E, 0x31,
  0x03, 0x4B, 0x11, 0x55, 0x75, 0x56, 0x80, 0x91, 0x7E, 0xB2, 0xB8, 0xB0, 0x90, 0x9A, 0x93, 0x89, 0xA4, 0x8D, 0x32, 0x60, 0x34, 0x01, 0x48, 0x0A, 0x05, 0x79, 0x2E, 0x04, 0xA7, 0x50, 0x0A, 0xD4, 0x6F, 0x11, 0xEB, 0x86, 0x0F, 0xF2, 0x98, 0x0B, 0xF1, 0x99,
  0x09, 0xF3, 0x88, 0x0E, 0xEC, 0x88, 0x0D, 0xEB, 0x8D, 0x09, 0xEC, 0x85, 0x07, 0xD6, 0x71, 0x05, 0x98, 0x45, 0x03, 0x5A, 0x1B, 0x28, 0x53, 0x29, 0x85, 0xA1, 0x82, 0x77, 0x8A, 0x75, 0x6C, 0x7B, 0x6C, 0x81, 0x9E, 0x82, 0x16, 0x4A, 0x1B, 0x00, 0x51, 0x11,
  0x01, 0x80, 0x33, 0x04, 0xAF, 0x57, 0x09, 0xD3, 0x6F, 0x0D, 0xDB, 0x7C, 0x07, 0xDD, 0x88, 0x02, 0xDC, 0x8C, 0x05, 0xDE, 0x87, 0x0A, 0xDD, 0x82, 0x06, 0xDE, 0x80, 0x03, 0xDA, 0x75, 0x02, 0xC8, 0x65, 0x09, 0xA1, 0x4E, 0x08, 0x68, 0x26, 0x11, 0x45, 0x13,
  0x78, 0x99, 0x78, 0x5A, 0x71, 0x5B, 0x5E, 0x6E, 0x5A, 0x7B, 0x98, 0x7A, 0x0E, 0x44, 0x17, 0x00, 0x5A, 0x1B, 0x00, 0x80, 0x30, 0x11, 0xB6, 0x5D, 0x10, 0xC9, 0x6D, 0x0F, 0xCA, 0x6E, 0x0E, 0xCB, 0x6E, 0x0B, 0xCB, 0x6E, 0x0B, 0xCB, 0x6E, 0x0E, 0xCA, 0x70,
  0x10, 0xC9, 0x6F, 0x0F, 0xCA, 0x6B, 0x07, 0xBF, 0x61, 0x01, 0x97, 0x41, 0x03, 0x6F, 0x28, 0x0A, 0x44, 0x0E, 0x6A, 0x88, 0x6A, 0x5F, 0x73, 0x63, 0x5E, 0x6D, 0x58, 0x7C, 0x98, 0x77, 0x10, 0x43, 0x13, 0x01, 0x59, 0x16, 0x07, 0x84, 0x30, 0x1C, 0xB2, 0x56,
  0x15, 0xB2, 0x54, 0x15, 0xB4, 0x55, 0x13, 0xB4, 0x56, 0x10, 0xB3, 0x54, 0x10, 0xB3, 0x55, 0x12, 0xB4, 0x56, 0x13, 0xB3, 0x56, 0x10, 0xB3, 0x54, 0x10, 0xB6, 0x55, 0x02, 0x8C, 0x35, 0x03, 0x69, 0x1F, 0x0D, 0x44, 0x0D, 0x6C, 0x88, 0x68, 0x62, 0x74, 0x62,
  0x6C, 0x79, 0x68, 0x83, 0x9B, 0x7E, 0x1C, 0x48, 0x1A, 0x01, 0x4F, 0x0D, 0x13, 0x86, 0x31, 0x1B, 0x9F, 0x43, 0x1E, 0xA2, 0x45, 0x1E, 0xA3, 0x45, 0x1C, 0xA2, 0x45, 0x1A, 0xA2, 0x44, 0x19, 0xA3, 0x44, 0x19, 0xA3, 0x45, 0x1B, 0xA5, 0x44, 0x18, 0xA4, 0x43,
  0x17, 0xA4, 0x44, 0x0E, 0x8B, 0x34, 0x04, 0x5D, 0x13, 0x11, 0x44, 0x0E, 0x7A, 0x92, 0x74, 0x5D, 0x6B, 0x5B, 0x88, 0x93, 0x88, 0x8A, 0x9C, 0x86, 0x33, 0x58, 0x30, 0x02, 0x45, 0x08, 0x1C, 0x80, 0x2F, 0x20, 0x97, 0x3D, 0x1F, 0x96, 0x3C, 0x20, 0x99, 0x3E,
  0x22, 0x9C, 0x41, 0x23, 0x9F, 0x43, 0x21, 0x9F, 0x44, 0x1F, 0x9E, 0x41, 0x20, 0x9C, 0x3C, 0x1F, 0x99, 0x3A, 0x1E, 0x98, 0x3C, 0x19, 0x82, 0x32, 0x03, 0x4F, 0x0C, 0x25, 0x4F, 0x21, 0x89, 0x9B, 0x84, 0x75, 0x7C, 0x74, 0xBF, 0xC5, 0xC1, 0x76, 0x84, 0x76,
  0x62, 0x81, 0x60, 0x07, 0x40, 0x09, 0x12, 0x6B, 0x20, 0x2C, 0x9A, 0x46, 0x28, 0x9D, 0x47, 0x2E, 0xA4, 0x4E, 0x30, 0xA9, 0x52, 0x2F, 0xAA, 0x52, 0x2E, 0xA9, 0x53, 0x2E, 0xAB, 0x51, 0x2F, 0xA8, 0x4A, 0x2E, 0xA0, 0x44, 0x2E, 0x9B, 0x45, 0x14, 0x6F, 0x26,
  0x04, 0x42, 0x07, 0x4A, 0x6F, 0x47, 0x87, 0x93, 0x85, 0xA6, 0xA8, 0xA8, 0xF8, 0xF9, 0xF8, 0x6F, 0x77, 0x6B, 0x92, 0xA6, 0x8C, 0x2C, 0x55, 0x2B, 0x07, 0x4A, 0x0C, 0x40, 0x9D, 0x54, 0x42, 0xA9, 0x5D, 0x52, 0xB9, 0x6F, 0x56, 0xBC, 0x75, 0x51, 0xB8, 0x73,
  0x51, 0xB8, 0x73, 0x55, 0xB7, 0x75, 0x57, 0xB9, 0x73, 0x49, 0xAC, 0x5E, 0x45, 0xA1, 0x53, 0x0A, 0x4D, 0x0F, 0x1E, 0x49, 0x19, 0x8C, 0x9D, 0x86, 0x75, 0x7A, 0x71, 0xE9, 0xE9, 0xE8, 0xFF, 0xFF, 0xFF, 0xB8, 0xBD, 0xB6, 0x84, 0x8D, 0x7F, 0x7E, 0x97, 0x79,
  0x1D, 0x4C, 0x1F, 0x1A, 0x58, 0x21, 0x72, 0xB7, 0x81, 0x8A, 0xCF, 0x9A, 0x8B, 0xCF, 0x9D, 0x8C, 0xD0, 0xA0, 0x8D, 0xCF, 0xA2, 0x8F, 0xD2, 0xA2, 0x88, 0xD0, 0x99, 0x6C, 0xBA, 0x79, 0x1B, 0x5D, 0x1F, 0x16, 0x43, 0x13, 0x72, 0x8B, 0x69, 0x9A, 0xA1, 0x94,
  0xA9, 0xAB, 0xA5, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xFD, 0xFD, 0x9F, 0xA1, 0x9C, 0x9A, 0xA4, 0x95, 0x7E, 0x95, 0x7A, 0x2E, 0x52, 0x2D, 0x20, 0x4B, 0x24, 0x73, 0x9F, 0x77, 0xAC, 0xD8, 0xB1, 0xC1, 0xED, 0xC7, 0xC1, 0xEC, 0xC8, 0xAE, 0xD9, 0xB0,
  0x72, 0xA2, 0x71, 0x1D, 0x4E, 0x1D, 0x23, 0x4D, 0x20, 0x73, 0x8B, 0x6C, 0xA9, 0xB2, 0xA0, 0x92, 0x94, 0x8D, 0xF9, 0xF9, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xF9, 0xF9, 0xA8, 0xA9, 0xA5, 0x9E, 0xA4, 0x98, 0x9B, 0xAC, 0x95,
  0x5A, 0x7D, 0x59, 0x30, 0x54, 0x2F, 0x26, 0x4A, 0x24, 0x2B, 0x4F, 0x29, 0x2A, 0x4E, 0x28, 0x27, 0x44, 0x20, 0x35, 0x4E, 0x2B, 0x5B, 0x72, 0x52, 0x8E, 0xA0, 0x87, 0xA6, 0xAD, 0x9F, 0xA0, 0x9F, 0x9A, 0xF2, 0xF2, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFB, 0xFC, 0xC1, 0xC1, 0xBE, 0x9A, 0xA1, 0x97, 0xA7, 0xBD, 0xA9, 0xA7, 0xBC, 0xA8, 0x9A, 0xB1, 0x9A, 0x8F, 0xA6, 0x8D, 0x8E, 0xA6, 0x8B, 0x9D, 0xAD, 0x97, 0xAA, 0xB5, 0xA5, 0xB4, 0xBC, 0xAF,
  0xA3, 0xA8, 0xA0, 0xB9, 0xBB, 0xB7, 0xF8, 0xF8, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0xF4, 0xF3, 0xCF, 0xD0, 0xD1, 0xC2, 0xC0, 0xC5,
  0xC0, 0xBF, 0xC1, 0xCA, 0xCB, 0xC8, 0xCA, 0xCA, 0xC6, 0xBB, 0xBF, 0xBA, 0xBD, 0xC2, 0xBE, 0xCA, 0xCC, 0xCA, 0xEA, 0xEB, 0xEB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};



/* Private functions ---------------------------------------------------------*/

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/