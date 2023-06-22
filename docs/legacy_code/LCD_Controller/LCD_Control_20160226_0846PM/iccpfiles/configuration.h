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


//Definitions for NVM Memeory Slot Names
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
#define     TCHCAL          12
#define     CFGHOST         13
#define     QTYSLOTS        14  // README!!! always update this with the amount of slots +1, if you add one put it before this and update this value+1



typedef enum { CFG_MODEL, CFG_SERIAL, CFG_VERSION,CFG_IPADR,CFG_MACADR,CFG_NETMSK,CFG_GWY,CFG_PRFL0,CFG_PRFL1,
								CFG_PRFL2,CFG_PRFL3,CFG_LOCATION,CFG_TCHCAL}cfgslotnames_t;

//define Data Types for EEPROM
#define FLOATDATA		0b0001	// 32 bits double or float
#define CHARDATA     0b0010	// Char Arrays up to 32bits
#define UINT8DATA		0b0100	// 8 bit unsigned or signed char
#define UINT16DATA	0b0101	// 16 bit unsigned or signed int
#define PTRDATA	    0b0110	// pointer data


//NOR flash IC defines
#define NORSTARTADR 0x00
#define NORBLKSZ (uint32_t)128000
#define NORQTYBLOCKS 256
#define BUFFER_SIZE         ((uint32_t)256)
#define WRITE_READ_ADDR     ((uint32_t)0x0000)

//define Data Types for EEPROM
#define VOLTSTYPE		0x01	// volts
#define AMPSTYPE		0x02	// amps
#define WATTSTYPE		0x03	// power 


void CfgGetArrayData(unsigned char *rtndata, unsigned int SlotName, unsigned char len);
void CfgSetArrayData(unsigned char *setdata, unsigned int SlotName, unsigned char len);
void CfgGetAsciiData(char *rtndata, unsigned int SlotName, unsigned char len);
void CfgSetAsciiData(char *setstr, unsigned int SlotName,unsigned char len);

//void CfgGetEepromByteData(unsigned char *rtndata, unsigned int SlotName);
//void CfgSetEepromByteData(unsigned char *setdata, unsigned int SlotName);
void LoadCfgMemMap2Ram(void);

//float CfgGetCalData(unsigned int CalSlotName, unsigned char *bytedataresult);
//void CfgSetCalData(float fdata, unsigned int CalSlotName);
unsigned char InitConfigVars(void);
//unsigned char IsValidGain(double gain, unsigned char type);
//unsigned char IsValidOffset(double offset, unsigned char type);
unsigned char ProgramCriticalConfigData(void);
unsigned char EEFactReset(void);
void ClearBlockEE(unsigned char index);	



#endif



