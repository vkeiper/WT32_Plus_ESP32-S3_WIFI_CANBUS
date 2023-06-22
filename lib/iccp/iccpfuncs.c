

#include "iccpfuncs.h"
#include "iccp_canfuncs.h"

#ifdef STM32F429
#include "k_module.h"
#include "k_storage.h"
#include "k_rtc.h" 
#include "stm324x9i_eval_nor.h"
#endif
#ifdef USECFGDRVR
#include "configuration.h"
#endif

// Private variables
uint8_t lclCanBuff[8];
uint64_t tmr_cntr[4],tmr_status[4];

unsigned char tmpbuff[32];
IccpPrfParam_t s_profile_ram[4]; //Default Iccp Profile Parameters to send if PSU is not programmed

GuiState_t GuiState;

//External resources
extern uint8_t b_CANTXFlag;

extern 	uint8_t WriteToLog(char *ptrpath,
		uint8_t *sec,uint8_t *min,uint8_t *hour,
		uint8_t *mon,uint8_t *day,uint16_t *year, 
		uint16_t *vrd,uint16_t *ird,uint8_t *temp,
		uint8_t *psuidx);

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
	int8_t i;
	
	//LoadCfgMemMap2Ram();

	//InitConfigVars();


	memset(lclCanBuff,0,sizeof(lclCanBuff));
	
	for(i=0;i<4;i++)
	{
		GuiState.SysDataPSU[i].fail_lockout = 0;
		GuiState.SysDataPSU[i].psufail_cnt = 0;
	}

	tmr_cntr[0] =0;
	tmr_cntr[1] =0;
	tmr_cntr[2] =0;
	tmr_cntr[3] =0;

	tmr_status[0]=0;
	tmr_status[1]=0;
	tmr_status[2]=0;
	tmr_status[3]=0;
	

	// If these profile parameters are not returned by the PSU's then we must load defaults 
	GuiState.psu_viewing = 0;// startup viewing PSU0
		
	// Build default iccp profile to send to PSU(s) if they are not programmed or need to sent to defaults
	s_profile_ram[0].refelec_min_th  = (-.50*1000);		// default value is saved as -.22Vdc
	s_profile_ram[0].refelec_dmd_setpt = (0.24 *1000);	// default value is saved as 0.12Vdc //EAJ Test Only
	s_profile_ram[0].refelec_max_th  = (.50*1000);		// default value is saved as +.50Vdc
	s_profile_ram[0].rec_vset_min_th = 5 * 10;		// min voltage 5.0 volts .1v/bit
	s_profile_ram[0].rec_vset_max_th = 27 * 10;         // max voltage 60.0 volts .1v/bit
	s_profile_ram[0].rec_iset_min_th = 1;               // min current 1 amp 1A/bit
	s_profile_ram[0].rec_iset_max_th = 25;     // max current 25A 1A/bit
	s_profile_ram[0].rec_fs_vset     = 295;    // full scale voltage 63.0V .1V/bit
	s_profile_ram[0].rec_fs_iset     = 1000;   // full scale current 125A 1A/bit
	s_profile_ram[0].rec_vset        = 240;    // voltage set if in CV mode 54.5V .1V/bit
	s_profile_ram[0].rec_iset        = 1000;   // full scale current 50.5A 1A/bit
	s_profile_ram[0].alarm_reg_mask  = 0xFFFF; // all alarms pass through none masked
	s_profile_ram[0].self_test_int   = 60;     // run self test 1 times every 60 minutes
	s_profile_ram[0].control_int     =  5;     // run control routine every 5 seconds ( change rectifier current based on RefElec input)
	s_profile_ram[0].adc_smp_int     = 39;     // interval the ADC values are stuffed into one index of the 128 smaple array in mS/bit 39mS= 128 samples in 5 seconds 
	s_profile_ram[0].tsts_active     =  1;     // bit mapped to each test in the self test routine, to allow us to select which tests will be run
	s_profile_ram[0].psu_mode        = P_CPOT; // Default to Constant potential
	s_profile_ram[0].last_psu_mode   = P_CPOT; // Default to Constant potential
	s_profile_ram[0].refelec_active  = 0x0B;   // Default to ref0, ref1 and ref3 active
	
	//s_profile_ram[0].refelec_dmd_setpt = 0; //EAJ 2016/01/12
	strcpy(s_profile_ram[0].refeleclongname[0],"<ELEC 1 DESC>"); //EAJ 2016/01/12
	strcpy(s_profile_ram[0].refeleclongname[1],"<ELEC 2 DESC>"); //EAJ 2016/01/12
	strcpy(s_profile_ram[0].refeleclongname[2],"<ELEC 3 DESC>"); //EAJ 2016/01/12
	strcpy(s_profile_ram[0].refeleclongname[3],"<ELEC 4 DESC>"); //EAJ 2016/01/12

	for(i=0;i<4;i++)
	{
		GuiState.SysDataPSU[i].Status 	= 0x0000; // CLEAR STA REG indicating no CAN_HELLO, NO Profile Rxd, No STA response
		GuiState.SysDataPSU[i].PSU_ADDR = 0xFF; // power up PSUADR with invalid CAN ADR for our network
												// This serves as a way for me to know whether a PSu was 
												// initilized correctly over CANbus						
	}
		
		
	//  Write Default Profiles to NVM (NOR FLASH)
	//	WriteProfileStructToNor(&s_profile_ram[0],0);
	//	WriteProfileStructToNor(&s_profile_ram[0],1);
	//	WriteProfileStructToNor(&s_profile_ram[0],2);
	//	WriteProfileStructToNor(&s_profile_ram[0],3);
		
	// While debugging w/o CAN load profile from MEM0 to PSU 0 profile to test with
	GuiState.style = 0; //EAJ 2/9/2016
	GuiState.demo = 0; //EAJ 2/11/2016

}

void PSUStateMachine(uint8_t psuidx)
{
	static const char uSDpath[14] = {"1:/default.log"};
	static uint8_t i_starsp_timeout_cnt = 0;
							 
	switch (GuiState.SysDataPSU[psuidx].init_state)
	{
		case I_OFF:
			GuiState.SysDataPSU[psuidx].Status =0; //clear status reg bcuz we are starting over
			// Send CN_HLLO request to PSU x
			CANWrite(psuidx,0x00,CMD_CAN_200,lclCanBuff,0);
			GuiState.SysDataPSU[psuidx].init_state = I_HLLO;
			tmr_status[psuidx] = tmr_cntr[psuidx];// seed timer for perspective psu

		break;
		
		case I_HLLO:
			// Check for answer to CN_HLLO
			if(GuiState.SysDataPSU[psuidx].Status & 0x0001)
			{		
				tmr_status[psuidx] = tmr_cntr[psuidx];
				GuiState.SysDataPSU[psuidx].psufail_cnt = 0;
				GuiState.SysDataPSU[psuidx].init_state = I_PFL;
				lclCanBuff[0]=0x00;// Byte0 must contain 0x00 to tell PSU to start sending profile frames
				// Send CN_PRFL request to PSU x
				CANWrite(GuiState.SysDataPSU[psuidx].PSU_ADDR,GuiState.SysDataPSU[psuidx].DC_ADDR,CMD_CAN_201,lclCanBuff,0);							
			}
			if((tmr_cntr[psuidx]-tmr_status[psuidx]) > tickspersec)
			{
				GuiState.SysDataPSU[psuidx].init_state = I_FAIL;
				tmr_status[psuidx] = tmr_cntr[psuidx];
			}
		break;
		
		case I_PFL:
			// Check for answer to CN_PFL bit
			if(GuiState.SysDataPSU[psuidx].Status & 0x0004)
			{
				tmr_status[psuidx] = tmr_cntr[psuidx];
				GuiState.SysDataPSU[psuidx].psufail_cnt = 0;
				GuiState.SysDataPSU[psuidx].init_state = I_STAQRY;
			}
			if((tmr_cntr[psuidx]-tmr_status[psuidx]) > tickspersec)
			{
				GuiState.SysDataPSU[psuidx].init_state = I_FAIL;
				tmr_status[psuidx] = tmr_cntr[psuidx];
			}
		break;
		
		case I_STAQRY:
			// Send CN_STA request to PSU x
			CANWrite(GuiState.SysDataPSU[psuidx].PSU_ADDR,GuiState.SysDataPSU[psuidx].DC_ADDR,CMD_CAN_202,lclCanBuff,0);
			GuiState.SysDataPSU[psuidx].init_state = I_STARSP;
			GuiState.SysDataPSU[psuidx].Status &= 0xFFF7;// AND OFF bit 3 "STARSP RXD bit"
			tmr_status[psuidx] = tmr_cntr[psuidx];
		break;
		
		case I_STARSP:
			// Check for answer to CN_STA rxd bit
			//GuiState.SysDataPSU[psuidx].Status |= 0x0008;
			if(GuiState.SysDataPSU[psuidx].Status & 0x0008)
			{
				tmr_status[psuidx] = tmr_cntr[psuidx];
				GuiState.SysDataPSU[psuidx].psufail_cnt = 0;
				i_starsp_timeout_cnt=0;
				GuiState.SysDataPSU[psuidx].init_state = I_STAQRY;
			}
			
			if((tmr_cntr[psuidx]-tmr_status[psuidx]) > 1) //was tickspersec
			{
				i_starsp_timeout_cnt++;
			
				if(i_starsp_timeout_cnt > 5)
				{						
						GuiState.SysDataPSU[psuidx].init_state = I_FAIL;
						i_starsp_timeout_cnt=0;
				}
				else
				{
						GuiState.SysDataPSU[psuidx].init_state = I_STAQRY;
				}
				
				tmr_status[psuidx] = tmr_cntr[psuidx];
			}
		break;
				
		case I_FAIL:
			
			GuiState.SysDataPSU[psuidx].PSU_ADDR = 0x00ff;
	
			if(GuiState.SysDataPSU[psuidx].fail_lockout == 0)
			{
				if((tmr_cntr[psuidx]-tmr_status[psuidx]) > tickspersec)
				{
					GuiState.SysDataPSU[psuidx].psufail_cnt++;
					
					tmr_status[psuidx] = tmr_cntr[psuidx];
					
					if(GuiState.SysDataPSU[psuidx].psufail_cnt >= psufail_cnt_max)
					{
						GuiState.SysDataPSU[psuidx].fail_lockout = 1;
						GuiState.SysDataPSU[psuidx].PSU_ADDR = 0x00ff;  //Tell GUI that address is not valid
						//and force status screen to repaint in ERROR mode
					}
					else
					{
						GuiState.SysDataPSU[psuidx].init_state = I_OFF;  //start over										
					}
				}
			}
			else
			{
				GuiState.SysDataPSU[psuidx].AlmReg  |= 0x0080; // SET COM ERROR BIT
			}
			
				
		break;
		
		
		default:
		break;
	}
	
}

void LcdControllerState(void)
{
	static int8_t i=FIRSTNODE;
	
	if(i<QTYNODES)
	{
		// if the last time in the state machine a CANTX or transmitting profile
		// was queued and not sent yet, block from going back
		// into the state machine until the CAN msg was sent
		if((!b_CANTXFlag)||!(BITCHK(GuiState.SysDataPSU[i].Status,6)))
		{
			if(!GuiState.demo){tmr_cntr[i]++;}// 100mS tick counter for all status timer operations
			PSUStateMachine(i);
			i++;
			if(i >=QTYNODES){
				i = FIRSTNODE;
			}
		}
	}
	
}

void IccpDoIccp(void)
{
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
	/*
	uint8_t idx;
	
	if(prflidx == 0){idx = PRFL0;}
	if(prflidx == 1){idx = PRFL1;}
	if(prflidx == 2){idx = PRFL2;}
	if(prflidx == 3){idx = PRFL3;}
	
	CfgSetArrayData((unsigned char*)pfl, idx, 52); //EAJ 1/14/2016 - Was 27 //104
	*/
}


void ReadProfileStructFromNor(IccpPrfParam_t *pfl, uint8_t prflidx)
{
		uint8_t idx;
	/*
		if(prflidx == 0){idx = PRFL0;}
		if(prflidx == 1){idx = PRFL1;}
		if(prflidx == 2){idx = PRFL2;}
		if(prflidx == 3){idx = PRFL3;}
		
		CfgGetArrayData((unsigned char*)pfl, idx, 52); //EAJ 1/14/2016 - Was 27
	*/
}


void CopyProfileStructRam2Ram(IccpPrfParam_t *pflsrc,IccpPrfParam_t *pfldst)
{
	pfldst->psu_mode = pflsrc->psu_mode;
	pfldst->adc_smp_int = pflsrc->adc_smp_int;
	pfldst->alarm_reg_mask = pflsrc->alarm_reg_mask;
	pfldst->control_int = pflsrc->control_int;
	pfldst->rec_fs_iset = pflsrc->rec_fs_iset;
	pfldst->rec_fs_vset = pflsrc->rec_fs_vset;
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

	pfldst->refelec_dmd_setpt = pflsrc->refelec_dmd_setpt; //EAJ 2016/01/12
	strcpy(pfldst->refeleclongname[0],pflsrc->refeleclongname[0]);
	strcpy(pfldst->refeleclongname[1],pflsrc->refeleclongname[1]);
	strcpy(pfldst->refeleclongname[2],pflsrc->refeleclongname[2]);
	strcpy(pfldst->refeleclongname[3],pflsrc->refeleclongname[3]);

}


void SetIPflash(uint32_t ipaddress32 )
{
	uint8_t tmpbuff[4];
	int8_t i;
	// convert 32bit to 4 byte array
	for(i=0;i<4;i++)
	{
			
			tmpbuff[i] = ipaddress32>>i*8 & 0x000000ff;
			
			GuiState.netpms.ip[i] =	tmpbuff[i];// set struct var
	}
	// send to FLASH
	//CfgSetArrayData(tmpbuff, IPADR, 4);
}	

uint32_t GetIPflash(void)
{
	uint8_t tmpbuff[4];
	int8_t i;
	uint32_t retval;
	
	// send to FLASH
	//CfgGetArrayData(tmpbuff, IPADR, 4);
	
	// convert 4 byte array to 32bit var & update struct var
	retval=0;
	for(i=0;i<4;i++)
	{
			GuiState.netpms.ip[i] =	tmpbuff[i];// set struct var
			if(i!=0){
				retval = retval<<8; //make room for next byte
			}
			retval = retval | tmpbuff[i];//OR on next byte
	}
	
	return retval; 
}	


void SetMacflash(uint64_t mac64 )
{
	uint8_t tmpbuff[6];
	int8_t i;
	// convert 64bit to 6 byte array
	for(i=0;i<6;i++)
	{	
			tmpbuff[i] = mac64>>i*8 & 0x00000000000000ff;	
			GuiState.netpms.mac[i] =	tmpbuff[i];// set struct var
	}
	// send to FLASH
	//CfgSetArrayData(tmpbuff, MACADR, 6);
}	

uint64_t GetMacflash(void)
{
	uint8_t tmpbuff[6];
	int8_t i;
	uint64_t retval;
	
	// send to FLASH
	//CfgGetArrayData(tmpbuff, MACADR, 6);
	
	// convert 6 byte array to 64bit var & update struct var
	retval=0;
	for(i=0;i<6;i++)
	{
			GuiState.netpms.mac[i] =	tmpbuff[i];// set struct var
			if(i!=0){
				retval = retval<<8; //make room for next byte
			}
			retval = retval | tmpbuff[i];//OR on next byte
	}
	
	return retval; 
}	
