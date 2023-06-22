/*********************************************************************
  * @author  Vince Keiper AstrodyneTDI
  * @version V1.0.0
  * @date    24-June-2015
  * @brief   ICCP Configuration Routines used to save data in non-volatile
  *          memory in the ICCP LCD controller we use external NOR flash.
  *			 This is not an EEPROM emmulator so this data should only be
  *          written to these slots a few times, not hundreds  of times.
  *			 That would cause data corruption. 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 AstrodyneTDI</center></h2>
  *
  *
  ******************************************************************************  
  */ 


/******************************************************************************/
/*                          include files                                     */
/******************************************************************************/
//#include "cmsis_os.h"             /* RTX kernel functions & defines      */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "iccpfuncs.h"
#include "stm32f4xx.h"              /* STM32F40x register definitions      */
#include "stm32f4xx_hal.h"          // give access to stm32f4xx_hal_flash_ex.h
#include "configuration.h"

        uint32_t FirstSector    = 0, NbOfSectors = 0, Address = 0;
        uint32_t SectorError    = 0;
__IO    uint32_t data32         = 0 , MemoryProgramStatus = 0;

/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;
static uint32_t GetSector(uint32_t Address);


/* Private define ------------------------------------------------------------ */
#define FLASH_USER_START_ADDR   ADDR_FLASH_SECTOR_7  /* Start @ of Flash area that I will store cfg data table */
#define FLASH_USER_END_ADDR     ADDR_FLASH_SECTOR_7   /* End @ of user Flash area that I will store cfg data table */

//controller cfg sectgion offset
#define CNTRLCFGOFFSET (0)  // NO TCPIP Stack on this so offset is zero. 
																				 //the last 64K is for controller cfg settings
//tcp/ip stack cfg sectgion offset
#define TCPIPCFGOFFSET 0  //The 1st 192K is for TCP/IP stack CFG storage. 


//config data
typedef enum{CFG_U8, CFG_U16, CFG_CHAR, CFG_U32, CFG_F32, CFG_PTR, CFG_END} e_cfgdatatype_t;

typedef struct 
{
		cfgslotnames_t  slotnames;
		e_cfgdatatype_t cfgtype[QTYSLOTS];      // what type of data is beng stored
		unsigned int    index;                  // qty of entries
		unsigned int    slotname[QTYSLOTS];     // entry name
		unsigned long   offset[QTYSLOTS];       // offset from section offset
		unsigned int    numbytes[QTYSLOTS];     // length in bytes of entry	
		unsigned int    block[QTYSLOTS];
		uint8_t         lclbuff[1024];
		uint32_t        stadrflash;
} s_cfgdata_t;
s_cfgdata_t s_cfgdata;


/***************************************************************************************
 * @author  Vince Keiper AstrodyneTDI
 * @version V1.0.0
 * @date    21-Sept-2015
 * @brief   CfgGetArrayData You pass in the SlotName of the data stored in NVM.
 *   		This function looks up the offset and data length that is mapped
 *		    to that SlotName macro via its respective index. This index is used to 
 *			get the offset of this data from the base of memory cfg data is saved at.
 *
 ******************************************************************************  
 */ 
void CfgGetArrayData(unsigned char *rtndata, unsigned int SlotName, unsigned char len)
{
	unsigned long startaddress;
	int i, j;
	uint32_t stadr_blk;
	
	for (i = 0; i < s_cfgdata.index; i++)
	{
        if (SlotName == s_cfgdata.slotname[i])
		{	
            startaddress    = CNTRLCFGOFFSET + s_cfgdata.offset[i];
			stadr_blk       = startaddress + FLASH_USER_START_ADDR;
			
            // loop through copy bytes from FLASH into buffer passed in
            for (j = 0; j < s_cfgdata.numbytes[i]; j++)
			{   
                //get value of data at ptr to flash to ptr to destination buffer
                *rtndata = *(__IO uint32_t *)stadr_blk;
				rtndata++;    //bump pointer to destination buffer by 1 
				stadr_blk++;  //bump pointer to source buffer in flash memory by 1 
			}
			
            break;
		}
	}		
}
/*****************************************************************************************
*
*	@author Vince Keiper
*   @brief  CfgSetArrayData, Response handler. Description: You pass in the 
*           SlotName of the data stored in NVM.  This function looks up the 
*           offset and length that is mapped to that SlotName macro via its
*           respective index. This index is used to get the offset of this
*           slot from the base of memory.  Since we are saving cfg data in
*           internal flash we are setting aside the last block of flash
*			inside the micro. We must delete the entire block before any new
*           data can be written.  Before we delete the block we must read all
*           of the data that resides in that block into an array in RAM. We then
*           update the data in the RAM array that matches the slotname we 
*           passed in.  Once the data we want changed is replaced in the local
*           array we can erase the block, then write the entire array back to
*           the block of flash.  Each block of flash is 128KB, and I will limit
*           the configuration data struct function to 1024 bytes and not put 
*           any other data in the remainder of block.
* ****************************************************************************************  
* @param    *setdata, pointer to buffer that will get written to flash
* @param    SlotName, MACRO of the mapped slot that we want to write to.
* @param    len, length of the buffer in bytes, tells the function when to stop writing.
*
* @retval none
******************************************************************************************/
void CfgSetArrayData(unsigned char *setdata, unsigned int SlotName, unsigned char len)
{
	//unsigned long startaddress;
	int i, k, j, l;
	
	uint32_t    stadr_blk;
	uint32_t    fourbytes;
	int         writedata=0;
	
	for (i = 0; i < s_cfgdata.index; i++)
	{
        if (SlotName == s_cfgdata.slotname[i])
        {	
            if (2 * len < s_cfgdata.numbytes[i])
            {
                // add spaces from last setstr char to length of slot
                // at the next char at the end of setstr upto SlotLength
                for (k = len; k < s_cfgdata.numbytes[i] + 1; k++)
                {
                    setdata[k] = 0x00; //pad with zeros						
                }
            }
					
            writedata = 1;
            break;
        }
	}			
	
	if (writedata)
	{
        // clear local buffer for population later
        memset (s_cfgdata.lclbuff, 0x00, sizeof(s_cfgdata.lclbuff));
					
        // read out 1024bytes to create local buffer
        stadr_blk = FLASH_USER_START_ADDR;// sEGMENT 7 
        while ((stadr_blk - FLASH_USER_START_ADDR) < 256) //FLASH_USER_END_ADDR + 256)//loop through 256 times copying 1024 bytes into local buffer
        {
            fourbytes = *(__IO uint32_t *)stadr_blk;

            // each address of flash holds 32 bits 4bytes
            // the configuration data buffer is 8bits 1 byte
            // so we need to push the data into the cfg buffer
            // 
            // also my cfg data buffer is only 1024 bytes so we only want to save 
            // the first 1024 bytes of the FLASH block into the local buffer
            // the rest is not used					
            if ((stadr_blk - FLASH_USER_START_ADDR)/4 < 256)
            {
                s_cfgdata.lclbuff[(stadr_blk - FLASH_USER_START_ADDR)/4+0]    = fourbytes & 0x000000FF;
                s_cfgdata.lclbuff[(stadr_blk - FLASH_USER_START_ADDR)/4+1]    = fourbytes >> 8 & 0x000000FF;
                s_cfgdata.lclbuff[(stadr_blk - FLASH_USER_START_ADDR)/4+2]    = fourbytes >> 16 & 0x000000FF;
                s_cfgdata.lclbuff[(stadr_blk - FLASH_USER_START_ADDR)/4+3]    = fourbytes >> 24 & 0x000000FF;
            }
							
            stadr_blk = stadr_blk + 4;
        }  
					
        // replace the bytes in the local buffer for the slot we are writing to 
        // we must re-write entire block to flash at once later
        l = 0;
    
        for (j = s_cfgdata.offset[i]; j < ((s_cfgdata.numbytes[i] + 1) + s_cfgdata.offset[i]); j++)
        {
            s_cfgdata.lclbuff[j] = setdata[l]; //copy passed in data to local buffer					
            l++;
        }
					
    /******************************/

        // Unlock the Flash to enable the flash control register access 
        HAL_FLASH_Unlock();
        FirstSector = GetSector(FLASH_USER_START_ADDR);
        NbOfSectors = 1;

        // Fill EraseInit struct
        EraseInitStruct.TypeErase       = FLASH_TYPEERASE_SECTORS;
        EraseInitStruct.VoltageRange    = FLASH_VOLTAGE_RANGE_3;
        EraseInitStruct.Sector          = FirstSector;
        EraseInitStruct.NbSectors       = NbOfSectors;

        __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                        FLASH_FLAG_PGAERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR |
                        FLASH_FLAG_PGSERR);
				
        if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
        { 
            Error_Handler();
        }

        // Program the Flash area byte by byte
        //	(area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
        stadr_blk = FLASH_USER_START_ADDR;
				
        for(j = 0; j < 1024; j++)
        {
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, stadr_blk, s_cfgdata.lclbuff[j]) == HAL_OK)
            {
                stadr_blk = stadr_blk + 1;
            }

            else
            { 
                Error_Handler();
            }
        }

        // Lock the Flash to disable the flash control register access (recommended
        //	 to protect the FLASH memory against possible unwanted write operations)
        HAL_FLASH_Lock(); 			
	}		
}
/******************************************************************************/

//static float fourbytes2float(unsigned char *ptr2fourbytearray);

//float fourbytes2float(unsigned char *ptr2fourbytearray)
//{
//	float f_data;
//	unsigned char *ptr2float = (unsigned char*)&f_data;
//	int i;

//	for (i = 0; i < 4; i++)
//	{
//        *(ptr2float++) = *ptr2fourbytearray++;
//	}
//	
//    return f_data;
//}
void CfgGetAsciiData(char *rtndata, unsigned int SlotName, unsigned char len)
{
	//unsigned long startaddress;
	int i;
	
	for (i = 0; i < s_cfgdata.index; i++)
	{
        if (SlotName == s_cfgdata.slotname[i])
        {	
            //startaddress = CNTRLCFGOFFSET + s_cfgdata.offset[i];

//            if(BSP_NOR_ReadData(startaddress, (uint16_t*)rtndata, (uint32_t)s_cfgdata.numbytes[i]) != NOR_STATUS_OK)
//            {
//                while(1);
//            }
//					
//            else
//            {
//                i = s_cfgdata.index + 1; //make sure we exit for loop
//            }
//					
//            return;
        }
	}		
}
void CfgSetAsciiData(char *setstr, unsigned int SlotName, unsigned char len)
{
	//unsigned long startaddress;
	int i, k;
		
	for (i = 0; i <s_cfgdata.index; i++)
	{
        if (SlotName == s_cfgdata.slotname[i])
        {	
            if (len < s_cfgdata.numbytes[i])
            {
                //add spaces from last setstr char to length of slot
                //at the next char at the end of setstr upto SlotLength
                for (k = len; k < s_cfgdata.numbytes[i] + 1; k++)
                {
                        setstr[k] = 0x32; //pad with ascii spaces						
                }
            }
				
            //	startaddress = CNTRLCFGOFFSET + s_cfgdata.offset[i];
//            if (BSP_NOR_WriteData(startaddress, (uint16_t*)setstr, (uint32_t)s_cfgdata.numbytes[i]) != NOR_STATUS_OK)
//            {
//                while(1);
//            }
//					
//            else
//            {
//                return;
//            }
        }
    }
}
/******************************************************************************/


//void CfgGetEepromByteData(unsigned char *rtndata, unsigned int SlotName)
//{
//	unsigned long startaddress;
//	unsigned int i;
//	for (i = 0; i < LastConfigIndex; i++)
//	{
//		if (SlotName == ConfigEepromName[i])
//		{	
//			startaddress = CNTRLCFGOFFSET + ConfigEepromOffset[i];
//			XEEReadArray(startaddress, rtndata, ConfigEepromLength[i]);
//			i = LastConfigIndex + 1; //make sure we exit for loop
//			return;
//		}
//	}		
//}
///**************************************************************************************/

//void CfgSetEepromByteData(unsigned char *setdata,unsigned int SlotName)
//{
//	unsigned long startaddress;
//	int i;
//
//   
//  //loop until we find the slot that matches the name we passed in
//	for (i = 0; i < LastConfigIndex; i++)
//	{
//		if (SlotName == ConfigEepromName[i])
//		{
//			startaddress = CNTRLCFGOFFSET + ConfigEepromOffset[i];
//			XEEBeginWrite(startaddress);
//			XEEWriteArray(setdata, ConfigEepromLength[i]);
//      }
//	}


//}
/**************************************************************************************/
//            			 
//							Function: CreatConfigMemMap
//                                                                                    */
/**************************************************************************************/
//
// Description: Populates a struct array called ConfigEeprom. This sets up what the
//				Eeprom data slots are named, what memory offset they get written to and how many 
//              bytes the data in that slot is. This builds the memory map.
//
// Inputs: None
//
// Outputs: Struct Array elements up to 65535 indexes.
//			LastConfigIndex... value equal to the last array index
//							   +1. To provide stop mechanism in the for loop	
//
// Dependencies: NOR flash must be initialized and redy to read & writtn to.	
//				 																				
/***************************************************************************************/

void CreateConfigMemMap(void)
{
	unsigned int i;
	s_cfgdata.stadrflash    = FLASH_USER_START_ADDR;     

//0
	i = 0;
	s_cfgdata.slotname[i]   = MODEL;
	s_cfgdata.offset[i]     = 0;//offset from base of NVM assigned
	s_cfgdata.numbytes[i]   = 16; // 16 characters in length max
	s_cfgdata.cfgtype[i]  	= CFG_CHAR;
	s_cfgdata.block[i]      = (s_cfgdata.offset[i] / BLKSZ) + 1; // determine what block of NOR in which this data resides
//1
	i++;
	s_cfgdata.slotname[i]   = SERIAL;
	s_cfgdata.offset[i]     = s_cfgdata.offset[i - 1] + s_cfgdata.numbytes[i - 1];  //offset
	s_cfgdata.numbytes[i]   = 16; //length
	s_cfgdata.cfgtype[i]  	= CFG_CHAR;
	s_cfgdata.block[i]      = (s_cfgdata.offset[i] / BLKSZ) + 1; // determine what block of NOR in which this data resides
//2
	i++;
	s_cfgdata.slotname[i]   = VERSION;
	s_cfgdata.offset[i]     = s_cfgdata.offset[i - 1] + s_cfgdata.numbytes[i - 1];  //offset
	s_cfgdata.numbytes[i]   = 16; //length
	s_cfgdata.cfgtype[i]  	= CFG_CHAR;
	s_cfgdata.block[i]      = (s_cfgdata.offset[i] / BLKSZ) + 1; // determine what block of NOR in which this data resides

//3
	i++;
    s_cfgdata.slotname[i]   = PRFL0;
	s_cfgdata.offset[i]     = s_cfgdata.offset[i - 1] + s_cfgdata.numbytes[i - 1];  //offset
	s_cfgdata.numbytes[i]   = 104; // length, was 32 NMM 01/27/2016
	s_cfgdata.cfgtype[i]  	= CFG_PTR;
	s_cfgdata.block[i]      = (s_cfgdata.offset[i] / BLKSZ) + 1; // determine what block of NOR in which this data resides

//4
	i++;
    s_cfgdata.slotname[i]   = CTLTYPE;  // ESPM,LIQUA BLADE,;
	s_cfgdata.offset[i]     = s_cfgdata.offset[i - 1] + s_cfgdata.numbytes[i - 1];  //offset
	s_cfgdata.numbytes[i]   = 1; //length
	s_cfgdata.cfgtype[i]  	= CFG_U8;
	s_cfgdata.block[i]      = (s_cfgdata.offset[i] / BLKSZ) + 1; // determine what block of NOR in which this data resides
	
//12
	i++;
    s_cfgdata.slotname[i]   = CALDATA; // Calibration data array hold, vrdb offset & gain, irdb offset & gain, refelec0,1,2,3 offset & gain, Isensor 0,1,2,3 offset & gain
	s_cfgdata.offset[i]     = s_cfgdata.offset[i - 1] + s_cfgdata.numbytes[i - 1];  //offset
	s_cfgdata.numbytes[i]   = 120; //length  allowing for 8 bytes per ADC & DAC cal factor, allow for 15 channels of cal (4bytes for offset 4 bytes for gain)
	s_cfgdata.cfgtype[i]  	= CFG_PTR;
	s_cfgdata.block[i]      = (s_cfgdata.offset[i] / BLKSZ) + 1; // determine what block of NOR in which this data resides
	
	s_cfgdata.index         = i + 1;  //this should equal the last index +1 in this structurearray
}
///////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/******************************************************************************/
/* Author: Vince Keiper         CfgGetCalData	    			 	     	  */
// Description:
//		   Called by Config functions that returns 32 bit double(float & double are the same in C30 & XC16 compilers)
//			back to the calling functions. We also want to save the result in a 4 byte array to tranfers out over CAN
//			for CalData read functions. This is done by passing in a pointer to an array that will store the result to  	
// Inputs: Defined SlotNames from Configuration.h tell CfgGet function what
//			address to go get the data from.
/******************************************************************************/

//float CfgGetCalData(unsigned int CalSlotName, unsigned char *bytedataresult)
//{
//    float result;
//    unsigned char *ptr = (unsigned char *)&result;
//    unsigned char i;
//    unsigned char bytedata[4];
//		
//    CfgGetEepromByteData(bytedata,CalSlotName);
//    // since data was saved from float32 we had to
//    // break it down by accessing bytes wth pointer
//    // and saving byte by byte. So we need to do the
//    // inverse to retrieve the float by building it
//    // byte by byte.
//    // loop through saving byte to array eah time
//    for (i = 0; i < 4; i++)
//    {
//        *(ptr++)              = bytedata[i];
//        *(bytedataresult++)   = bytedata[i];

//    }
//    
//    return result;
//}
///////////////////////////////////////////////////////////////////////////
/******************************************************************************/
/******************************************************************************/
/* Author: Vince Keiper         CfgSetCalData	    			 	     	  */
// Description:
//		   Called by Config functions passes in float32. 32 bit double
//		   (float & double are the same in C30 & XC16 compilers)
//		   CAN functions fro frontpanel can call this to set the caldata
//		   I also wrote a function to program default config data if it detects
//		   no cal blocks have been programmed yet, which could be dangerous and
//		   because gain could be very high or very low. So we need to control this
//		   and check on every boot up.
//		
// Inputs: Defined SlotNames from Configuration.h tell CfgSet function what
//			address to set the data.
/******************************************************************************/

//void CfgSetCalData(float fdata, unsigned int CalSlotName)
//{
//	unsigned char *ptr = (unsigned char *)&fdata;
//  unsigned char i;
//	unsigned char bytedata[4];

//    for (i = 0; i < 4; i++)
//    {
//	    bytedata[i] = *(ptr++);
//	  }
//	
//	CfgSetEepromByteData(bytedata, CalSlotName);	
//}


/**
*  Init Variables that are stored in CFG section of 256K EEPROM - 
*/
unsigned char InitConfigVars(void)
{
    

//	unsigned char *ptrmodel = (unsigned char * )&modelnum;  //point to global model number variable
//	unsigned char CriticalConfigDataValid = TRUE;
	
//	gbl_f32_vsetgain = CfgGetCalData(VSETGAIN, gbl_vsetgain_array);
//	//check to see if gain was returned
//	if(!IsValidGain(gbl_f32_vsetgain, VOLTSTYPE)){CriticalConfigDataValid = FALSE;}

//	gbl_f32_vsetoffset = CfgGetCalData(VSETOFFSET, gbl_vsetoffset_array);
//	//check to see if gain was returned
//	if(!IsValidOffset(gbl_f32_vsetoffset, VOLTSTYPE)){CriticalConfigDataValid = FALSE;}

//	gbl_f32_vrdbgain = CfgGetCalData(VREADGAIN, gbl_vrdbgain_array);
//	//check to see if gain was returned
//	if(!IsValidGain(gbl_f32_vrdbgain, VOLTSTYPE)){CriticalConfigDataValid = FALSE;}

//	gbl_f32_vrdboffset = CfgGetCalData(VREADOFFSET, gbl_vrdboffset_array);
//	//check to see if gain was returned
//	if(!IsValidOffset(gbl_f32_vrdboffset, VOLTSTYPE)){CriticalConfigDataValid = FALSE;}

//	gbl_f32_isetgain = CfgGetCalData(ISETGAIN, gbl_isetgain_array);
//	//check to see if gain was returned
//	if(!IsValidGain(gbl_f32_isetgain,AMPSTYPE)){CriticalConfigDataValid = FALSE;}

//	gbl_f32_isetoffset = CfgGetCalData(ISETOFFSET, gbl_isetoffset_array);
//	//check to see if gain was returned
//	if(!IsValidOffset(gbl_f32_isetoffset, AMPSTYPE)){CriticalConfigDataValid = FALSE;}

//	gbl_f32_irdbgain = CfgGetCalData(IREADGAIN, gbl_irdbgain_array);
//	//check to see if gain was returned
//	if(!IsValidGain(gbl_f32_irdbgain, AMPSTYPE)){CriticalConfigDataValid = FALSE;}

//	gbl_f32_irdboffset = CfgGetCalData(IREADOFFSET, gbl_irdboffset_array);
//	//check to see if gain was returned
//	if(!IsValidOffset(gbl_f32_irdboffset, AMPSTYPE)){CriticalConfigDataValid = FALSE;}
//	
//	gbl_f32_psetgain = CfgGetCalData(PSETGAIN, gbl_psetgain_array);
//	//check to see if gain was returned
//	if(!IsValidGain(gbl_f32_psetgain, WATTSTYPE)){CriticalConfigDataValid = FALSE;}
//	
//	gbl_f32_psetoffset = CfgGetCalData(PSETOFFSET, gbl_psetoffset_array);
//	//check to see if gain was returned
//	if(!IsValidOffset(gbl_f32_psetoffset, WATTSTYPE)){CriticalConfigDataValid = FALSE;}

//	//	example how to read ascii from EE
//	CfgGetEepromAsciiData(serialnumber, SERIAL);
//	CfgGetEepromAsciiData(builddate, BLDDTE);
//	CfgGetEepromAsciiData(codever, CODEVER);

//        CfgGetEepromByteData(ptrmodel, MODELNUM);//Populate model number var

//        if (modelnum == 0x00 || modelnum == 0xFF ){modelnum = MODELSPS5930;}

//        if (modelnum == MODELSPS5979)
//        {
//            s_Controldcdc.otpval      = 75;// Dragon over temp threshold is 75
//            s_Controldcdc.RatedAmps   = 12000;
//            s_Controldcdc.RatedVolts  = 6300;
//            s_Controldcdc.RatedWatts  = 5500;
//        }
//        else if (modelnum == MODELSPS5930)
//        {
//            s_Controldcdc.otpval      = 85;// Std LB  over temp threshold is 85
//            s_Controldcdc.RatedAmps   = 12000;
//            s_Controldcdc.RatedVolts  = 6300;
//            s_Controldcdc.RatedWatts  = 5500;
//        }

//        // if fixed output mode setting is outsidce of normal bounds set it to fully adjustable until
//        // they get re-programmed by interface of ProgramCriticalConfigVars() is run
//        gbl_f32_defwatts = CfgGetCalData(DEFWATTS, gbl_defwatts_array);
//        if (gbl_f32_defwatts > 5500 || gbl_f32_defwatts < 0){gbl_f32_defwatts = 0;}

//        gbl_f32_defamps = CfgGetCalData(DEFAMPS,gbl_defamps_array);
//        if (gbl_f32_defamps > 130 || gbl_f32_defamps < 0){gbl_f32_defamps = 0;}

//        gbl_f32_defvolts = CfgGetCalData(DEFVOLTS, gbl_defvolts_array);
//        if(gbl_f32_defvolts > 63 || gbl_f32_defvolts < 0){gbl_f32_defvolts = 0;}

//	return CriticalConfigDataValid; 
return 1;
}


//unsigned char IsValidGain(double gain, unsigned char type)
//{
//	unsigned char return_val = FALSE;
//	float lowlimit = 0, hilimit = 0;
//	if (type == VOLTSTYPE || type == AMPSTYPE)
//	{
//		lowlimit = -3.00;
//		hilimit = 3.00;	
//	}
//	
//  else if (type == WATTSTYPE)
//	{
//		lowlimit    = -10.00;
//		hilimit     = 10.00;	
//	}
//	
//  if (gain > lowlimit && gain < hilimit  && gain ! = 0){return_val = TRUE;}
//	return return_val;	
//}
//unsigned char IsValidOffset(double offset, unsigned char type)
//{
//	unsigned char return_val = FALSE;
//	float lowlimit = 0, hilimit = 0;
//	if (type == VOLTSTYPE || type == AMPSTYPE)
//	{
//		lowlimit    = -10.00;
//		hilimit     = 10.00;	
//	}
//	else if (type == WATTSTYPE)
//	{
//		lowlimit    = -30.00;
//		hilimit     = 30.00;	
//	}
//	if (offset > lowlimit && offset < hilimit){return_val = TRUE;}
//	return 1;//return_val;	
//}
////////////////////////////////////////////////////////////////////////////////
//
// This is executed if the unit is brand new and the EEPROM is all 0xFF
// after this the unit will be a fully adjustable SPS5930 Std LB with all
// gains & offsets pgm'd to 1&0's respecively.
//
////////////////////////////////////////////////////////////////////////////////
unsigned char ProgramCriticalConfigData(void)
{
//	float gain,offset;

//	gain    = 1.00;
//	offset  = 0;
//	CfgSetCalData(gain, VSETGAIN);
//	CfgSetCalData(offset, VSETOFFSET);
//	CfgSetCalData(gain, ISETGAIN);
//	CfgSetCalData(offset, ISETOFFSET);
//	CfgSetCalData(gain, VREADGAIN);
//	CfgSetCalData(offset, VREADOFFSET);
//	CfgSetCalData(gain, IREADGAIN);
//	CfgSetCalData(offset, IREADOFFSET);
//	CfgSetCalData(gain, PSETGAIN);
//	CfgSetCalData(offset, PSETOFFSET);
//        
//        //Default PSU to fully adjustable
//        CfgSetCalData(0,DEFWATTS);
//	CfgSetCalData(0, DEFAMPS);
//	CfgSetCalData(0, DEFVOLTS);

//        unsigned char modelset_ary[1];
//        modelset_ary[0] = MODELSPS5930;
//	//Set Model to Std Liqua Blade
//        CfgSetEepromByteData((unsigned char *)modelset_ary, MODELNUM);

        return 1;
}
/////////////////////////////////////////////////////////////////////////////////
/////
//			EEFactReset sets 64K from the offset passed in. The cfg data
//			for the controller is stored in the 1st 64K. If a 256K EE is used
//			the offet passed in should be set to 192000, because the 1st
//			192K is used for TCPIP stack info
//
/////
//////////////////////////////////////////////////////////////////////////////////
unsigned char EEFactReset(void)
{
//	unsigned long startaddress;
//	unsigned int i;
//	BYTE zerostr[16];
//	
//	for(i = 0; i< 17 ; i++){zerostr[i] = 0xff;} //zero out string
//			
//	for(i = 0; i< (4000-1); i++)    //16bytes per loop 16*4000= 64000 to equal 
//	{
//		startaddress = CNTRLCFGOFFSET + (i*16); //startaddress 0 + i*16 
//		XEEBeginWrite(startaddress);
//		XEEWriteArray(zerostr, sizeof(zerostr));	
//	}		
//	
//	
return 1;
}
/////////////////////////////////////////////////////////////////////////////////
/////
//			Clears a block of EEPROM the index of the array is passed in and 
//      this function starts clearing there and stops at index address plus 
//			length array. 
//
/////
//////////////////////////////////////////////////////////////////////////////////
void ClearBlock(unsigned char index)
{
//	unsigned int i;
//	BYTE zerostr[32];
//			
//	for(i = 0; i< ConfigEepromLength[i+1]; i++){zerostr[i] = 0x00;}//zero out string
//	
//	XEEBeginWrite(ConfigEepromOffset[index]);
//	XEEWriteArray(zerostr, ConfigEepromLength[index]);	
//	sprintf(CommTxBuff,"Index Number: %i Block Zeroed\r\n", ConfigEepromName[index]);
//	RS232Write(CommTxBuff);
	
}


//	BSP_NOR_ReturnToReadMode();
//    
//  if(BSP_NOR_Erase_Block(WRITE_READ_ADDR) != NOR_STATUS_OK)
//  {
//    ubEraseStatus++; 
//  }
//  
//  /*##-4- NOR memory read/write access  ######################################*/   
//  /* Fill the buffer to write */
//  Fill_Buffer(nor_aTxBuffer, BUFFER_SIZE, 0xC20F);   
//  
//  /* Write data to the NOR memory */
//  if (BSP_NOR_WriteData(WRITE_READ_ADDR, nor_aTxBuffer, BUFFER_SIZE) != NOR_STATUS_OK)
//  {
//    ubWriteStatus++; 
//  }
//  
//  /* Read back data from the NOR memory */
//  if(BSP_NOR_ReadData(WRITE_READ_ADDR, nor_aRxBuffer, BUFFER_SIZE) != NOR_STATUS_OK)
//  {
//    ubReadStatus++; 
//  }


/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
	
static uint32_t GetSector(uint32_t Address)
{
    uint32_t sector = 0;
  
    if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
    {
        sector = FLASH_SECTOR_0;  
    }
  
    else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
    {
        sector = FLASH_SECTOR_1;  
    }
  
    else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
    {
        sector = FLASH_SECTOR_2;  
    }
  
    else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
    {
        sector = FLASH_SECTOR_3;  
    }
      
    else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
    {
        sector = FLASH_SECTOR_4;  
    }
      
    else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
    {
        sector = FLASH_SECTOR_5;  
    }
      
    else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
    {
        sector = FLASH_SECTOR_6;  
    }
  
    else /* if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7)) */
    {
        sector = FLASH_SECTOR_7;  
    }

  return sector;
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  while(1)
  {
  }
}
/*=============================================================================
 *  FINIS
 *===========================================================================*/

