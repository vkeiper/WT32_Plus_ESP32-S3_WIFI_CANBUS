/*******************************************************************************
* FileName:        	can.c
* Processor:       	any
* Complier:        	MPLAB C30 v2.01.00 or higher
* Company:         	AstrodyneTDI
* Author:           Vince Keiper
*******************************************************************************/

/******************************************************************************/
/*                          include files                                     */
/******************************************************************************/
#include "iccp_canfuncs.h"
#include "iccpfuncs.h"
#include "CAN_Ex1.h"
#include "string.h"
#include "stdio.h"

uint8_t b_CANTXFlag;
uint8_t canBuff[8], datalength, dummycmd[8]; 
uint32_t dummyid;

 uint32_t DCADR;
 uint32_t PSUADR;
 
 /* CAN Message Buffer Configuration */

uint32_t CanFilter1;
uint32_t CanFilter2;
uint32_t CanFilterMask;
#define CANCMDCOUNT     3
#define CANMAXFUNCTION	64
#define CANBUFFCOUNTER	255
uint16_t CANSyntax[CANMAXFUNCTION];

void (*FuncCANCmdArray[CANMAXFUNCTION])(uint8_t *cmd, uint32_t id,uint8_t dlc);

static uint8_t Bufftail = 0;
static uint8_t Buffhead = 0;
static uint8_t Buffcount = 0;

static CANmsgbuff Canbuff[CANBUFFCOUNTER];
static CANmsgbuff dummyread;


void CanApiEnqueue(uint32_t id, uint8_t *canbuffer,uint8_t dlc)
{
	int8_t i;
	
	// dump the message if the ring buffer is full
	if(Buffcount >=CANBUFFCOUNTER)
	{
				for ( i = 0; i < 8; i++ ) 
				{
						dummyread.data[i] = canbuffer[i];	
				}
	}
	else
	{
			Buffhead++;
			if(Buffhead == CANBUFFCOUNTER){Buffhead = 0;}
			
			// put message into the buffer if there is not data in that slot
			if(Canbuff[Buffhead].flag == 0)
			{
					Canbuff[Buffhead].id = id; 
					Canbuff[Buffhead].flag = 1;// vk, set data available flag
					Canbuff[Buffhead].dlc = dlc;
					for ( i = 0; i < 8; i++ ) 
					{
							Canbuff[Buffhead].data[i] = canbuffer[i];	
					}
			}			
			Buffcount++;
	}
	
}

unsigned char Dequeue(int *buffindex)
{
	if(Buffcount ==0){return 0;}
	else
	{
			Bufftail++;
			if(Bufftail ==CANBUFFCOUNTER){Bufftail = 0;}
			*buffindex = Bufftail;	// pop the message out of the buffer//
			Buffcount --;
			return 1;
	}

	
}
/******************************************************************************/



/******************************************************************************/
/*                          CANInit       							 		  */
/* 6/25/14 vk, CHANGED PSUADR from 0x0Y00 to 0x000Y   to allow for 64 Blades */
/******************************************************************************/
void IccpCanApiInit()
{
   // these are set by GPIO in the LB DCDC but 
   #define SLOT1 1
	 #define SLOT2 1
	
	//DCADR can be 1,2 or 3
	DCADR = 0x00;	
	if(SLOT1){DCADR |= 0x01;}  
	if(SLOT2){DCADR |= 0x02;}


	//0x0XY00FFF  to 0x00XXYFFF
	PSUADR = 0x11;// removed * 0x100;//vk, changed CANId structure 6/25/14 to allow for 64 blades, 
							  //    moved DCDCId from Bits 20-23 to bits 12-15 to get the BladeId into 8 bits 
						    //    16-23. This allows for up to 255 Blades but we will only use 64 as max qty.

	CANLoadSyntax();


	//  6/25/14 Vince Keiper added notes for TI apps eng.
	// The CANFilter and Mask only look at the CANID section, the 8 bytes of data are not included 
	//  in the filter and mask scheme. Any data will pass.
	//  CANFilter setting a bit in filter register means in the CANID section of the received packet 
	//  that bit must be a '1' to pass filter.
	//  A clearied bit in the filter register means in the CANID section of the received packet 
	//  that bit must be a '0' to pass filter.
	//  The CANFilterMask gives you the ability bypass the filter. For instance setting the bit i
	//  in the CANFilter register will include that bit in the filter comparison, 
	//  clearing the bit '0' is a "Dont Care" and this bit is excluded from the filter comparison
	
	//  So for example if slot id = 1 ( which is determined by a DIP switch setting on GPIO )
	//  we set the CANFilter1 to be 0x00001fff
	//  And the Mask is 0x1ffff000 and is applied to both filters 1&2. We can set multiple filters, i think up to 4 filters.
	//  We only use 2. One filter is setup to only allow messages that are for the this specific SLOT_ID or DCADR.
	//  Filter2 is used to allow a special command that is a broadcast to all addresses. Explained better below.
	
	//  Anyway, look at Filer "1" below only CANID's that are received with a "1" in bit 0 of nibble3 will pass the filter.
	//  IF DCADR = "1" if DCADR = "2" bit 0 of nibble3 must be a 2 to pass filter. 
	//  This is how we filter out  messages that are not for this address. So we do not interrupt the processor for CAN messages that
	//  are for another address. This is all done in hardware in the ECAN module of the micro.
	
	//  Filter2 is setup to allow broadcasts to all SLOTID's (DCADR's) to accept any function ID. This is done by sending 
	//  a zero in bit 0 of nibble 3. So irregardless of the SLOTID the command will pass the 
	//  filter for all DCDC's to execute.

	//PSU will use this filter so it only sees command for its PSUADR and DCADR
	//CanFilter1 		= (PSUADR * 0x10000) + (DCADR * 0x1000) + 0x00000fff;  //ex1. 0x00002fff = DCADR is 2 ex 2. 0x00003fff DCADR is 3
	//CanFilter2 		= 0x00000fff;//allows DCADR of '0' to pass filter this is the broadcast address
	//CanFilterMask 	= 0x1ffff000; // Bits 0-11 are function-id's allow any number from 000 through fff this is our command set	
	
	//LCD Controller will use this filter so it sees commands from all PSUADR and DCADR
	CanFilter1 		= (PSUADR * 0x10000) + (DCADR * 0x1000) + 0x00000fff;  //ex1. 0x00002fff = DCADR is 2 ex 2. 0x00003fff DCADR is 3
	CanFilter2 		= 0x00000fff;//allows PSUADR && DCADR of '0' to pass filter this is the broadcast address
	CanFilterMask 	= 0x1f000000; // Bits 0-11 are function-id's allow any number from 000 through fff this is our command set	
																// also allow any DCADR ( bits 12-15 ) and any PSUADR (bits 16-23 )
																
	
}
/******************************************************************************/







/******************************************************************************/
/*                          CANDoCANSlave  							 		  								*/
/******************************************************************************/
void CANDoCAN(void)
{
	int buffindex = 0;

	while(Dequeue(&buffindex))
	{
		CANParseCommand(Canbuff[buffindex].data,Canbuff[buffindex].id,Canbuff[buffindex].dlc);
		Canbuff[buffindex].flag = 0;
	}
}
/******************************************************************************/




/******************************************************************************/
/*                          CANParseCommand							 		  								*/
/******************************************************************************/
void CANParseCommand(uint8_t *cmd, uint32_t id,uint8_t dlc)
{
	int i;
	uint16_t scmd;

	scmd = CANGetCommand(id);
	
	for(i=0;i<CANCMDCOUNT;i++)
	{
		if( scmd == CANSyntax[i] )
		{	
			(*FuncCANCmdArray[i])(cmd,id,dlc);
			return;
		}
	}
}
/******************************************************************************/




/******************************************************************************/
/*                          CANSetCanID    							 		  							  */
/******************************************************************************/
uint32_t CANSetCanID(uint8_t psuadr, uint8_t dcadr)
{
	// set the CANID to the last six chars of the MAC address
	uint32_t canid;
	uint16_t funcid =0x0000;
	
	PSUADR = psuadr;
	DCADR =  dcadr;
		
	canid = psuadr;
	canid = canid <<4;
	canid = canid | dcadr;
	canid = canid <<12;
	canid = canid | funcid;
	
	return canid;
}
/******************************************************************************/



/******************************************************************************/
/*                          CANWrite
 * 
 * CANWrite(id, canBuff, 0x206, datalength);		 		                          */
/******************************************************************************/
void CANWrite(uint8_t psuadr,uint8_t dcadr,uint16_t funcid,uint8_t *data, uint8_t datalength)
{
		uint32_t canid;
		
		canid = psuadr;
		canid = canid <<4;
		canid = canid | dcadr;
		canid = canid <<12;
		canid = canid | funcid;
	
		CanTxBufferMutator(data,datalength, canid);
}
/******************************************************************************/

/******************************************************************************/
/*                         			CANLoadSyntax  					 		                  */
/******************************************************************************/
void CANLoadSyntax()
{
	int8_t i=0;

	CANSyntax[i] 				    = CMD_CAN_200;
	FuncCANCmdArray[i++]		=	&CANFunction_200;

	CANSyntax[i] 				    =	CMD_CAN_201;				
	FuncCANCmdArray[i++]		=	&CANFunction_201;

	CANSyntax[i] 				    =	CMD_CAN_202;				
	FuncCANCmdArray[i++]		=	&CANFunction_202;
	
}
/******************************************************************************/


/******************************************************************************/
/*                         		CANGetCommand						 		  									*/
/******************************************************************************/
uint16_t CANGetCommand(uint32_t id)
{
	unsigned long x;
	unsigned short rval;


	x = id & 0x00000FFF;			// strip out all except the 4 byte command

	rval = (unsigned short)x;

	return rval;	// return the command
}
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
// @author Vince Keiper
// @brief CN_HLLO Response handler. the request gets sent out from the state machine
//	      for each PSUIDX. The response populates the the PSU ADR & DCADR for each PSU
//				struct.
// @param cmd, 8 deep uint8 buffer
// @param id, uint32 CANID from PSU
// @retval none
// 
/******************************************************************************/
void CANFunction_200(uint8_t *cmd, uint32_t id,uint8_t dlc)
{
	  uint8_t lclpsuidx,dcidx;
		lclpsuidx = id >>16;
		dcidx = (id >>12) & 0x0F;
	
		//GuiState.SysDataPSU[lclpsuidx].Status |= 0x0001;// set CAN HELLO RXD bit for PSU that sent the response "lclpsuidx"
	
		//GuiState.SysDataPSU[lclpsuidx].PSU_ADDR = lclpsuidx;
	  //GuiState.SysDataPSU[lclpsuidx].DC_ADDR  = dcidx;
		
		// No Echo needed for a CAN_HELLO Echo is only needed for qry for the LCD from the PSU
		//CANWrite(lclpsuidx,dcidx,CMD_CAN_200, canBuff, datalength);  
}


#define ACOK_flag 1
#define DCDC_PWM_Ena 1
#define OV_flag 1
#define OT_flag 1
#define Enable_cmd 1
uint16_t DCDCVoltage,DCDCCurrent,Temperature;

/******************************************************************************/
/******************************************************************************/
// @author Vince Keiper
// @brief CN_PFLQRY Response handler. the request gets sent out from the state machine
//				Byte 0 of each CAN frame will contain the current frame count.
//        This function will recieve multiple frames to get the iccp runtime profile
//			  the PSU is running. If none is set the PSU will return 0xff in Byte zero
//				Byte 0 will contain the frame count Bytes 1-7 are the data payload.
//        If an 0xFF is received in the frame count byte that means no profile is running
//        and the profile rxd bit must be CLEARED in the data struct.
//        The LCD controller will always echo back the frame count byte ( Byte0 ) to
//				the PSU to tell the PSU to send the next frame. 
//   
//  Example Profile data struct. Contains 27 bytes which will take 4 frames of 7 data byte ( 1 byte is the frame count )
//  typedef struct {
//			psu_mode_t psu_mode;// P_CV,P_CI,P_CPOT,P_OFF
//			int16_t refelec_min_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
//			int16_t refelec_max_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
//			uint16_t rec_vset;        // resolution 1mv/bit 4096 = Full Scale
//			uint16_t rec_vset_max_th; // resolution 100mv/bit 4096 = Full Scale
//			uint16_t rec_vset_min_th; // resolution 100mv/bit
//			uint16_t rec_fs_vset;     // resolution = vset_fs / 4096 ex 63V/4096 = 15mA/bit for a 54V rect. this would be the OV point or 63v= 630bits
//			uint16_t rec_iset; 				// resolution = iset_fs/4096 ex. 125A/4096 = 30mA/bit
//			uint16_t rec_iset_max_th; // resolution 1A/bit
//			uint16_t rec_iset_min_th; // resolution 1A/bit
//			uint16_t rec_fs_iset;     // resolution 1A/bit 
//			uint16_t alarm_reg_mask;  // 0= bit masked 1= active, same bit mapping as alarm register
//			uint16_t self_test_int;// interval self test is run at in minutes/bit 0= disbaled
//			uint8_t adc_smp_int;// interval adc samples are placed into 128 sample array mS/bit 0=uses real time values for debug purposes
//			uint8_t control_int;// interval the rectifer output current is updated based on the ref elec AVG buffers. second/bit 
//			uint32_t tsts_active; // bit mapped to each test in the self test routine, to allow us to select which tests will be run
//			uint8_t  refelec_active; //
//} IccpPrfParam_t; //Iccp Profile Parameters

// @param cmd, 8 deep uint8 buffer
// @param id, uint32 CANID from PSU
// @retval none
// 
/******************************************************************************/
void CANFunction_201(uint8_t *cmd, uint32_t id,uint8_t dlc)
{
	static uint8_t frm_cnt[4];// frame count received tfrom the PSU
	uint8_t lclpsuidx,dcidx;
	
	lclpsuidx = id >>16;
	dcidx = (id >>12) & 0x0F;
	
	memset(canBuff, 0, 8);
	
	frm_cnt[lclpsuidx] = cmd[0];// byte 0 from PSU always contains frame count 
	canBuff[0] = frm_cnt[lclpsuidx];
	datalength = 1;// echo will only be 1 byte, the frame count
	
	// Profile struct contains 27 bytes of data, we use 7 bytes per CAN frame
	// so we need 4 frames to get 27 bytes of data
	switch (frm_cnt[lclpsuidx])
	{
			case 0:  //first frame contains 
					//	psu_mode_t psu_mode;// P_CV,P_CI,P_CPOT,P_OFF
					//	int16_t refelec_min_th;  // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
					//	int16_t refelec_max_th;  // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
					//	uint16_t rec_vset_max_th; // resolution 1mv/bit 4096 = Full Scale
			//	  GuiState.SysDataPSU[lclpsuidx].pflparams.psu_mode        = (psu_mode_t)cmd[1];
			//		GuiState.SysDataPSU[lclpsuidx].pflparams.refelec_min_th  = cmd[2] + (cmd[3] * 0x100);
			//		GuiState.SysDataPSU[lclpsuidx].pflparams.refelec_max_th  = cmd[4] + (cmd[5] * 0x100);
			//		GuiState.SysDataPSU[lclpsuidx].pflparams.rec_vset_max_th = cmd[6] + (cmd[7] * 0x100);
				
			break;
			
			case 1: // Frame 2 contains....
				  //	uint16_t rec_vset_min_th; // resolution 100mv/bit
					//	uint16_t rec_fs_vset;     // .1V/bit if 54V rectifier this would be the OV point or 63v= 630bits
					//	uint16_t rec_iset_max_th; // resolution 100mA/bit
					//	uint16_t rec_iset_min_th; // resolution 100mA/bit  LOW BYTE ONLY, HI BYTE COMES IN NEXT FRAME

			//	  GuiState.SysDataPSU[lclpsuidx].pflparams.rec_vset_min_th  = cmd[1] + (cmd[2] * 0x100);
			//		GuiState.SysDataPSU[lclpsuidx].pflparams.rec_fs_vset      = cmd[3] + (cmd[4] * 0x100);
			//		GuiState.SysDataPSU[lclpsuidx].pflparams.rec_iset_max_th  = cmd[5] + (cmd[6] * 0x100);
			//		GuiState.SysDataPSU[lclpsuidx].pflparams.rec_iset_min_th  = cmd[7] & 0x00ff; 
			break;
			
			case 2:// Frame 3 contains....
				  //	uint16_t rec_iset_min_th; // resolution 100mA/bit  HI BYTE ONLY, LO BYTE WAS STUFFED IN LAST FRAME
					//	uint16_t rec_fs_iset;     // .1A/bit if rectifier can do 125A at ILIM this would be the 125A= 1250 bits
					//	uint16_t alarm_reg_mask;  // 0= bit masked 1= active, same bit mapping as alarm register
					//	uint16_t self_test_int;// interval self test is run at in minutes/bit 0= disbaled
			//	GuiState.SysDataPSU[lclpsuidx].pflparams.rec_iset_min_th  += (cmd[1] * 0x100);
			//		GuiState.SysDataPSU[lclpsuidx].pflparams.rec_fs_iset       = cmd[2] + (cmd[3] * 0x100);
			//		GuiState.SysDataPSU[lclpsuidx].pflparams.alarm_reg_mask    = cmd[4] + (cmd[5] * 0x100);
			//		GuiState.SysDataPSU[lclpsuidx].pflparams.self_test_int     = cmd[6] + (cmd[7] * 0x100);
			break;
			
			case 3: // Frame 4 contains....
					//	uint8_t adc_smp_int;// interval adc samples are placed into 128 sample array mS/bit 0=uses real time values for debug purposes
					//	uint8_t control_int;// interval the rectifer output current is updated based on the ref elec AVG buffers. second/bit 
					//  uint32_t tsts_active; // bit mapped to each test in the self test routine, to allow us to select which tests will be run
			//		GuiState.SysDataPSU[lclpsuidx].pflparams.adc_smp_int        = cmd[1];
			//		GuiState.SysDataPSU[lclpsuidx].pflparams.control_int        = cmd[2];
			//		GuiState.SysDataPSU[lclpsuidx].pflparams.tsts_active        = cmd[3] + (cmd[4] * 0x100) + (cmd[5] * 0x10000) + (cmd[6] * 0x1000000);
			//		GuiState.SysDataPSU[lclpsuidx].Status |= 0x0004;// OR ON bit 2 "Profile RXD bit"
				  
					
			break;
			
			case 0xFF:
					// if we get an 0xff for the frame count and we did not get any other frames successully then
					// the PSU is telling us it doe s not have a ICCP profile programmed yet.
		//			GuiState.SysDataPSU[lclpsuidx].Status &= 0xFFFB;// AND OFF bit 2 "Profile RXD bit" 
				
			break;
			
			default:
			break;
	}
	
  
  CANWrite(lclpsuidx,dcidx,CMD_CAN_201, canBuff, datalength);
	
}



/******************************************************************************/
/******************************************************************************/
// @author Vince Keiper
// @brief CN_STAQRY Response handler. the request gets sent out from the state machine
//				Byte 0 of each CAN frame will contain the curent frame count.
//        This function will recieve multiple frames to get all of the PSU status data
//				which is 13 bytes. 
//				Byte 0 will contain the frame count Bytes 1-7 are data payload.
//        The LCD controller will always echo back the frame count byte ( Byte0 ) to
//				the PSU to tell the PSU to send the next frame. 
//   
//  Example Profile data struct. Contains 27 bytes which will take 4 frames of 7 data byte ( 1 byte is the frame count )
//  typedef struct {
//	psu_mode_t psu_mode;// P_CV,P_CI,P_CPOT,P_OFF
//	int16_t refelec_min_th;  // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
//	int16_t refelec_max_th;  // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
//	uint16_t rec_vset_max_th; // resolution 1mv/bit 4096 = Full Scale
//	uint16_t rec_vset_min_th; // resolution 100mv/bit
//	uint16_t rec_fs_vset;     // .1V/bit if 54V rectifier this would be the OV point or 63v= 630bits
//	uint16_t rec_iset_max_th; // resolution 100mA/bit
//	uint16_t rec_iset_min_th; // resolution 100mA/bit
//	uint16_t rec_fs_iset;     // .1A/bit if rectifier can do 125A at ILIM this would be the 125A= 1250 bits
//	uint16_t alarm_reg_mask;  // 0= bit masked 1= active, same bit mapping as alarm register
//	uint16_t self_test_int;// interval self test is run at in minutes/bit 0= disbaled
//	uint8_t adc_smp_int;// interval adc samples are placed into 128 sample array mS/bit 0=uses real time values for debug purposes
//	uint8_t control_int;// interval the rectifer output current is updated based on the ref elec AVG buffers. second/bit 
//  uint32_t tsts_active; // bit mapped to each test in the self test routine, to allow us to select which tests will be run
//} IccpPrfParam_t; //Iccp Profile Parameters

// @param cmd, 8 deep uint8 buffer
// @param id, uint32 CANID from PSU
// @retval none
// 
/******************************************************************************/
void CANFunction_202(uint8_t *cmd, uint32_t id,uint8_t dlc)
{ 
	static uint8_t frm_cnt[4];// frame count received tfrom the PSU
	uint8_t lclpsuidx,dcidx;
	
	lclpsuidx = id >>16;
	dcidx = (id >>12) & 0x0F;
	
	memset(canBuff, 0, 8);
	
	frm_cnt[lclpsuidx] = cmd[0];// byte 0 from PSU always contains frame count 
	canBuff[0] = frm_cnt[lclpsuidx];
	datalength = 1;// echo will only be 1 byte, the frame count
	
	// A PSU status contains 15 bytes of data, each CAN frame has one byte for frame count
	// and 7 bytes of data payload per CAN frame
	// So we need 3 frames to get 15 bytes of data
	switch (frm_cnt[lclpsuidx])
	{
			case 0:  //first frame contains 
			//		GuiState.SysDataPSU[lclpsuidx].Vrdbk       = cmd[1] + (cmd[2] * 0x100);
			//		GuiState.SysDataPSU[lclpsuidx].Irdbk       = cmd[3] + (cmd[4] * 0x100); 
			//		GuiState.SysDataPSU[lclpsuidx].AlmReg      = cmd[5] + (cmd[6] * 0x100);
			//		GuiState.SysDataPSU[lclpsuidx].PsuTemp     = cmd[7];
					
			break;
			
			case 1: // Frame 2 contains....
			//	  GuiState.SysDataPSU[lclpsuidx].RefCells[0] = cmd[1] + (cmd[2] * 0x100);
			//		GuiState.SysDataPSU[lclpsuidx].RefCells[1] = cmd[3] + (cmd[4] * 0x100); 
			//		GuiState.SysDataPSU[lclpsuidx].RefCells[2] = cmd[5] + (cmd[6] * 0x100);
					
			break;
						
			case 2: // Frame 3 contains....
			//	  GuiState.SysDataPSU[lclpsuidx].RefCells[3] = cmd[1] + (cmd[2] * 0x100);
			//		GuiState.SysDataPSU[lclpsuidx].Status |= 0x0008;// OR ON bit 3 "STARSP RXD bit"
			break;
	}
	 
	CANWrite(lclpsuidx,dcidx,CMD_CAN_202, canBuff, datalength);
}

/******************************************************************************/
// @author Vince Keiper
// @brief CN_PFLDNLD_FLASH. CANbus command, Iccp Profile download from LCD ctl to the PSU ctl flash.
//        This will  be saved in the PSUs flash and recalled upon power up.
//				The PSU selected from the LCD GUI will determine the PSU address
//        the request is sent to. And that in turn will determine which PSU answers the 
//				command and the lclpsuidx variable will be set in here based on that.
//        So when sending the 203 cmd request you must pass the correct PSU address at that time.
//        Request sent to PSU from the LCD gui, or TCP, HTTP server.
//				The request will go out with NO DATA DLC=0, the PSU will respond with the frame count byte of zero. 
//				That will kick off the transmission of the remaining frames because the 203 response handler.
//				Byte 0 of each CAN frame will contain the current frame count.
//
//        This function will have to transmit multiple frames to get the entire iccp profile
//			  to the PSU. The PSU must echo back the frame count to ACK it received the data. This will 
//			  trigger the LCD controller to send the next frame. If no response from the PSU within 5 seconds the command will timeout 
//				and go into COM fault.
//				As discussed Byte 0 will contain the frame count Bytes 1-7 are the data payload.
//        
//  Example Profile data struct. Contains 27 bytes which will take 4 frames with 7 data bytes in each frame
//			( Again, these CANbus frames can have 8 bytes of data but we use one byte for the frame count )
//  typedef struct {
//			psu_mode_t psu_mode;// P_CV,P_CI,P_CPOT,P_OFF
//			int16_t refelec_min_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
//			int16_t refelec_max_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
//			uint16_t rec_vset;        // resolution 1mv/bit 4096 = Full Scale
//			uint16_t rec_vset_max_th; // resolution 100mv/bit 4096 = Full Scale
//			uint16_t rec_vset_min_th; // resolution 100mv/bit
//			uint16_t rec_fs_vset;     // resolution = vset_fs / 4096 ex 63V/4096 = 15mA/bit for a 54V rect. this would be the OV point or 63v= 630bits
//			uint16_t rec_iset; 				// resolution = iset_fs/4096 ex. 125A/4096 = 30mA/bit
//			uint16_t rec_iset_max_th; // resolution 1A/bit
//			uint16_t rec_iset_min_th; // resolution 1A/bit
//			uint16_t rec_fs_iset;     // resolution 1A/bit 
//			uint16_t alarm_reg_mask;  // 0= bit masked 1= active, same bit mapping as alarm register
//			uint16_t self_test_int;// interval self test is run at in minutes/bit 0= disbaled
//			uint8_t adc_smp_int;// interval adc samples are placed into 128 sample array mS/bit 0=uses real time values for debug purposes
//			uint8_t control_int;// interval the rectifer output current is updated based on the ref elec AVG buffers. second/bit 
//			uint32_t tsts_active; // bit mapped to each test in the self test routine, to allow us to select which tests will be run
//			uint8_t  refelec_active; //
//} IccpPrfParam_t; //Iccp Profile Parameters

// @param cmd, 8 deep uint8 buffer
// @param id, uint32 CANID from PSU
// @retval none
// 
/******************************************************************************/
void CANFunction_203(uint8_t *cmd, uint32_t id,uint8_t dlc)
{
	static uint8_t frm_cnt[4];// frame count received tfrom the PSU
	uint8_t lclpsuidx,dcidx;
	
	lclpsuidx = id >>16;
	dcidx = (id >>12) & 0x0F;
	
	memset(canBuff, 0, 8);
	
	// if DLC is zero this is the PSU responding to the request so there is no data
	// this kicks off the transmission of data frames 
	if(dlc == 0)
	{
			frm_cnt[lclpsuidx] = 0x00;// start sending 
	}
	else
	{
			frm_cnt[lclpsuidx] = cmd[0]+1; //now we increment the frame count index rx'd from the PSU
																		 // to send the next frame
	}
	
	
	canBuff[0] = frm_cnt[lclpsuidx];
	datalength = 8;// we will always send 8 bytes 
	
	// Profile struct contains 27 bytes of data, we use 7 bytes per CAN frame
	// so we need 4 frames to get 27 bytes of data
	switch (frm_cnt[lclpsuidx])
	{
			case 0:  //first frame contains 
			//		GuiState.SysDataPSU[lclpsuidx].Status &= 0xFFBF;// AND OFF bit 6 "Profile TX'd bit" 
					//	psu_mode_t psu_mode;// P_CV,P_CI,P_CPOT,P_OFF
					//	int16_t refelec_min_th;  // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
					//	int16_t refelec_max_th;  // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
					//	uint16_t rec_vset_max_th; // resolution 1mv/bit 4096 = Full Scale
			//	  canBuff[1] = GuiState.SysDataPSU[lclpsuidx].pflparams.psu_mode;
			//		canBuff[2] = GuiState.SysDataPSU[lclpsuidx].pflparams.refelec_min_th & 0x00FF;
			//		canBuff[3] = GuiState.SysDataPSU[lclpsuidx].pflparams.refelec_min_th >>8;
			//		canBuff[4] = GuiState.SysDataPSU[lclpsuidx].pflparams.refelec_max_th & 0x00ff;
				//  canBuff[5] = GuiState.SysDataPSU[lclpsuidx].pflparams.refelec_max_th >>8;
			//		canBuff[6] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_vset_max_th & 0x00ff;
			//	  canBuff[7] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_vset_max_th >>8;
				  
			break;
			
			case 1: // Frame 2 contains....
				  //	uint16_t rec_vset_min_th; // resolution 100mv/bit
					//	uint16_t rec_fs_vset;     // .1V/bit if 54V rectifier this would be the OV point or 63v= 630bits
					//	uint16_t rec_iset_max_th; // resolution 100mA/bit
					//	uint16_t rec_iset_min_th; // resolution 100mA/bit  LOW BYTE ONLY, HI BYTE COMES IN NEXT FRAME
			//	  canBuff[1] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_vset_min_th & 0x00ff;
			//		canBuff[2] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_vset_min_th >>8;
			//		canBuff[3] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_fs_vset  & 0x00FF;
			//	canBuff[4] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_fs_vset  >>8;
			//		canBuff[5] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_iset_max_th & 0x00ff;
			//	  canBuff[6] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_iset_max_th >>8;
			//		canBuff[7] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_iset_min_th & 0x00ff;
			break;
			
			case 2:// Frame 3 contains....
				  //	uint16_t rec_iset_min_th; // resolution 100mA/bit  HI BYTE ONLY, LO BYTE WAS STUFFED IN LAST FRAME
					//	uint16_t rec_fs_iset;     // .1A/bit if rectifier can do 125A at ILIM this would be the 125A= 1250 bits
					//	uint16_t alarm_reg_mask;  // 0= bit masked 1= active, same bit mapping as alarm register
					//	uint16_t self_test_int;// interval self test is run at in minutes/bit 0= disbaled
			//		canBuff[1] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_iset_min_th;
			//		canBuff[2] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_fs_iset & 0x00FF;
			//		canBuff[3] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_fs_iset >>8;
			//		canBuff[4] = GuiState.SysDataPSU[lclpsuidx].pflparams.alarm_reg_mask & 0x00ff;
			//	  canBuff[5] = GuiState.SysDataPSU[lclpsuidx].pflparams.alarm_reg_mask >>8;
			//		canBuff[6] = GuiState.SysDataPSU[lclpsuidx].pflparams.self_test_int & 0x00ff;
			//	  canBuff[7] = GuiState.SysDataPSU[lclpsuidx].pflparams.self_test_int >>8;
				
			break;
			
			case 3: // Frame 4 contains....
					//	uint8_t adc_smp_int;// interval adc samples are placed into 128 sample array mS/bit 0=uses real time values for debug purposes
					//	uint8_t control_int;// interval the rectifer output current is updated based on the ref elec AVG buffers. second/bit 
					//  uint32_t tsts_active; // bit mapped to each test in the self test routine, to allow us to select which tests will be run
			//		canBuff[1] = GuiState.SysDataPSU[lclpsuidx].pflparams.adc_smp_int;
			//		canBuff[2] = GuiState.SysDataPSU[lclpsuidx].pflparams.control_int;
			//		canBuff[3] = GuiState.SysDataPSU[lclpsuidx].pflparams.tsts_active & 0x000000ff;
			//		canBuff[4] = GuiState.SysDataPSU[lclpsuidx].pflparams.tsts_active >>8 & 0x000000ff;
			//		canBuff[5] = GuiState.SysDataPSU[lclpsuidx].pflparams.tsts_active >>16 & 0x000000ff;
			//		canBuff[6] = GuiState.SysDataPSU[lclpsuidx].pflparams.tsts_active >>24 & 0x000000ff;
			//		canBuff[7] = 0xff;//Spare byte not used in profile just sent 8 so DLC is same in all frames
			
			//		GuiState.SysDataPSU[lclpsuidx].Status |= 0x0040;// OR on bit 6 "Profile TXD bit" 
			break;
			
			default:
			break;
	}
  
  CANWrite(lclpsuidx,dcidx,CMD_CAN_203, canBuff, datalength);
	
}


/******************************************************************************/
// @author Vince Keiper
// @brief CN_PFLDNLD_RAM. CANbus command, Iccp Profile download from LCD ctl to the PSU ctl RAM.
//        This will be saved in the PSUs RAM and NOT be recalled upon power up.
//        If PSU is powered down and back up it will recall the profile saved in flash.
//				The PSU selected from the LCD GUI will determine the PSU address
//        the request is sent to. And that in turn will determine which PSU answers the 
//				command and the lclpsuidx variable will be set in here based on that.
//        So when sending the 203 cmd request you must pass the correct PSU address at that time.
//        Request sent to PSU from the LCD gui, or TCP, HTTP server.
//				The request will go out with NO DATA DLC=0, the PSU will respond with the frame count byte of zero. 
//				That will kick off the transmission of the remaining frames because the 203 response handler.
//				Byte 0 of each CAN frame will contain the current frame count.
//
//        This function will have to transmit multiple frames to get the entire iccp profile
//			  to the PSU. The PSU must echo back the frame count to ACK it received the data. This will 
//			  trigger the LCD controller to send the next frame. If no response from the PSU within 5 seconds the command will timeout 
//				and go into COM fault.
//				As discussed Byte 0 will contain the frame count Bytes 1-7 are the data payload.
//        
//  Example Profile data struct. Contains 27 bytes which will take 4 frames with 7 data bytes in each frame
//			( Again, these CANbus frames can have 8 bytes of data but we use one byte for the frame count )
//  typedef struct {
//			psu_mode_t psu_mode;// P_CV,P_CI,P_CPOT,P_OFF
//			int16_t refelec_min_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
//			int16_t refelec_max_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
//			uint16_t rec_vset;        // resolution 1mv/bit 4096 = Full Scale
//			uint16_t rec_vset_max_th; // resolution 100mv/bit 4096 = Full Scale
//			uint16_t rec_vset_min_th; // resolution 100mv/bit
//			uint16_t rec_fs_vset;     // resolution = vset_fs / 4096 ex 63V/4096 = 15mA/bit for a 54V rect. this would be the OV point or 63v= 630bits
//			uint16_t rec_iset; 				// resolution = iset_fs/4096 ex. 125A/4096 = 30mA/bit
//			uint16_t rec_iset_max_th; // resolution 1A/bit
//			uint16_t rec_iset_min_th; // resolution 1A/bit
//			uint16_t rec_fs_iset;     // resolution 1A/bit 
//			uint16_t alarm_reg_mask;  // 0= bit masked 1= active, same bit mapping as alarm register
//			uint16_t self_test_int;// interval self test is run at in minutes/bit 0= disbaled
//			uint8_t adc_smp_int;// interval adc samples are placed into 128 sample array mS/bit 0=uses real time values for debug purposes
//			uint8_t control_int;// interval the rectifer output current is updated based on the ref elec AVG buffers. second/bit 
//			uint32_t tsts_active; // bit mapped to each test in the self test routine, to allow us to select which tests will be run
//			uint8_t  refelec_active; //
//} IccpPrfParam_t; //Iccp Profile Parameters

// @param cmd, 8 deep uint8 buffer
// @param id, uint32 CANID from PSU
// @retval none
// 
/******************************************************************************/
void CANFunction_204(uint8_t *cmd, uint32_t id,uint8_t dlc)
{
	static uint8_t frm_cnt[4];// frame count received tfrom the PSU
	uint8_t lclpsuidx,dcidx;
	
	lclpsuidx = id >>16;
	dcidx = (id >>12) & 0x0F;
	
	memset(canBuff, 0, 8);
	
	// if DLC is zero this is the PSU responding to the request so there is no data
	// this kicks off the transmission of data frames 
	if(dlc == 0)
	{
			frm_cnt[lclpsuidx] = 0x00;// start sending 
	}
	else
	{
			frm_cnt[lclpsuidx] = cmd[0]+1; //now we increment the frame count index rx'd from the PSU
																		 // to send the next frame
	}
	
	
	canBuff[0] = frm_cnt[lclpsuidx];
	datalength = 8;// we will always send 8 bytes 
	
	// Profile struct contains 27 bytes of data, we use 7 bytes per CAN frame
	// so we need 4 frames to get 27 bytes of data
	switch (frm_cnt[lclpsuidx])
	{
			case 0:  //first frame contains 
			//		GuiState.SysDataPSU[lclpsuidx].Status &= 0xFFBF;// AND OFF bit 6 "Profile TX'd bit" 
					//	psu_mode_t psu_mode;// P_CV,P_CI,P_CPOT,P_OFF
					//	int16_t refelec_min_th;  // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
					//	int16_t refelec_max_th;  // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
					//	uint16_t rec_vset_max_th; // resolution 1mv/bit 4096 = Full Scale
			//	  canBuff[1] = GuiState.SysDataPSU[lclpsuidx].pflparams.psu_mode;
			//		canBuff[2] = GuiState.SysDataPSU[lclpsuidx].pflparams.refelec_min_th & 0x00FF;
			//		canBuff[3] = GuiState.SysDataPSU[lclpsuidx].pflparams.refelec_min_th >>8;
//					canBuff[4] = GuiState.SysDataPSU[lclpsuidx].pflparams.refelec_max_th & 0x00ff;
//				  canBuff[5] = GuiState.SysDataPSU[lclpsuidx].pflparams.refelec_max_th >>8;
//					canBuff[6] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_vset_max_th & 0x00ff;
//				  canBuff[7] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_vset_max_th >>8;
				  
			break;
			
			case 1: // Frame 2 contains....
				  //	uint16_t rec_vset_min_th; // resolution 100mv/bit
					//	uint16_t rec_fs_vset;     // .1V/bit if 54V rectifier this would be the OV point or 63v= 630bits
					//	uint16_t rec_iset_max_th; // resolution 100mA/bit
					//	uint16_t rec_iset_min_th; // resolution 100mA/bit  LOW BYTE ONLY, HI BYTE COMES IN NEXT FRAME
//				  canBuff[1] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_vset_min_th & 0x00ff;
//					canBuff[2] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_vset_min_th >>8;
//					canBuff[3] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_fs_vset  & 0x00FF;
//					canBuff[4] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_fs_vset  >>8;
//					canBuff[5] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_iset_max_th & 0x00ff;
//				  canBuff[6] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_iset_max_th >>8;
//					canBuff[7] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_iset_min_th & 0x00ff;
			break;
			
			case 2:// Frame 3 contains....
				  //	uint16_t rec_iset_min_th; // resolution 100mA/bit  HI BYTE ONLY, LO BYTE WAS STUFFED IN LAST FRAME
					//	uint16_t rec_fs_iset;     // .1A/bit if rectifier can do 125A at ILIM this would be the 125A= 1250 bits
					//	uint16_t alarm_reg_mask;  // 0= bit masked 1= active, same bit mapping as alarm register
					//	uint16_t self_test_int;// interval self test is run at in minutes/bit 0= disbaled
//					canBuff[1] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_iset_min_th;
//					canBuff[2] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_fs_iset & 0x00FF;
//					canBuff[3] = GuiState.SysDataPSU[lclpsuidx].pflparams.rec_fs_iset >>8;
//					canBuff[4] = GuiState.SysDataPSU[lclpsuidx].pflparams.alarm_reg_mask & 0x00ff;
//				  canBuff[5] = GuiState.SysDataPSU[lclpsuidx].pflparams.alarm_reg_mask >>8;
//					canBuff[6] = GuiState.SysDataPSU[lclpsuidx].pflparams.self_test_int & 0x00ff;
//				  canBuff[7] = GuiState.SysDataPSU[lclpsuidx].pflparams.self_test_int >>8;
				
			break;
			
			case 3: // Frame 4 contains....
					//	uint8_t adc_smp_int;// interval adc samples are placed into 128 sample array mS/bit 0=uses real time values for debug purposes
					//	uint8_t control_int;// interval the rectifer output current is updated based on the ref elec AVG buffers. second/bit 
					//  uint32_t tsts_active; // bit mapped to each test in the self test routine, to allow us to select which tests will be run
//					canBuff[1] = GuiState.SysDataPSU[lclpsuidx].pflparams.adc_smp_int;
//					canBuff[2] = GuiState.SysDataPSU[lclpsuidx].pflparams.control_int;
//					canBuff[3] = GuiState.SysDataPSU[lclpsuidx].pflparams.tsts_active & 0x000000ff;
//					canBuff[4] = GuiState.SysDataPSU[lclpsuidx].pflparams.tsts_active >>8 & 0x000000ff;
//					canBuff[5] = GuiState.SysDataPSU[lclpsuidx].pflparams.tsts_active >>16 & 0x000000ff;
//					canBuff[6] = GuiState.SysDataPSU[lclpsuidx].pflparams.tsts_active >>24 & 0x000000ff;
//					canBuff[7] = 0xff;//Spare byte not used in profile just sent 8 so DLC is same in all frames
//			
//					GuiState.SysDataPSU[lclpsuidx].Status |= 0x0040;// OR on bit 6 "Profile TXD bit" 
			break;
			
			default:
			break;
	}
  
  CANWrite(lclpsuidx,dcidx,CMD_CAN_204, canBuff, datalength);
	
}
