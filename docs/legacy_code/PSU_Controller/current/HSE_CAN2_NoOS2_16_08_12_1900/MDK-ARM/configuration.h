/*********************************************************************
* @author  Vince Keiper AstrodyneTDI
* @version V1.0.0
* @date    24-June-2015
* @brief   ICCP Configuration Routines used to save data in non-volatile memory
* 				 in the ICCP LCD controller we use external NOR flash.
*					 This is not an EEPROM emmulator so this data should only be written 
*          to these slots a few times, not hundreds  of times. That would cause 
*					 data corruption. 
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2015 AstrodyneTDI</center></h2>
*
*
******************************************************************************  
*/ 


#ifndef __CONFIG_H
#define __CONFIG_H


// MAX QTY NVM Slots
#define QTYSLOTS 12


/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
////#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
////#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
////#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
////#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */


//Definitions for NVM Memory Slot Names
#define     MODEL           0
#define     SERIAL          1
#define     VERSION         2
#define     IPADR           3
#define     MACADR          4
#define     NETMSK     	    5
#define     GWY       	    6
#define     PRFL0     	    7
#define     PRFL1           8
#define     PRFL2           9
#define     PRFL3           10
#define     LOCATION        11
#define     CTLTYPE         12
#define     CALDATA         13

typedef enum { CFG_MODEL, CFG_SERIAL, CFG_VERSION,CFG_IPADR,CFG_MACADR,CFG_NETMSK,CFG_GWY,CFG_PRFL0,CFG_PRFL1,
               CFG_PRFL2, CFG_PRFL3, CFG_LOCATION, CFG_TCHCAL} cfgslotnames_t;

//define Data Types for EEPROM
#define FLOATDATA   0b0001	// 32 bits double or float
#define CHARDATA    0b0010	// Char Arrays up to 32bits
#define UINT8DATA	0b0100	// 8 bit unsigned or signed char
#define UINT16DATA	0b0101	// 16 bit unsigned or signed int
#define PTRDATA	    0b0110	// pointer data


//NOR flash IC defines
#define NORSTARTADR         0x00
#define BLKSZ (uint32_t)    128000
#define NORQTYBLOCKS        256
#define BUFFER_SIZE         ((uint32_t)1024)
#define WRITE_READ_ADDR     ((uint32_t)0x0000)

//define Data Types for EEPROM
#define VOLTSTYPE   0x01	// volts
#define AMPSTYPE	0x02	// amps
#define WATTSTYPE	0x03	// power 


void CfgGetArrayData(unsigned char *rtndata, unsigned int SlotName, unsigned char len);
void CfgSetArrayData(unsigned char *setdata, unsigned int SlotName, unsigned char len);
void CfgGetAsciiData(char *rtndata, unsigned int SlotName, unsigned char len);
void CfgSetAsciiData(char *setstr, unsigned int SlotName,unsigned char len);

//void CfgGetEepromByteData(unsigned char *rtndata, unsigned int SlotName);
//void CfgSetEepromByteData(unsigned char *setdata, unsigned int SlotName);
void CreateConfigMemMap(void);

//float CfgGetCalData(unsigned int CalSlotName, unsigned char *bytedataresult);
//void CfgSetCalData(float fdata, unsigned int CalSlotName);
unsigned char InitConfigVars(void);
//unsigned char IsValidGain(double gain, unsigned char type);
//unsigned char IsValidOffset(double offset, unsigned char type);
unsigned char ProgramCriticalConfigData(void);
unsigned char EEFactReset(void);
void ClearBlockEE(unsigned char index);	

static void Error_Handler(void);

#endif
/*=============================================================================
 *  FINIS
 *===========================================================================*/

