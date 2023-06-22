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
#include "stdio.h"
#include "string.h"
#include "main.h"
#include "selftest.h"
#include "iccpfuncs.h"
#include "iccp_canfuncs.h"
#include "iccp_can_stm32f446.h"

/// uint8_t b_CANTXFlag;
uint8_t canBuff[8], datalength, dummycmd[8]; 
uint32_t dummyid;

uint32_t DCADR;
uint32_t PSUADR;
 
 /* CAN Message Buffer Configuration */
uint32_t CanFilter1;
uint32_t CanFilter2;
uint32_t CanFilterMask;

uint8_t CANCMDCOUNT     = 0;     
#define CANMAXFUNCTION	64
#define CANBUFFCOUNTER	255

uint16_t CANSyntax[CANMAXFUNCTION];

void (*FuncCANCmdArray[CANMAXFUNCTION])(uint8_t *cmd, uint32_t id, uint8_t dlc);

static uint8_t Bufftail   = 0;
static uint8_t Buffhead   = 0;
static uint8_t Buffcount  = 0;

static CANmsgbuff Canbuff[CANBUFFCOUNTER];
static CANmsgbuff dummyread;

void CanApiEnqueue(uint32_t id, uint8_t *canbuffer, uint8_t dlc)
{
	int8_t i;
	dummyread.data[0] = 0;
	
	// dump the message if the ring buffer is full
	if (Buffcount >= CANBUFFCOUNTER)
	{
        for ( i = 0; i < 8; i++ ) 
		{
            dummyread.data[i] = canbuffer[i];	
		}
        i = (int8_t)dummyread.data[0];// benign command to remove warning of unused var dummydata....
	}
	else
	{
        Buffhead++;
		if (Buffhead == CANBUFFCOUNTER) {Buffhead = 0;}
			
        // put message into the buffer if there is not data in that slot
		if (Canbuff[Buffhead].flag == 0)
		{
            Canbuff[Buffhead].id    = id; 
			Canbuff[Buffhead].flag  = 1;  // vk, set data available flag
			Canbuff[Buffhead].dlc   = dlc;
			
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
    if (Buffcount == 0) {return 0;}
	
    else
	{
        Bufftail++;
        
        if (Bufftail == CANBUFFCOUNTER) {Bufftail = 0;}
			
        *buffindex = Bufftail;	// pop the message out of the buffer//
		Buffcount--;
			
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
    // these are set by GPIO in the LB DCDC but in ICCP hard coding them HIGH
    #define SLOT1 1
    #define SLOT2 1
	
	// DCADR can be 1,2 or 3
	DCADR = 0x00;	
	if (SLOT1) {DCADR |= 0x00;}  
	if (SLOT2) {DCADR |= 0x00;}

////    Conditional delay for NODE_ADDR
    while (SysData.nodeadr_adc_flag == 0x00)
    {
        __NOP();
    }

    SysData.NODE_ADDR   = GetNodeAddress(SysData.nodeadr_adc);
	SysData.DC_ADDR     = DCADR;
	
	//0x0XY00FFF  to 0x00XXYFFF
	PSUADR = SysData.NODE_ADDR; // DCDCId Bits 12-15 , NodeId Bits 16-23 
                                // This allows for up to 255 Nodes "Blades" 
                                // but we will use many less

	CANLoadSyntax(); // Loadc the API syntax map


	//  6/25/14 Vince Keiper added notes for TI apps eng.
	//  The CANFilter and Mask only look at the CANID section, the 8 bytes of 
    //  data are not included in the filter and mask scheme. Any data will
	//  pass. CANFilter setting a bit in filter register means in the CANID 
    //  section of the received packet that bit must be a '1' to pass filter.
	//  A cleared bit in the filter register means in the CANID section of the
    //  received packet that bit must be a '0' to pass filter.
	//  The CANFilterMask gives you the ability to bypass the filter. For 
    //  instance setting the bit in the CANFilter register will include that
    //  bit in the filter comparison, clearing the bit '0' is a "Dont Care" 
    //  and this bit is excluded from the filter comparison
	
	//  So for example if slot id = 1 ( which is determined by a DIP switch 
    //  setting on GPIO ) we set the CANFilter1 to be 0x00001fff and the 
	//  Mask is 0x1ffff000 and is applied to both filters 1&2. We can set 
    //  multiple filters, i think up to 4 filters.  We only use 2. One filter
    //  is setup to only allow messages that are for the this specific SLOT_ID 
    //  or DCADR.  Filter2 is used to allow a special command that is a 
    //  broadcast to all addresses. Explained better below.
	
	//  Anyway, look at Filer "1" below only CANID's that are received with 
    //  a "1" in bit 0 of nibble3 will pass the filter.  IF DCADR = "1" if 
    //  DCADR = "2" bit 0 of nibble3 must be a 2 to pass filter.  This is how 
	//  we filter out  messages that are not for this address. So we do not
    //  interrupt the processor for CAN messages that are for another address.
    //  This is all done in hardware in the ECAN module of the micro.
	
	//  Filter2 is setup to allow broadcasts to all SLOTID's (DCADR's) to 
    //  accept any function ID. This is done by sending a zero in bit 0 of
	//  nibble 3. So irregardless of the SLOTID the command will pass the 
	//  filter for all DCDC's to execute.

	//  PSU will use this filter so it only sees command for its PSUADR and DCADR
    //  ex1. 0x00002fff = DCADR is 2 ex 2. 0x00003fff DCADR is 3
	//  CanFilter1 		= (PSUADR * 0x10000) + (DCADR * 0x1000) + 0x00000fff;
    //  allows DCADR of '0' to pass filter this is the broadcast address
	//  CanFilter2 		= 0x00000fff;
    //  Bits 0-11 are function-id's allow any number from 000 through fff this 
    //  is our command set
	//  CanFilterMask 	= 0x1ffff000; 	
	
	//  LCD Controller will use this filter so it sees commands from all
    //  PSUADR and DCADR
    //  ex1. 0x00002fff = DCADR is 2 ex 2. 0x00003fff DCADR is 3
	CanFilter1 = (PSUADR * 0x10000) + (DCADR * 0x1000) + 0x00000fff; 
    // allows PSUADR && DCADR of '0' to pass filter this is the broadcast address
	CanFilter2 = 0x00000fff;
    //  Bits 0-11 are function-id's allow any number from 000 through fff this 
    //  is our command set also allow any DCADR ( bits 12-15 ) and any PSUADR 
    //  (bits 16-23 )
	CanFilterMask = 0x1f000000;															
}
/******************************************************************************/

/******************************************************************************/
/*                          CANDoCANSlave  							 		  */
/******************************************************************************/
void CANDoCAN(void)
{
	int buffindex = 0;

   	while (Dequeue(&buffindex))
	{
		CANParseCommand(Canbuff[buffindex].data, Canbuff[buffindex].id, Canbuff[buffindex].dlc);
		Canbuff[buffindex].flag = 0;
	}
}
/******************************************************************************/

/******************************************************************************/
/*                          CANParseCommand							 		  */
/******************************************************************************/
void CANParseCommand(uint8_t *cmd, uint32_t id, uint8_t dlc)
{
	int i;
	uint16_t scmd;

	scmd = CANGetCommand(id);
	
	for (i = 0; i < CANCMDCOUNT; i++)
	{
		if ( scmd == CANSyntax[i])
		{	
			(*FuncCANCmdArray[i])(cmd, id, dlc);
			return;
		}
	}
}
/******************************************************************************/

/******************************************************************************/
/*                          CANSetCanID    							 		  */
/******************************************************************************/
uint32_t CANSetCanID(uint8_t psuadr, uint8_t dcadr)
{
	// set the CANID to the last six chars of the MAC address
	uint32_t canid;
	uint16_t funcid = 0x0000;
	
	PSUADR  = psuadr;
	DCADR   = dcadr;
		
	canid = psuadr;
	canid = canid << 4;
	canid = canid | dcadr;
	canid = canid << 12;
	canid = canid | funcid;
	
	return canid;
}
/******************************************************************************/

/******************************************************************************/
/*                          CANWrite
 * 
 * CANWrite(id, canBuff, 0x206, datalength);		 		                  */
/******************************************************************************/
void CANWrite(uint8_t psuadr, uint8_t dcadr, uint16_t funcid, uint8_t *data, uint8_t datalength)
{
    uint32_t canid;
    
    canid = psuadr;
    canid = canid << 4;
    canid = canid | dcadr;
    canid = canid << 12;
    canid = canid | funcid;

    CanTxBufferMutator(data, datalength, canid);
}
/******************************************************************************/

/******************************************************************************/
/*                         			CANLoadSyntax  					 		  */
/******************************************************************************/
void CANLoadSyntax()
{
	int8_t i=0;

	CANSyntax[i] 				= CMD_CAN_200;
	FuncCANCmdArray[i++]		= &CANFunction_200;

	CANSyntax[i] 				= CMD_CAN_201;				
	FuncCANCmdArray[i++]		= &CANFunction_201;

	CANSyntax[i] 				= CMD_CAN_202;				
	FuncCANCmdArray[i++]		= &CANFunction_202;
	
	CANSyntax[i] 				= CMD_CAN_203;				
	FuncCANCmdArray[i++]		= &CANFunction_203;
	
	CANSyntax[i] 				= CMD_CAN_204;				
	FuncCANCmdArray[i++]		= &CANFunction_204;

    CANSyntax[i] 				= CMD_CAN_205;				
	FuncCANCmdArray[i++]		= &CANFunction_205;

    CANCMDCOUNT = i + 1;
}
/******************************************************************************/

/******************************************************************************/
/*                         		CANGetCommand						 		  */
/******************************************************************************/
uint16_t CANGetCommand(uint32_t id)
{
	unsigned long   x;
	unsigned short  rval;

	x = id & 0x00000FFF;    // strip out all except the 4 byte command

	rval = (unsigned short)x;

	return rval;	// return the command
}
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
// @author  Vince Keiper
// @brief   CN_HLLO Response handler. the request gets sent out from 
//          the state machine for each PSUIDX.
//	        The response populates the PSU ADR & DCADR for each PSU struct.
// @param   cmd, 8 deep uint8 buffer
// @param   id, uint32 CANID from PSU
// @retval  none
// 
/******************************************************************************/
void CANFunction_200(uint8_t *cmd, uint32_t id, uint8_t dlc)
{
	BITSET(SysData.Status, 0);  // OR on bit 0 rxd request for CAN HELLO bit

    // Echo to LCD controller needed for a CAN_HELLO 
	CANWrite(SysData.NODE_ADDR, SysData.DC_ADDR, CMD_CAN_200, canBuff, datalength);
    BITCLR(SysData.Status, 0);
    BITSET(SysData.Status, 1);  // OR on bit 1 txd CAN HELLO data bit  
}
#define ACOK_flag     1
#define DCDC_PWM_Ena  1
#define OV_flag       1
#define OT_flag       1
#define Enable_cmd    1

// uint16_t DCDCVoltage, DCDCCurrent, Temperature;

/******************************************************************************/
/******************************************************************************/
// @author  Vince Keiper
// @brief   CN_PFLQRY Response handler. the request gets sent out from the
//			state machine.  Byte 0 of each CAN frame will contain the current
//          frame count.  This function will recieve multiple frames to get the
//			Iccp runtime profile that the PSU is running. If none is set, the 
//          PSU will return 0xff in Byte zero.  Byte 0 will contain the frame
//          count, Bytes 1-7 are the data payload.  If an 0xFF is received in
//          the frame count byte that means no profile is running and the
//          profile rxd bit must be CLEARED in the data struct.  The LCD
//          controller will always echo back the frame count byte ( Byte0 )
//          to the PSU to tell the PSU to send the next frame. 
//   
//  Example Profile data struct. Contains 27 bytes which will take 4 frames 
//  of 7 data byte ( 1 byte is the frame count )
//
//  typedef struct {
//			psu_mode_t  psu_mode;           // P_CV, P_CI, P_CPOT, P_OFF
//			int16_t     refelec_min_th;     // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
//			int16_t     refelec_max_th;     // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
//			uint16_t    rec_vset;           // resolution 1mv/bit 4096 = Full Scale
//			uint16_t    rec_vset_max_th;    // resolution 100mv/bit 4096 = Full Scale
//			uint16_t    rec_vset_min_th;    // resolution 100mv/bit
//			uint16_t    rec_fs_vset;        // resolution = vset_fs / 4096 ex 63V/4096 = 15mA/bit for 
                                            // a 54V rect. this would be the OV point or 63v= 630bits
//			uint16_t    rec_iset; 		    // resolution = iset_fs/4096 ex. 125A/4096 = 30mA/bit
//			uint16_t    rec_iset_max_th;    // resolution 1A/bit
//			uint16_t    rec_iset_min_th;    // resolution 1A/bit
//			uint16_t    rec_fs_iset;        // resolution 1A/bit 
//			uint16_t    alarm_reg_mask;     // 0=bit masked 1=active, same bit mapping as alarm register
//			uint16_t    self_test_int;      // interval self test is run at in minutes/bit 0= disbaled
//			uint8_t     adc_smp_int;        // interval adc samples are placed into 128 sample array 
                                            // mS/bit 0=uses real time values for debug purposes
//			uint8_t     control_int;        // interval the rectifer output current is updated based 
                                            // on the ref elec AVG buffers. second/bit 
//			uint32_t    tsts_active;        // bit mapped to each test in the self test routine, to 
                                            // allow us to select which tests will be run
//			uint8_t     refelec_active;     //
//} IccpPrfParam_t;                         // Iccp Profile Parameters

// @param   cmd, 8 deep uint8 buffer
// @param   id, uint32 CANID from PSU
// @retval  none
// 
/******************************************************************************/
void CANFunction_201(uint8_t *cmd, uint32_t id, uint8_t dlc)
{
	static uint8_t frm_cnt; // frame count received tfrom the PSU

    strcpy((char *)SysData.pflparams.refeleclongname[0], "BOW PORT");
    strcpy((char *)SysData.pflparams.refeleclongname[1], "BOW STAR BRD");
    strcpy((char *)SysData.pflparams.refeleclongname[2], "AFT PORT");
    strcpy((char *)SysData.pflparams.refeleclongname[3], "AFT STAR BRD");
	
	memset(canBuff, 0, 8);
	
    //  the initial request from the LCD has a data length of 0 so
    //  this seeds the frmcnt with zero
	if (dlc == 0)   
	{	
		frm_cnt = 0;
	}

    else 
    {
		frm_cnt = cmd[0];   // every response from LCD has the frame count in byte 0
		frm_cnt++;          // increment every time an echo from last frame recieved by the LCD
                            // controller. Thise sends the next frame in this callback.
	}
	
    canBuff[0] = frm_cnt;   // send frame count in byte 0
	datalength = 8;         // response will always be 8 bytes
	
	// Profile struct contains 27 bytes of data, we use 7 bytes per CAN frame
	// so we need 4 frames to get 27 bytes of data
	switch (frm_cnt)
	{
        case 0: // first frame contains 
                // psu_mode_t  psu_mode;         // P_CV, P_CI, P_CPOT, P_OFF
                // int16_t     refelec_min_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
                // int16_t     refelec_max_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
                // uint16_t    rec_vset_max_th;  // resolution 1mv/bit 4096 = Full Scale
                BITSET(SysData.Status, 2);  // OR on bit 2 rxd request for Profile data bit
                canBuff[1] = (psu_mode_t)SysData.pflparams.psu_mode;
                canBuff[2] = SysData.pflparams.refelec_min_th & 0x00FF;
                canBuff[3] = SysData.pflparams.refelec_min_th >> 8;
                canBuff[4] = SysData.pflparams.refelec_max_th & 0x00FF;
                canBuff[5] = SysData.pflparams.refelec_max_th >> 8;
                canBuff[6] = SysData.pflparams.rec_vset_max_th & 0x00FF;
                canBuff[7] = SysData.pflparams.rec_vset_max_th >> 8;
//                SysData.Status |= 0x0004; // OR on bit 2 rxd request for Profile data bit
        break;
			
        case 1: // Frame 2 contains....
                // uint16_t rec_vset_min_th;   // resolution 100mv/bit
                // uint16_t rec_fs_vset;       // .1V/bit if 54V rectifier this would be the OV point 
                                               // or 63v= 630bits
                // uint16_t rec_iset_max_th;   // resolution 100mA/bit
                // uint16_t rec_iset_min_th;   // resolution 100mA/bit  LOW BYTE ONLY, 
                                               // HI BYTE COMES IN NEXT FRAME
                canBuff[1] = SysData.pflparams.rec_vset_min_th & 0x00FF;
                canBuff[2] = SysData.pflparams.rec_vset_min_th >> 8;
                canBuff[3] = SysData.pflparams.rec_fs_vset  & 0x00FF;
                canBuff[4] = SysData.pflparams.rec_fs_vset  >> 8;
                canBuff[5] = SysData.pflparams.rec_iset_max_th & 0x00FF;
                canBuff[6] = SysData.pflparams.rec_iset_max_th >> 8;
                canBuff[7] = SysData.pflparams.rec_iset_min_th & 0x00FF;
        break;
			
        case 2: // Frame 3 contains....
                // uint16_t rec_iset_min_th; // resolution 100mA/bit  HI BYTE ONLY, LO BYTE WAS STUFFED IN LAST FRAME
                // uint16_t rec_fs_iset;     // .1A/bit if rectifier can do 125A at ILIM this would be the 125A= 1250 bits
                // uint16_t alarm_reg_mask;  // 0= bit masked 1= active, same bit mapping as alarm register
                // uint16_t self_test_int;   // interval self test is run at in minutes/bit 0= disbaled
                canBuff[1] = SysData.pflparams.rec_iset_min_th;
                canBuff[2] = SysData.pflparams.rec_fs_iset & 0x00FF;
                canBuff[3] = SysData.pflparams.rec_fs_iset >> 8;
                canBuff[4] = SysData.pflparams.alarm_reg_mask & 0x00FF;
                canBuff[5] = SysData.pflparams.alarm_reg_mask >> 8;
                canBuff[6] = SysData.pflparams.self_test_int & 0x00FF;
                canBuff[7] = SysData.pflparams.self_test_int >> 8;
        break;
			
        case 3: // Frame 4 contains....
                // uint8_t   adc_smp_int;  // interval adc samples are placed into 128 sample array 
                // mS/bit 0=uses real time values for debug purposes
                // uint8_t   control_int;  // interval the rectifer output current is updated based 
                // on the ref elec AVG buffers. second/bit 
                // uint32_t  tsts_active;  // bit mapped to each test in the self test routine, 
                // to allow us to select which tests will be run
                canBuff[1] = SysData.pflparams.adc_smp_int;
                canBuff[2] = SysData.pflparams.control_int;
                canBuff[3] = SysData.pflparams.tsts_active & 0x000000FF;
                canBuff[4] = SysData.pflparams.tsts_active >> 8 & 0x000000FF;
                canBuff[5] = SysData.pflparams.tsts_active >> 16 & 0x000000FF;
                canBuff[6] = SysData.pflparams.tsts_active >> 24 & 0x000000FF;
                canBuff[7] = SysData.pflparams.refelec_active;
        break;
		
        case 4: // Frame 5 contains...
                // int16_t  refelec_dmd_setpt          // resolution 1mv/bit -2048 = -0.5Vdc +2048 = +0.5Vdc    (2 bytes)
                // char     refeleclongname[4][16];    // refeleclongname[0][0] thru[2]                         (5 bytes)
                canBuff[1] = SysData.pflparams.refelec_dmd_setpt & 0x00FF;
                canBuff[2] = SysData.pflparams.refelec_dmd_setpt >> 8;
                canBuff[3] = SysData.pflparams.refeleclongname[0][0];
                canBuff[4] = SysData.pflparams.refeleclongname[0][1];
                canBuff[5] = SysData.pflparams.refeleclongname[0][2];
                canBuff[6] = SysData.pflparams.refeleclongname[0][3];
                canBuff[7] = SysData.pflparams.refeleclongname[0][4];
        break;

        case 5: // Frame 6 contains...
                // char     refeleclongname[4][16];     // refeleclongname[0][5] thru[11]   (7 bytes)
                canBuff[1] = SysData.pflparams.refeleclongname[0][5];
                canBuff[2] = SysData.pflparams.refeleclongname[0][6];
                canBuff[3] = SysData.pflparams.refeleclongname[0][7];
                canBuff[4] = SysData.pflparams.refeleclongname[0][8];
                canBuff[5] = SysData.pflparams.refeleclongname[0][9];
                canBuff[6] = SysData.pflparams.refeleclongname[0][10];
                canBuff[7] = SysData.pflparams.refeleclongname[0][11];
        break;

        case 6: // Frame 7 contains...
                // char     refeleclongname[4][16];     // refeleclongname[0][12] thru[15]  (4 bytes)
                // char     refeleclongname[4][16];     // refeleclongname[1][0] thru[2]    (3 bytes)
                canBuff[1] = SysData.pflparams.refeleclongname[0][12];
                canBuff[2] = SysData.pflparams.refeleclongname[0][13];
                canBuff[3] = SysData.pflparams.refeleclongname[0][14];
                canBuff[4] = SysData.pflparams.refeleclongname[0][15];
                canBuff[5] = SysData.pflparams.refeleclongname[1][0];
                canBuff[6] = SysData.pflparams.refeleclongname[1][1];
                canBuff[7] = SysData.pflparams.refeleclongname[1][2];
        break;

        case 7: // Frame 8 contains...
                // char     refeleclongname[4][16];     // refeleclongname[1][3] thru[9]    (7 bytes)
                canBuff[1] = SysData.pflparams.refeleclongname[1][3];
                canBuff[2] = SysData.pflparams.refeleclongname[1][4];
                canBuff[3] = SysData.pflparams.refeleclongname[1][5];
                canBuff[4] = SysData.pflparams.refeleclongname[1][6];
                canBuff[5] = SysData.pflparams.refeleclongname[1][7];
                canBuff[6] = SysData.pflparams.refeleclongname[1][8];
                canBuff[7] = SysData.pflparams.refeleclongname[1][9];
        break;

        case 8: // Frame 9 contains...
                // char     refeleclongname[4][16];     // refeleclongname[1][10] thru[15]  (6 bytes)
                // char     refeleclongname[4][16];     // refeleclongname[2][0]            (1 byte)
                canBuff[1] = SysData.pflparams.refeleclongname[1][10];
                canBuff[2] = SysData.pflparams.refeleclongname[1][11];
                canBuff[3] = SysData.pflparams.refeleclongname[1][12];
                canBuff[4] = SysData.pflparams.refeleclongname[1][13];
                canBuff[5] = SysData.pflparams.refeleclongname[1][14];
                canBuff[6] = SysData.pflparams.refeleclongname[1][15];
                canBuff[7] = SysData.pflparams.refeleclongname[2][0];
        break;

        case 9: // Frame 10 contains...
                // char     refeleclongname[4][16];     // refeleclongname[2][1] thru 7     (7 bytes)
                canBuff[1] = SysData.pflparams.refeleclongname[2][1];
                canBuff[2] = SysData.pflparams.refeleclongname[2][2];
                canBuff[3] = SysData.pflparams.refeleclongname[2][3];
                canBuff[4] = SysData.pflparams.refeleclongname[2][4];
                canBuff[5] = SysData.pflparams.refeleclongname[2][5];
                canBuff[6] = SysData.pflparams.refeleclongname[2][6];
                canBuff[7] = SysData.pflparams.refeleclongname[2][7];
        break;

        case 10: // Frame 11 contains...
                 // char    refeleclongname[4][16];     // refeleclongname[2][8] thru[14]   (7 bytes)
                canBuff[1] = SysData.pflparams.refeleclongname[2][8];
                canBuff[2] = SysData.pflparams.refeleclongname[2][9];
                canBuff[3] = SysData.pflparams.refeleclongname[2][10];
                canBuff[4] = SysData.pflparams.refeleclongname[2][11];
                canBuff[5] = SysData.pflparams.refeleclongname[2][12];
                canBuff[6] = SysData.pflparams.refeleclongname[2][13];
                canBuff[7] = SysData.pflparams.refeleclongname[2][14];
        break;

        case 11: // Frame 12 contains...
                 // char    refeleclongname[4][16];     // refeleclongname[2][15] thru[14]  (1 byte)
                 // char    refeleclongname[4][16];     // refeleclongname[3][0] thru[5]    (6 bytes)
                canBuff[1] = SysData.pflparams.refeleclongname[2][15];
                canBuff[2] = SysData.pflparams.refeleclongname[3][0];
                canBuff[3] = SysData.pflparams.refeleclongname[3][1];
                canBuff[4] = SysData.pflparams.refeleclongname[3][2];
                canBuff[5] = SysData.pflparams.refeleclongname[3][3];
                canBuff[6] = SysData.pflparams.refeleclongname[3][4];
                canBuff[7] = SysData.pflparams.refeleclongname[3][5];
        break;
        
        case 12: // Frame 13 contains...
                 // char    refeleclongname[4][16];     // refeleclongname[3][6] thru[12]   (7 bytes)
                canBuff[1] = SysData.pflparams.refeleclongname[3][6];
                canBuff[2] = SysData.pflparams.refeleclongname[3][7];
                canBuff[3] = SysData.pflparams.refeleclongname[3][8];
                canBuff[4] = SysData.pflparams.refeleclongname[3][9];
                canBuff[5] = SysData.pflparams.refeleclongname[3][10];
                canBuff[6] = SysData.pflparams.refeleclongname[3][11];
                canBuff[7] = SysData.pflparams.refeleclongname[3][12];
        break;

        case 13: // Frame 14 contains...
                 // char    refeleclongname[4][16];     // refeleclongname[3][13] thru[15]  (3 bytes)
                 // spare                                                                   (5 bytes)
                canBuff[1] = SysData.pflparams.refeleclongname[3][13];
                canBuff[2] = SysData.pflparams.refeleclongname[3][14];
                canBuff[3] = SysData.pflparams.refeleclongname[3][15];
                canBuff[4] = SysData.pflparams.rec_vset & 0x00FF;
                canBuff[5] = SysData.pflparams.rec_vset >> 8;
                canBuff[6] = SysData.pflparams.rec_iset & 0x00FF;
                canBuff[7] = SysData.pflparams.rec_iset >> 8;
//                SysData.Status |= 0x0008; // OR on bit 3 txd all Profile data bit
                BITCLR(SysData.Status, 2);
                BITSET(SysData.Status, 3);  // OR on bit 3 sent all profile data 
        break;

        case 0xFF:
                // If we get an 0xFF for the frame count and we did not get any other
                // successfully then the PSU is telling us it does not have a ICCP
                // profile programmed yet.
                SysData.Status &= 0xFFFB;
                
        default:
            return;
	}

    CANWrite(SysData.NODE_ADDR, SysData.DC_ADDR, CMD_CAN_201, canBuff, datalength);
}
/******************************************************************************/
/******************************************************************************/
// @author  Vince Keiper
// @brief   CN_STAQRY Response handler. the request gets sent out from the 
//			state machine.  Byte 0 of each CAN frame will contain the curent
//          frame count.  This function will recieve multiple frames to get 
//			all of the PSU status data which is 13 bytes. 
//		    Byte 0 will contain the frame count Bytes 1-7 are data payload.
//          The LCD controller will always echo back the frame count byte 
//			( Byte0 ) to the PSU to tell the PSU to send the next frame. 
//   
//  Example Profile data struct. Contains 27 bytes which will take 4 frames of
//  7 data byte ( 1 byte is the frame count )
//
//  typedef struct {
//	psu_mode_t  psu_mode;         // P_CV,P_CI,P_CPOT,P_OFF
//	int16_t     refelec_min_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
//	int16_t     refelec_max_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
//	uint16_t    rec_vset_max_th;  // resolution 1mv/bit 4096 = Full Scale
//	uint16_t    rec_vset_min_th;  // resolution 100mv/bit
//	uint16_t    rec_fs_vset;      // .1V/bit if 54V rectifier this would be the OV point or 63v= 630bits
//	uint16_t    rec_iset_max_th;  // resolution 100mA/bit
//	uint16_t    rec_iset_min_th;  // resolution 100mA/bit
//	uint16_t    rec_fs_iset;      // .1A/bit if rectifier can do 125A at ILIM this would be the 125A=1250 bits
//	uint16_t    alarm_reg_mask;   // 0= bit masked 1= active, same bit mapping as alarm register
//	uint16_t    self_test_int;    // interval self test is run at in minutes/bit 0= disbaled
//	uint8_t     adc_smp_int;      // interval adc samples are placed into 128 sample array mS/bit 
                                  // 0=uses real time values for debug purposes
//	uint8_t     control_int;      // interval the rectifer output current is updated based on the 
                                  // ref elec AVG buffers. second/bit 
//  uint32_t    tsts_active;      // bit mapped to each test in the self test routine, to allow us 
                                  // to select which tests will be run
//} IccpPrfParam_t;               // Iccp Profile Parameters

// @param cmd, 8 deep uint8 buffer
// @param id, uint32 CANID from PSU
// @retval none
// 
/******************************************************************************/
void CANFunction_202(uint8_t *cmd, uint32_t id, uint8_t dlc)
{ 
	static uint8_t frm_cnt; // frame count received from the LCD

    volatile int16_t vRealtime, iRealtime;
    volatile int16_t Realtime_RF0, Realtime_RF1, Realtime_RF2, Realtime_RF3;

    vRealtime =     (int16_t)((0.95462 * (SCALE_CV_RDBK(SysData.Vrdbk.collection[0]))) + 0.07823);
////    iRealtime =     (int16_t)((1.1551 * (SCALE_CI_RDBK(SysData.Irdbk[0].collection[0]))) + (-0.00000));    //// 1.02241, -0.42741
////    iRealtime +=    (int16_t)((1.1551 * (SCALE_CI_RDBK(SysData.Irdbk[1].collection[0]))) + (-0.00000)); 
        iRealtime = (int16_t)(1.03232 * ((double)(SCALE_CI_RDBK(SysData.Irdbk[0].collection[0]) + SCALE_CI_RDBK(SysData.Irdbk[1].collection[0]))) - 0.36229);


    Realtime_RF0 = SCALE_CPOT_RDBK(SysData.RefCell[0].collection[0]);
    Realtime_RF1 = SCALE_CPOT_RDBK(SysData.RefCell[1].collection[0]);
    Realtime_RF2 = SCALE_CPOT_RDBK(SysData.RefCell[2].collection[0]);
    Realtime_RF3 = SCALE_CPOT_RDBK(SysData.RefCell[3].collection[0]);

	memset(canBuff, 0, 8);
	
	if (dlc == 0) // the initial request from the LCD has a data length of 0 so this seeds the frmcnt with zero
	{	
        frm_cnt = 0;
	}
	
    else
    {
			frm_cnt = cmd[0];   // every response from LCD has the frame count in byte 0
			frm_cnt++;          // increment every time an echo from last frame recieved by the LCD
                                // controller. Thise sends the next frame in this callback.
	}
	
    canBuff[0] = frm_cnt;   // send frame count in byte 0
	datalength = 8;         // response will always be 8 bytes
	
	// A PSU status contains 15 bytes of data, each CAN frame has one byte for frame count
	// and 7 bytes of data payload per CAN frame
	// So we need 3 frames to get 15 bytes of data
	switch (frm_cnt)
	{
        case 0: // first frame contains 
                BITSET(SysData.Status, 4);  // OR ON bit 4 rxd cn_staqry
                canBuff[1]      = vRealtime;
                canBuff[2]      = vRealtime >> 8;
                canBuff[3]      = iRealtime;
                canBuff[4]      = iRealtime >> 8;
                canBuff[5]      = SysData.AlmReg;
                canBuff[6]      = SysData.AlmReg >> 8;
                canBuff[7]      = SysData.Temperature;
        break;
			
                case 1: // Frame 2 contains....
                canBuff[1]      = Realtime_RF0;
                canBuff[2]      = Realtime_RF0 >> 8;
                canBuff[3]      = Realtime_RF1;
                canBuff[4]      = Realtime_RF1 >> 8;
                canBuff[5]      = Realtime_RF2;
                canBuff[6]      = Realtime_RF2 >> 8;
        break;
						
        case 2: // Frame 3 contains....
                canBuff[1]      = Realtime_RF3;
                canBuff[2]      = Realtime_RF3 >> 8;
                BITCLR(SysData.Status, 4);
                BITSET(SysData.Status, 5);  // OR ON bit 5 txd all status query data cn_staqry
        break;
    
        default:
            return;
	}
	
    CANWrite(SysData.NODE_ADDR, SysData.DC_ADDR, CMD_CAN_202, canBuff, datalength);
}
/******************************************************************************/
// @author  Vince Keiper
// @brief   CN_PFLDNLD_FLASH. CANbus command, Iccp Profile download from LCD
//          ctl to the PSU ctl flash.  This will  be saved in the PSUs flash 
//          and recalled upon power up.  The PSU selected from the LCD GUI will
//          determine the PSU address the request is sent to. And that in turn
//          will determine which PSU answers the command and the lclpsuidx
//          variable will be set in here based on that.  So when sending the 
//          203 cmd request you must pass the correct PSU address at that time.
//          Request sent to PSU from the LCD gui, or TCP, HTTP server.  
//          The request will go out with NO DATA DLC=0, the PSU will respond
//          with the frame count byte of zero.  That will kick off the
//          transmission of the remaining frames because the 203 response handler.
//		    Byte 0 of each CAN frame will contain the current frame count.
//
//          This function will have to transmit multiple frames to get the
//			entire Iccp profile to the PSU. The PSU must echo back the frame
//          count to ACK it received the data. This will trigger the LCD
//          controller to send the next frame. If no response from the PSU
//          within 5 seconds the command will timeout and go into COM fault.
//          As discussed, Byte 0 will contain the frame count Bytes 1-7
//          are the data payload.
//        
//  Example Profile data struct. Contains 27 bytes which will take 4 frames 
//  with 7 data bytes in each frame (Again, these CANbus frames can have 8
//  bytes of data but we use one byte for the frame count)
//
//  typedef struct {
//			psu_mode_t  psu_mode;         // P_CV, P_CI, P_CPOT, P_OFF
//			int16_t     refelec_min_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
//			int16_t     refelec_max_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
//			uint16_t    rec_vset;         // resolution 1mv/bit 4096 = Full Scale
//			uint16_t    rec_vset_max_th;  // resolution 100mv/bit 4096 = Full Scale
//			uint16_t    rec_vset_min_th;  // resolution 100mv/bit
//			uint16_t    rec_fs_vset;      // resolution = vset_fs / 4096 ex 63V/4096 = 15mA/bit 
                                          // for a 54V rect. this would be the OV point or 63v= 630bits
//			uint16_t    rec_iset;         // resolution = iset_fs/4096 ex. 125A/4096 = 30mA/bit
//			uint16_t    rec_iset_max_th;  // resolution 1A/bit
//			uint16_t    rec_iset_min_th;  // resolution 1A/bit
//			uint16_t    rec_fs_iset;      // resolution 1A/bit 
//			uint16_t    alarm_reg_mask;   // 0= bit masked 1= active, same bit mapping as alarm register
//			uint16_t    self_test_int;    // interval self test is run at in minutes/bit 0= disbaled
//			uint8_t     adc_smp_int;      // interval adc samples are placed into 128 sample array mS/bit 
                                          // 0=uses real time values for debug purposes
//			uint8_t     control_int;      // interval the rectifer output current is updated based on the 
                                          // ref elec AVG buffers. second/bit 
//			uint32_t    tsts_active;      // bit mapped to each test in the self test routine, to allow us 
                                          // to select which tests will be run
//			uint8_t     refelec_active;   //
//} IccpPrfParam_t;                       // Iccp Profile Parameters

// @param cmd, 8 deep uint8 buffer
// @param id, uint32 CANID from PSU
// @retval none
// 
/******************************************************************************/
void CANFunction_203(uint8_t *cmd, uint32_t id, uint8_t dlc)
{
	static uint8_t frm_cnt;// frame count received tfrom the PSU
	
	memset(canBuff, 0, 8);
	
//	if (dlc == 0)// the initial request from the LCD has a data length of 0 so this seeds the frmcnt with zero
//	{	
//        frm_cnt = 0;
//	}
//	
//    else
//    {
//        frm_cnt = cmd[0];   // every response from LCD has the frame count in byte 0
//        frm_cnt++;          // increment every time an echo from last frame recieved by the LCD
//                            // controller. Thise sends the next frame in this callback.
//	}
	
    canBuff[0] = frm_cnt;       // send frame count in byte 0
	
	datalength = 8;             // we will always send 8 bytes

    if (dlc == 0)
    {
        datalength = 0;
        BITSET(SysData.Status, 2);      // OR on bit 2 rxd request for Profile data bit  
    }
    
    else
    {
        datalength   = 1;       // echo will only be 1 byte, the frame count
        frm_cnt      = cmd[0];  // byte 0 from PSU always contains frame count 
        canBuff[0]   = frm_cnt;
	
        // Profile struct contains 27 bytes of data, we use 7 bytes per CAN frame
        // so we need 4 frames to get 27 bytes of data
	
        switch (frm_cnt)
        {
            case 0: //  first frame contains 
                    //	psu_mode_t  psu_mode;         // P_CV, P_CI, P_CPOT, P_OFF
                    //	int16_t     refelec_min_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
                    //	int16_t     refelec_max_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
                    //	uint16_t    rec_vset_max_th;  // resolution 1mv/bit 4096 = Full Scale
                    SysData.pflparams.psu_mode        = (psu_mode_t)cmd[1];
                    SysData.pflparams.refelec_min_th  = cmd[2] + (cmd[3] * 0x100);
                    SysData.pflparams.refelec_max_th  = cmd[4] + (cmd[5] * 0x100);
                    SysData.pflparams.rec_vset_max_th = cmd[6] + (cmd[7] * 0x100);
                    // SysData.Status |= 0x0004;    // OR on bit 2 rxd request for Profile data bit
        //          BITSET(SysData.Status, 2);      // OR on bit 2 rxd request for Profile data bit
            break;
			
            case 1: //  Frame 2 contains....
                //	uint16_t  rec_vset_min_th;  // resolution 100mv/bit
                //	uint16_t  rec_fs_vset;      // .1V/bit if 54V rectifier this would be the OV point or 63v= 630bits
                //	uint16_t  rec_iset_max_th;  // resolution 100mA/bit
                //	uint16_t  rec_iset_min_th;  // resolution 100mA/bit  LOW BYTE ONLY, HI BYTE COMES IN NEXT FRAME
                SysData.pflparams.rec_vset_min_th  = cmd[1] + (cmd[2] * 0x100);
                SysData.pflparams.rec_fs_vset      = cmd[3] + (cmd[4] * 0x100);
                SysData.pflparams.rec_iset_max_th  = cmd[5] + (cmd[6] * 0x100);
                SysData.pflparams.rec_iset_min_th  = cmd[7] & 0x00ff; 
            break;
			
            case 2: // Frame 3 contains....
                //	uint16_t rec_iset_min_th; // resolution 100mA/bit  HI BYTE ONLY, LO BYTE WAS STUFFED IN LAST FRAME
                //	uint16_t rec_fs_iset;     // .1A/bit if rectifier can do 125A at ILIM this would be the 125A= 1250 bits
                //	uint16_t alarm_reg_mask;  // 0= bit masked 1= active, same bit mapping as alarm register
                //	uint16_t self_test_int;   // interval self test is run at in minutes/bit 0= disbaled
                SysData.pflparams.rec_iset_min_th  = (cmd[1] * 0x100);
                SysData.pflparams.rec_fs_iset      = cmd[2] + (cmd[3] * 0x100);
                SysData.pflparams.alarm_reg_mask   = cmd[4] + (cmd[5] * 0x100);
                SysData.pflparams.self_test_int    = cmd[6] + (cmd[7] * 0x100);
            break;
			
            case 3: // Frame 4 contains....
                //	uint8_t adc_smp_int;      // interval adc samples are placed into 128 sample array mS/bit 
                                              // 0=uses real time values for debug purposes
                //	uint8_t control_int;      // interval the rectifer output current is updated based on the 
                                              // ref elec AVG buffers. second/bit 
                //  uint32_t tsts_active;     // bit mapped to each test in the self test routine, to allow 
                                              // us to select which tests will be run
                SysData.pflparams.adc_smp_int = cmd[1];
                SysData.pflparams.control_int = cmd[2];
                SysData.pflparams.tsts_active = cmd[3] + (cmd[4] * 0x100) + (cmd[5] * 0x10000) + (cmd[6] * 0x1000000);
				SysData.pflparams.refelec_active = cmd[7];	
                // TODO: Need to reload the RAM rpofile and resend to the LCD to ensure the LCD knows what proifile is running
                // SysData.Status |= 0x0004;  // OR ON bit 2 "Profile RXD bit"
            break;

            case 4: // Frame 5 contains...
                // int16_t  refelec_dmd_setpt;      // resolution 1mV/bit -2048 = -0.5Vdc + 2048 = +0.5Vd.  (2 bytes)
                // char     refeleclongname[4][16]; // refeleclong[0][0] thru[4]                            (5 bytes)
                SysData.pflparams.refelec_dmd_setpt     = cmd[1] + (cmd[2] * 0x100);
                SysData.pflparams.refeleclongname[0][0] = cmd[3];
                SysData.pflparams.refeleclongname[0][1] = cmd[4];
                SysData.pflparams.refeleclongname[0][2] = cmd[5];
                SysData.pflparams.refeleclongname[0][3] = cmd[6];
                SysData.pflparams.refeleclongname[0][4] = cmd[7];
            break;

            case 5: // Frame 6 contains...
                // char     refeleclongname[4][16]; // refeleclongname[0][5] thur[11]                       (7 bytes)
                SysData.pflparams.refeleclongname[0][5]  = cmd[1];
                SysData.pflparams.refeleclongname[0][6]  = cmd[2];
                SysData.pflparams.refeleclongname[0][7]  = cmd[3];
                SysData.pflparams.refeleclongname[0][8]  = cmd[4];
                SysData.pflparams.refeleclongname[0][9]  = cmd[5];
                SysData.pflparams.refeleclongname[0][10] = cmd[6];
                SysData.pflparams.refeleclongname[0][11] = cmd[7];
            break;

            case 6: // Frame 7 contains...
                // char     refeleclongname[4][16]; // refeleclongname[0][12] thru[15]                      (4 bytes)
                // char     refeleclongname[4][16]; // refeleclongname[1][0] thru[2]                        (3 bytes)
                SysData.pflparams.refeleclongname[0][12]  = cmd[1];
                SysData.pflparams.refeleclongname[0][13]  = cmd[2];
                SysData.pflparams.refeleclongname[0][14]  = cmd[3];
                SysData.pflparams.refeleclongname[0][15]  = cmd[4];
                SysData.pflparams.refeleclongname[1][0]   = cmd[5];
                SysData.pflparams.refeleclongname[1][1]   = cmd[6];
                SysData.pflparams.refeleclongname[1][2]   = cmd[7];
            break;

            case 7: // Frame 8 contains...
                // char     refeleclongname[4][16]; // refeleclongname[1][3] thru[9]                        (7 bytes)
                SysData.pflparams.refeleclongname[1][3]  = cmd[1];
                SysData.pflparams.refeleclongname[1][4]  = cmd[2];
                SysData.pflparams.refeleclongname[1][5]  = cmd[3];
                SysData.pflparams.refeleclongname[1][6]  = cmd[4];
                SysData.pflparams.refeleclongname[1][7]  = cmd[5];
                SysData.pflparams.refeleclongname[1][8]  = cmd[6];
                SysData.pflparams.refeleclongname[1][9]  = cmd[7];
            break;

            case 8: // Frame 9 contains...
                // char     refeleclongname[4][16]; // refeleclongname[1][10] thru[15]                      (6 bytes)
                // char     refeleclongname[4][16]; // refeleclongname[2][0]                                (1 byte)
                SysData.pflparams.refeleclongname[1][10] = cmd[1];
                SysData.pflparams.refeleclongname[1][11] = cmd[2];
                SysData.pflparams.refeleclongname[1][12] = cmd[3];
                SysData.pflparams.refeleclongname[1][13] = cmd[4];
                SysData.pflparams.refeleclongname[1][14] = cmd[5];
                SysData.pflparams.refeleclongname[1][15] = cmd[6];
                SysData.pflparams.refeleclongname[2][0]  = cmd[7];
            break;

            case 9: // Frame 10 contains...
                // char     refeleclongname[4][16]; // refeleclongname[2][1] thru[7]                        (7 bytes)
                SysData.pflparams.refeleclongname[2][1] = cmd[1];
                SysData.pflparams.refeleclongname[2][2] = cmd[2];
                SysData.pflparams.refeleclongname[2][3] = cmd[3];
                SysData.pflparams.refeleclongname[2][4] = cmd[4];
                SysData.pflparams.refeleclongname[2][5] = cmd[5];
                SysData.pflparams.refeleclongname[2][6] = cmd[6];
                SysData.pflparams.refeleclongname[2][7] = cmd[7];
            break;

            case 10: // Frame 11 contains...
                 // char    refeleclongname[4][16]; // refeleclongname[2][8] thru[14]                       (7 bytes)
                SysData.pflparams.refeleclongname[2][8]  = cmd[1];
                SysData.pflparams.refeleclongname[2][9]  = cmd[2];
                SysData.pflparams.refeleclongname[2][10] = cmd[3];
                SysData.pflparams.refeleclongname[2][11] = cmd[4];
                SysData.pflparams.refeleclongname[2][12] = cmd[5];
                SysData.pflparams.refeleclongname[2][13] = cmd[6];
                SysData.pflparams.refeleclongname[2][14] = cmd[7];
            break;
        
            case 11: // Frame 12 contains...
                 // char    refeleclongname[4][16]; // refeleclongname[2][15]                               (1 byte)
                 // char    refeleclongname[4][16]; // refeleclongname[3][0] thur[5]                        (6 bytes)
                SysData.pflparams.refeleclongname[2][15] = cmd[1];
                SysData.pflparams.refeleclongname[3][0]  = cmd[2];
                SysData.pflparams.refeleclongname[3][1]  = cmd[3];
                SysData.pflparams.refeleclongname[3][2]  = cmd[4];
                SysData.pflparams.refeleclongname[3][3]  = cmd[5];
                SysData.pflparams.refeleclongname[3][4]  = cmd[6];
                SysData.pflparams.refeleclongname[3][5]  = cmd[7];                 
            break;

            case 12: // Frame 13 contains...
                 // char    refeleclongname[4][16]; // refeleclongname[3][6] thru[12]                       (7 bytes)
                SysData.pflparams.refeleclongname[3][6]  = cmd[1];
                SysData.pflparams.refeleclongname[3][7]  = cmd[2];
                SysData.pflparams.refeleclongname[3][8]  = cmd[3];
                SysData.pflparams.refeleclongname[3][9]  = cmd[4];
                SysData.pflparams.refeleclongname[3][10] = cmd[5];
                SysData.pflparams.refeleclongname[3][11] = cmd[6];
                SysData.pflparams.refeleclongname[3][12] = cmd[7];
            break;

            case 13: // Frame 14 contains...
                 // char    refeleclongname[4][16]; // refeleclongname[3][13] thru[15]                       (3 bytes)
                 // spares                                                                                   (5 bytes)
                SysData.pflparams.refeleclongname[3][13]    = cmd[1];
                SysData.pflparams.refeleclongname[3][14]    = cmd[2];
                SysData.pflparams.refeleclongname[3][15]    = cmd[3];
                SysData.pflparams.rec_vset                  = cmd[4] + (cmd[5] * 100);
                SysData.pflparams.rec_iset                  = cmd[6] + (cmd[7] * 100);
                SysData.Status &= 0xFFFB;   // 0b1111 1111 1111 1011 // AND off bit 2 rxd request for Profile data bit 
                WriteProfileStructToFlash(&SysData.pflparams, 0);
                ReadProfileStructFromFlash(&SysData.pflparams, 0);
                BITSET(SysData.Status, 3);  // OR ON bit 3 "Profile RXD bit"
            break;

            case 0xFF:
                // if we get an 0xff for the frame count and we did not get any other frames successully then
                // the PSU is telling us it doe s not have a ICCP profile programmed yet.
                // SysData.Status &= 0xFFF3; // AND OFF bit 2 & 3 "Profile RXD bit" && all profile data sent bit
                BITSET(SysData.Status, 0);
                BITSET(SysData.Status, 1);
                BITCLR(SysData.Status, 2);
                BITCLR(SysData.Status, 3);
				
            break;

            default:
                return;
        }
    }
    
    CANWrite(SysData.NODE_ADDR, SysData.DC_ADDR, CMD_CAN_203, canBuff, datalength);	
}
/******************************************************************************/
// @author  Vince Keiper
// @brief   CN_PFLDNLD_RAM. CANbus command, Iccp Profile download from LCD ctl
//          to the PSU ctl RAM.  This will be saved in the PSUs RAM and NOT be
//          recalled upon power up.  If PSU is powered down and back up it will
//          recall the profile saved in flash.  The PSU selected from the LCD
//          GUI will determine the PSU address the request is sent to. And that
//          in turn will determine which PSU answers the command and the 
//          lclpsuidx variable will be set in here based on that.  So when
//          sending the 203 cmd request you must pass the correct PSU address
//          at that time.  Request sent to PSU from the LCD gui, or TCP, HTTP
//          server.  The request will go out with NO DATA DLC=0, the PSU will
//          respond with the frame count byte of zero.  That will kick off the
//          transmission of the remaining frames because the 203 response
//          handler.  Byte 0 of each CAN frame will contain the current 
//          frame count.
//
//          This function will have to transmit multiple frames to get the 
//          entire iccp profile to the PSU. The PSU must echo back the frame
//          count to ACK it received the data. This will trigger the LCD
//          controller to send the next frame. If no response from the PSU
//          within 5 seconds the command will timeout and go into COM fault.
//		    As discussed Byte 0 will contain the frame count Bytes 1-7 
//          are the data payload.
//        
//  Example Profile data struct. Contains 27 bytes which will take 4 frames 
//  with 7 data bytes in each frame (Again, these CANbus frames can have 
//  8 bytes of data but we use one byte for the frame count)
//
//  typedef struct {
//			psu_mode_t  psu_mode;         // P_CV, P_CI, P_CPOT, P_OFF
//			int16_t     refelec_min_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
//			int16_t     refelec_max_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
//			uint16_t    rec_vset;         // resolution 1mv/bit 4096 = Full Scale
//			uint16_t    rec_vset_max_th;  // resolution 100mv/bit 4096 = Full Scale
//			uint16_t    rec_vset_min_th;  // resolution 100mv/bit
//			uint16_t    rec_fs_vset;      // resolution = vset_fs / 4096 ex 63V/4096 = 15mA/bit for a 54V rect. 
                                          // this would be the OV point or 63v= 630bits
//			uint16_t    rec_iset; 	      // resolution = iset_fs/4096 ex. 125A/4096 = 30mA/bit
//			uint16_t    rec_iset_max_th;  // resolution 1A/bit
//			uint16_t    rec_iset_min_th;  // resolution 1A/bit
//			uint16_t    rec_fs_iset;      // resolution 1A/bit 
//			uint16_t    alarm_reg_mask;   // 0= bit masked 1= active, same bit mapping as alarm register
//			uint16_t    self_test_int;    // interval self test is run at in minutes/bit 0= disbaled
//			uint8_t     adc_smp_int;      // interval adc samples are placed into 128 sample array mS/bit 0=uses 
                                          // real time values for debug purposes
//			uint8_t     control_int;      // interval the rectifer output current is updated based on the ref elec
                                          // AVG buffers. second/bit 
//			uint32_t    tsts_active;      // bit mapped to each test in the self test routine, to allow us 
                                          // to select which tests will be run
//			uint8_t     refelec_active;   //
//} IccpPrfParam_t;                       // Iccp Profile Parameters

// @param cmd, 8 deep uint8 buffer
// @param id, uint32 CANID from PSU
// @retval none
// 
/******************************************************************************/
void CANFunction_204(uint8_t *cmd, uint32_t id, uint8_t dlc)
{
	static uint8_t frm_cnt;// frame count received tfrom the PSU
	
	memset(canBuff, 0, 8);
	
//	if (dlc == 0) // the initial request from the LCD has a data length of 0 so this seeds the frmcnt with zero
//	{	
//			frm_cnt = 0;
//	}
//	
//  else 
//  {
//        frm_cnt = cmd[0];   // every response from LCD has the frame count in byte 0
//        frm_cnt++;          // increment every time an echo from last frame recieved by the LCD
//                                // controller. Thise sends the next frame in this callback.
//	}
	
		
	// Profile struct contains 27 bytes of data, we use 7 bytes per CAN frame
	// so we need 4 frames to get 27 bytes of data

    if (dlc == 0)
    {
        datalength = 0;
        BITSET(SysData.Status, 2);      // OR on bit 2 rxd request for Profile data bit  
    }
    
    else
    {
        datalength   = 1;       // echo will only be 1 byte, the frame count
        frm_cnt      = cmd[0];  // byte 0 from PSU always contains frame count 
        canBuff[0]   = frm_cnt;
    
        switch (frm_cnt)
        {
//            case 0:
//                datalength  = 0;
//                BITSET(SysData.Status, 2);      // OR on bit 2 rxd request for Profile data bit
//            break;

            case 0: // first frame contains 
                // psu_mode_t psu_mode;         // P_CV, P_CI, P_CPOT, P_OFF
                // int16_t refelec_min_th;      // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
                // int16_t refelec_max_th;      // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
                // uint16_t rec_vset_max_th;    // resolution 1mv/bit 4096 = Full Scale
                SysData.pflparams.psu_mode        = (psu_mode_t)cmd[1];
                SysData.pflparams.refelec_min_th  = cmd[2] + (cmd[3] * 0x100);
                SysData.pflparams.refelec_max_th  = cmd[4] + (cmd[5] * 0x100);
                SysData.pflparams.rec_vset_max_th = cmd[6] + (cmd[7] * 0x100);
//                SysData.Status |= 0x0004;       // OR on bit 2 rxd request for Profile data bit
                
            break;
			
            case 1: //  Frame 2 contains....
                //	uint16_t  rec_vset_min_th;  // resolution 100mv/bit
                //	uint16_t  rec_fs_vset;      // .1V/bit if 54V rectifier this would be the OV point or 63v= 630bits
                //	uint16_t  rec_iset_max_th;  // resolution 100mA/bit
                //	uint16_t  rec_iset_min_th;  // resolution 100mA/bit  LOW BYTE ONLY, HI BYTE COMES IN NEXT FRAME
                SysData.pflparams.rec_vset_min_th  = cmd[1] + (cmd[2] * 0x100);
                SysData.pflparams.rec_fs_vset      = cmd[3] + (cmd[4] * 0x100);
                SysData.pflparams.rec_iset_max_th  = cmd[5] + (cmd[6] * 0x100);
                SysData.pflparams.rec_iset_min_th  = cmd[7] & 0x00ff; 
            break;
			
            case 2: // Frame 3 contains....
                //	uint16_t rec_iset_min_th; // resolution 100mA/bit  HI BYTE ONLY, LO BYTE WAS STUFFED IN LAST FRAME
                //	uint16_t rec_fs_iset;     // .1A/bit if rectifier can do 125A at ILIM this would be the 125A= 1250 bits
                //	uint16_t alarm_reg_mask;  // 0= bit masked 1= active, same bit mapping as alarm register
                //	uint16_t self_test_int;   // interval self test is run at in minutes/bit 0= disbaled
                SysData.pflparams.rec_iset_min_th  = (cmd[1] * 0x100);
                SysData.pflparams.rec_fs_iset      = cmd[2] + (cmd[3] * 0x100);
                SysData.pflparams.alarm_reg_mask   = cmd[4] + (cmd[5] * 0x100);
                SysData.pflparams.self_test_int    = cmd[6] + (cmd[7] * 0x100);
            break;
			
            case 3: //  Frame 4 contains....
                //	uint8_t adc_smp_int;    // interval adc samples are placed into 128 sample array mS/bit 0=uses 
                                            // real time values for debug purposes
                //	uint8_t control_int;    // interval the rectifer output current is updated based on the ref elec 
                                            // AVG buffers. second/bit 
                //  uint32_t tsts_active;   // bit mapped to each test in the self test routine, to allow us to 
                                            // select which tests will be run
                SysData.pflparams.adc_smp_int    = cmd[1];
                SysData.pflparams.control_int    = cmd[2];
                SysData.pflparams.tsts_active    = cmd[3] + (cmd[4] * 0x100) + (cmd[5] * 0x10000) + (cmd[6] * 0x1000000);
                SysData.pflparams.refelec_active = cmd[7];
                // TODO: Need to resend to the LCD to ensure the LCD knows what proifile is running
                // SysData.Status |= 0x0004;// OR ON bit 2 "Profile RXD bit"
            break;
		
            case 4: // Frame 5 contains...
                // int16_t  refelec_dmd_setpt;      // resolution 1mV/bit -2048 = -0.5Vdc + 2048 = +0.5Vd.  (2 bytes)
                // char     refeleclongname[4][16]; // refeleclong[0][0] thru[4]                            (5 bytes)
                SysData.pflparams.refelec_dmd_setpt     = cmd[1] + (cmd[2] * 0x100);
                SysData.pflparams.refeleclongname[0][0] = cmd[3];
                SysData.pflparams.refeleclongname[0][1] = cmd[4];
                SysData.pflparams.refeleclongname[0][2] = cmd[5];
                SysData.pflparams.refeleclongname[0][3] = cmd[6];
                SysData.pflparams.refeleclongname[0][4] = cmd[7];
            break;

            case 5: // Frame 6 contains...
                // char     refeleclongname[4][16]; // refeleclongname[0][5] thur[11]                       (7 bytes)
                SysData.pflparams.refeleclongname[0][5]  = cmd[1];
                SysData.pflparams.refeleclongname[0][6]  = cmd[2];
                SysData.pflparams.refeleclongname[0][7]  = cmd[3];
                SysData.pflparams.refeleclongname[0][8]  = cmd[4];
                SysData.pflparams.refeleclongname[0][9]  = cmd[5];
                SysData.pflparams.refeleclongname[0][10] = cmd[6];
                SysData.pflparams.refeleclongname[0][11] = cmd[7];
            break;

            case 6: // Frame 7 contains...
                // char     refeleclongname[4][16]; // refeleclongname[0][12] thru[15]                      (4 bytes)
                // char     refeleclongname[4][16]; // refeleclongname[1][0] thru[2]                        (3 bytes)
                SysData.pflparams.refeleclongname[0][12]  = cmd[1];
                SysData.pflparams.refeleclongname[0][13]  = cmd[2];
                SysData.pflparams.refeleclongname[0][14]  = cmd[3];
                SysData.pflparams.refeleclongname[0][15]  = cmd[4];
                SysData.pflparams.refeleclongname[1][0]   = cmd[5];
                SysData.pflparams.refeleclongname[1][1]   = cmd[6];
                SysData.pflparams.refeleclongname[1][2]   = cmd[7];
            break;

            case 7: // Frame 8 contains...
                // char     refeleclongname[4][16]; // refeleclongname[1][3] thru[9]                        (7 bytes)
                SysData.pflparams.refeleclongname[1][3]  = cmd[1];
                SysData.pflparams.refeleclongname[1][4]  = cmd[2];
                SysData.pflparams.refeleclongname[1][5]  = cmd[3];
                SysData.pflparams.refeleclongname[1][6]  = cmd[4];
                SysData.pflparams.refeleclongname[1][7]  = cmd[5];
                SysData.pflparams.refeleclongname[1][8]  = cmd[6];
                SysData.pflparams.refeleclongname[1][9]  = cmd[7];
            break;

            case 8: // Frame 9 contains...
                // char     refeleclongname[4][16]; // refeleclongname[1][10] thru[15]                      (6 bytes)
                // char     refeleclongname[4][16]; // refeleclongname[2][0]                                (1 byte)
                SysData.pflparams.refeleclongname[1][10] = cmd[1];
                SysData.pflparams.refeleclongname[1][11] = cmd[2];
                SysData.pflparams.refeleclongname[1][12] = cmd[3];
                SysData.pflparams.refeleclongname[1][13] = cmd[4];
                SysData.pflparams.refeleclongname[1][14] = cmd[5];
                SysData.pflparams.refeleclongname[1][15] = cmd[6];
                SysData.pflparams.refeleclongname[2][0] = cmd[7];
            break;

            case 9: // Frame 10 contains...
                // char     refeleclongname[4][16]; // refeleclongname[2][1] thru[7]                        (7 bytes)
                SysData.pflparams.refeleclongname[2][1] = cmd[1];
                SysData.pflparams.refeleclongname[2][2] = cmd[2];
                SysData.pflparams.refeleclongname[2][3] = cmd[3];
                SysData.pflparams.refeleclongname[2][4] = cmd[4];
                SysData.pflparams.refeleclongname[2][5] = cmd[5];
                SysData.pflparams.refeleclongname[2][6] = cmd[6];
                SysData.pflparams.refeleclongname[2][7] = cmd[7];
            break;

            case 10: // Frame 11 contains...
                 // char    refeleclongname[4][16]; // refeleclongname[2][8] thru[14]                       (7 bytes)
                SysData.pflparams.refeleclongname[2][8]  = cmd[1];
                SysData.pflparams.refeleclongname[2][9]  = cmd[2];
                SysData.pflparams.refeleclongname[2][10] = cmd[3];
                SysData.pflparams.refeleclongname[2][11] = cmd[4];
                SysData.pflparams.refeleclongname[2][12] = cmd[5];
                SysData.pflparams.refeleclongname[2][13] = cmd[6];
                SysData.pflparams.refeleclongname[2][14] = cmd[7];
            break;
        
            case 11: // Frame 12 contains...
                 // char    refeleclongname[4][16]; // refeleclongname[2][15]                               (1 byte)
                 // char    refeleclongname[4][16]; // refeleclongname[3][0] thur[5]                        (6 bytes)
                SysData.pflparams.refeleclongname[2][15] = cmd[1];
                SysData.pflparams.refeleclongname[3][0]  = cmd[2];
                SysData.pflparams.refeleclongname[3][1]  = cmd[3];
                SysData.pflparams.refeleclongname[3][2]  = cmd[4];
                SysData.pflparams.refeleclongname[3][3]  = cmd[5];
                SysData.pflparams.refeleclongname[3][4]  = cmd[6];
                SysData.pflparams.refeleclongname[3][5]  = cmd[7];                 
            break;

            case 12: // Frame 13 contains...
                 // char    refeleclongname[4][16]; // refeleclongname[3][6] thru[12]                       (7 bytes)
                SysData.pflparams.refeleclongname[3][6]  = cmd[1];
                SysData.pflparams.refeleclongname[3][7]  = cmd[2];
                SysData.pflparams.refeleclongname[3][8]  = cmd[3];
                SysData.pflparams.refeleclongname[3][9]  = cmd[4];
                SysData.pflparams.refeleclongname[3][10] = cmd[5];
                SysData.pflparams.refeleclongname[3][11] = cmd[6];
                SysData.pflparams.refeleclongname[3][12] = cmd[7];
            break;

            case 13: // Frame 14 contains...
                 // char    refeleclongname[4][16]; // refeleclongname[3][13] thru[15]                       (3 bytes)
                 // spares                                                                                   (5 bytes)
                SysData.pflparams.refeleclongname[3][13]  = cmd[1];
                SysData.pflparams.refeleclongname[3][14]  = cmd[2];
                SysData.pflparams.refeleclongname[3][15]  = cmd[3];
                SysData.pflparams.rec_vset                = cmd[4] + (cmd[5] * 0x100);
                SysData.pflparams.rec_iset                = cmd[6] + (cmd[7] * 0x100);
//                SysData.Status &= 0xFFFB;   // 0b1111 1111 1111 1011 // AND off bit 2 rxd request for Profile data bit 
                BITCLR(SysData.Status, 2);
                BITSET(SysData.Status, 3); // OR ON bit 3 "Profile RXD bit"
            break;
    
            case 0xFF:
                // if we get an 0xff for the frame count and we did not get any other frames successully then
                // the PSU is telling us it doe s not have a ICCP profile programmed yet.
//                SysData.Status &= 0xFFF3;// AND OFF bit 2 & 3 "Profile RXD bit" && all profile data sent bit
                BITSET(SysData.Status, 0);
                BITSET(SysData.Status, 1);
                BITCLR(SysData.Status, 2);
                BITCLR(SysData.Status, 3);
                break;
      
            default:
                return;
	}
    
  }
    CANWrite(SysData.NODE_ADDR,SysData.DC_ADDR,CMD_CAN_204, canBuff, datalength);
}
/******************************************************************************/
/******************************************************************************/
// @author  Vince Keiper
// @brief   CN_SELFTEST_RQST Self Test Request. the request gets sent out from the 
//			LCD when Button pressed
//
// @param cmd, 8 deep uint8 buffer
// @param id, uint32 CANID from PSU
// @retval none
// 
/******************************************************************************/
void CANFunction_205(uint8_t *cmd, uint32_t id, uint8_t dlc)
{ 
	unsigned char   test_rq = cmd[0];   // contains request type
    
    if (test_rq & 0x01)
    {
        if (AppState == RUNSELFTEST)
        {
            __NOP();
        }
        else
        {
            AppState = RUNSELFTEST;
        }
    }
    else
    {   /* Closing if/else */
        __NOP();
    }
}
/******************************************************************************/
/******************************************************************************/
// @author  Vince Keiper
// @brief   CN_SELFTEST_RPT Self Test Report. The report gets sent out from the 
//			PSU upon completion of the Self Test when invoked from LCD.
//          Also gets sent upon start of the Self Test if invoked by System Timer
//          The TestStaReg is the payload.
//
// @param cmd, 8 deep uint8 buffer
// @param id, uint32 CANID from PSU
// @retval none
// 
/******************************************************************************/
void CANFunction_205_TX(uint8_t state)
{
    datalength = state;

    canBuff[0]  = state;
    canBuff[1]  = SysData.TestStaReg         & 0x000000ff;
    canBuff[2]  = (SysData.TestStaReg >> 8)  & 0x000000ff;
    canBuff[3]  = (SysData.TestStaReg >> 16) & 0x000000ff;
    canBuff[4]  = (SysData.TestStaReg >> 24) & 0x000000ff;
    canBuff[5]  = 0xFF;
    canBuff[6]  = 0xFF;
    canBuff[7]  = 0xFF;

    CANWrite(SysData.NODE_ADDR, SysData.DC_ADDR, CMD_CAN_205, canBuff, datalength);
}
/*=================================================================================================
 *  FINIS
 *===============================================================================================*/

