
#include "iccpfuncs.h"
#include "iccp_canfuncs.h"
#include <string.h>
#include "configuration.h"


// Private variables
uint8_t lclCanBuff[8];
uint64_t tmr_cntr,tmr_status[4];
uint8_t psufail_cnt[4];
uint8_t fail_lockout[4];
unsigned char tmpbuff[32];
IccpPrfParam_t s_profile_ram[4]; //Default Iccp Profile Parameters to send if PSU is not programmed

SysData_t  SysData;

//External Variables
extern uint8_t b_CANTXFlag;


// TODO:  vk, need to implement the init routine yet
// DONE = X
// X    1. populate defaults into data struictures
// X    2. See how many PSU's are out there
//					X a. send out IDN? qry to each PSU adr (qty 10 1-10) waiting for response for each adr for 100mS
//		      X b. each one that answers CNHLLO sets the present bit ( bit 0 ) in the status element of the struct
//          X 3. Every PSU that has present bit set gets qry'd for it profile params and populates a struct
// X    4. Every PSU that answers its profile QRY ( once the structs are populated ) we set bit 2 of status reg
// X    5. Every PSU that has bit 2 set we send status qry for
//				Xa. status qry contains, 1.RectVrd, RectIrd,RectAlmReg,RefElecAvg(0,1,2,3)
//     6. If new profile is sent to PSU the struct is cleared and init process starts over
void Init_Iccp(void)
{
	 //int8_t i;
	
		//CreateConfigMemMap();  when you implement the configuration memory saving restoring routine uncomment 
	
		memset(fail_lockout,0,sizeof(fail_lockout));
	  memset(lclCanBuff,0,sizeof(lclCanBuff));
	  memset(psufail_cnt,0,sizeof(psufail_cnt));
	
    tmr_cntr =0;
	  tmr_status[0]=0;
	  

		// Build default iccp profile to send to PSU(s) if they are not programmed or need to sent to defaults
		s_profile_ram[0].refelec_min_th  = (-.22*1000);// default value is saved as -.22Vdc
		s_profile_ram[0].refelec_max_th  =  (.50*1000);// default value is saved as +.50Vdc
		s_profile_ram[0].rec_vset_min_th =   5 * 10;// min voltage 5.0 volts .1v/bit
		s_profile_ram[0].rec_vset_max_th =  60 * 10;// max voltage 60.0 volts .1v/bit
		s_profile_ram[0].rec_iset_min_th =   1;// min current 1 amp 1A/bit
		s_profile_ram[0].rec_iset_max_th =  25;// max current 25A 1A/bit
		s_profile_ram[0].rec_fs_vset     = 630;    // full scale voltage 63.0V .1V/bit
		s_profile_ram[0].rec_fs_iset     = 125;    // full scale current 125A 1A/bit
		s_profile_ram[0].rec_vset        = 545;    // voltage set if in CV mode 54.5V .1V/bit
		s_profile_ram[0].rec_iset        = 505;    // full scale current 50.5A 1A/bit
		s_profile_ram[0].alarm_reg_mask  = 0xFFFF; // all alarms pass through none masked
		s_profile_ram[0].self_test_int   = 60; // run self test 1 times every 60 minutes
		s_profile_ram[0].control_int     =  5; // run control routine every 5 seconds ( change rectifier current based on RefElec input)
		s_profile_ram[0].adc_smp_int     = 39; // interval the ADC values are stuffed into one index of the 128 smaple array in mS/bit 39mS= 128 samples in 5 seconds 
		s_profile_ram[0].tsts_active     =  1;  // bit mapped to each test in the self test routine, to allow us to select which tests will be run
		s_profile_ram[0].psu_mode        = P_CPOT; // Default to Constant potential
		s_profile_ram[0].refelec_active  = 0x0B; // Default to ref0, ref1 and ref3 active

		
		SysData.Status = 0x0000;// CLEAR STA REG indicating no CAN_HELLO, NO Profile Rxd, No STA response
		SysData.NODE_ADDR = 0xFF; // power up PSUADR with invalid CAN ADR for our network
																										// This serves as a way for me to know whether a PSu was 
																										// initilized correctly over CANbus
						
		
		
	//  Write Default Profiles to NVM (NOR FLASH)
	//	WriteProfileStructToNor(&s_profile_ram[0],0);
	//	WriteProfileStructToNor(&s_profile_ram[0],1);
	//	WriteProfileStructToNor(&s_profile_ram[0],2);
	//	WriteProfileStructToNor(&s_profile_ram[0],3);
		
		// While debugging w/o CAN load profile from MEM0 to PSU 0 profile to test with
		//ReadProfileStructFromNor(&GuiState.SysDataPSU[0].pflparams,0);
			
}

void PSUStateMachine(void)
{
	switch (SysData.init_state)
	{
			case I_OFF:
					// Send CN_HLLO request to PSU x
					CANWrite(SysData.NODE_ADDR,SysData.DC_ADDR,CMD_CAN_200,lclCanBuff,0);
					SysData.init_state = I_HLLO;
					tmr_status[0] = tmr_cntr;// seed timer for perspective psu
	
			break;
			
			case I_HLLO:
					 // Check for answer to CN_HLLO
					if(SysData.Status & 0x0001 && tmr_status[0] < tickspersec/2)
					{		
								tmr_status[0] = tmr_cntr;
								SysData.init_state = I_PFL;
								lclCanBuff[0]=0x00;// Byte0 must contain 0x00 to tell PSU to start sending profile frames
								// Send CN_PRFL request to PSU x
								CANWrite(SysData.NODE_ADDR,SysData.DC_ADDR,CMD_CAN_201,lclCanBuff,1);							
					}
					if(tmr_status[0] > tickspersec/2)
					{
							SysData.init_state = I_FAIL;
						  tmr_status[0] = tmr_cntr;
					}
			break;
			
			case I_PFL:
					// Check for answer to CN_PFL bit
					if(SysData.Status & 0x0004 && tmr_status[0] < tickspersec/2)
					{
						  tmr_status[0] = tmr_cntr;
							SysData.init_state = I_STAQRY;
					}
					if(tmr_status[0] > tickspersec/2)
					{
							SysData.init_state = I_FAIL;
							tmr_status[0] = tmr_cntr;
					}
			break;
			
			case I_STAQRY:
					 // Send CN_STA request to PSU x
					 CANWrite(SysData.NODE_ADDR,SysData.DC_ADDR,CMD_CAN_202,lclCanBuff,0);
					 SysData.init_state = I_STARSP;
					 SysData.Status &= 0xFFF7;// AND OFF bit 3 "STARSP RXD bit"
					 tmr_status[0] = tmr_cntr;
			break;
			
			case I_STARSP:
					 // Check for answer to CN_STA rxd bit
					if(SysData.Status & 0x0008 && tmr_status[0] < tickspersec/2)
					{
						  tmr_status[0] = tmr_cntr;
							SysData.init_state = I_STAQRY;
					}
				  
					if(tmr_status[0] > tickspersec/2)
					{
							SysData.init_state = I_FAIL;
							tmr_status[0] = tmr_cntr;
					}

			case I_FAIL:
					 if(fail_lockout[0] == 0)
					 {
								if(tmr_status[0] > tickspersec/2)
								{
										psufail_cnt[0]++;
										tmr_status[0] = tmr_cntr;
										
									  if(psufail_cnt[0] > 10)
										{
												fail_lockout[0] = 1;
										}
										else
										{
											  SysData.init_state = I_OFF;  //start over
												SysData.Status =0; //clear status reg bcuz we are starting over
										}
								}
					 }
					 else
					 {
								SysData.AlmReg  |= 0x0080; // SET COM ERROR BIT
					 }
					 
					
			break;
			
			
			default:
			break;
	}
	
}

void LcdControllerState(void)
{
	
			if(!b_CANTXFlag)// if the last time in the state machine a CANTX
											// was queued and not sent yet, block from going back
											// into the state maqcxhine until the CAN msg was sent
			{
					PSUStateMachine();
			
			}
}

void IccpDoIccp(void)
{
		//tmr_cntr++;// 100mS tick counter for all status timer operations
		LcdControllerState();
}


/**
*     Save profile from RAM location to Flash location
*
*			psu_mode_t psu_mode;// P_CV,P_CI,P_CPOT,P_OFF
			int16_t refelec_min_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
			int16_t refelec_max_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
			uint16_t rec_vset;        // resolution 1mv/bit 4096 = Full Scale
			uint16_t rec_vset_max_th; // resolution 100mv/bit 4096 = Full Scale
			uint16_t rec_vset_min_th; // resolution 100mv/bit
			uint16_t rec_fs_vset;     // resolution = vset_fs / 4096 ex 63V/4096 = 15mA/bit for a 54V rect. this would be the OV point or 63v= 630bits
			uint16_t rec_iset; 				// resolution = iset_fs/4096 ex. 125A/4096 = 30mA/bit
			uint16_t rec_iset_max_th; // resolution 1A/bit
			uint16_t rec_iset_min_th; // resolution 1A/bit
			uint16_t rec_fs_iset;     // resolution 1A/bit 
			uint16_t alarm_reg_mask;  // 0= bit masked 1= active, same bit mapping as alarm register
			uint16_t self_test_int;// interval self test is run at in minutes/bit 0= disbaled
			uint8_t adc_smp_int;// interval adc samples are placed into 128 sample array mS/bit 0=uses real time values for debug purposes
			uint8_t control_int;// interval the rectifer output current is updated based on the ref elec AVG buffers. second/bit 
			uint32_t tsts_active; // bit mapped to each test in the self test routine, to allow us to select which tests will be run
			uint8_t  refelec_active; //
} IccpPrfParam_t; //Iccp Profile Parameters

**/

void WriteProfileStructToNor(IccpPrfParam_t *pfl, uint8_t prflidx)
{
		//uint8_t idx;
	
//		if(prflidx == 0){idx = PRFL0;}
//		if(prflidx == 1){idx = PRFL1;}
//		if(prflidx == 2){idx = PRFL2;}
//		if(prflidx == 3){idx = PRFL3;}
//		
		//CfgSetArrayData((unsigned char*)pfl, idx, 27);
}


void ReadProfileStructFromNor(IccpPrfParam_t *pfl, uint8_t prflidx)
{
		//uint8_t idx;
	
//		if(prflidx == 0){idx = PRFL0;}
//		if(prflidx == 1){idx = PRFL1;}
//		if(prflidx == 2){idx = PRFL2;}
//		if(prflidx == 3){idx = PRFL3;}
//		
		//CfgGetArrayData((unsigned char*)pfl, idx, 27);
}


void CopyProfileStructRam2Ram(IccpPrfParam_t *pflsrc,IccpPrfParam_t *pfldst)
{
		pfldst->psu_mode = pflsrc->psu_mode;
		pfldst->adc_smp_int = pflsrc->adc_smp_int;
		pfldst->alarm_reg_mask = pflsrc->alarm_reg_mask;
		pfldst->control_int = pflsrc->control_int;
		pfldst->rec_fs_iset = pflsrc->rec_fs_iset;
		pfldst->rec_fs_vset = pflsrc->rec_fs_iset;
		pfldst->rec_iset = pflsrc->rec_iset;
		pfldst->rec_iset_max_th = pflsrc->rec_iset_max_th;
		pfldst->rec_iset_min_th = pflsrc->rec_iset_min_th;
		pfldst->rec_vset_max_th = pflsrc->rec_vset_max_th;
		pfldst->rec_vset_min_th = pflsrc->rec_vset_min_th;
		pfldst->rec_vset = pflsrc->rec_vset;
		pfldst->refelec_active = pflsrc->refelec_active;
		pfldst->refelec_max_th = pflsrc->refelec_max_th;
		pfldst->refelec_min_th = pflsrc->refelec_min_th;
		pfldst->self_test_int = pflsrc->self_test_int;
		pfldst->tsts_active = pflsrc->tsts_active;
	
}






