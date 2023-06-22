#include <string.h>
#include <stm32f4xx.h>                  // Device header
#include <stm32f4xx_hal.h>              // Keil::Device:STM32Cube HAL:Common
#include <stm32f4xx_hal_conf.h>
#include "main.h"
#include "selftest.h"
#include "iccpfuncs.h"
#include "iccp_canfuncs.h"
#include "configuration.h"

// Private variables
uint8_t         lclCanBuff[8];
uint64_t        tmr_cntr, tmr_status;
uint8_t         psufail_cnt[4];
uint8_t         fail_lockout[4];
unsigned char   tmpbuff[32];
IccpPrfParam_t  s_profile_ram[4];       // Default Iccp Profile Parameters to 
                                        // send if PSU is not programmed

// External Variables
extern      uint8_t             b_CANTXFlag;
extern      ADC_HandleTypeDef   hADC1;
extern      DAC_HandleTypeDef   hDAC1;

SysData_t   SysData;

// TODO:  vk, need to implement the init routine yet
// DONE = X
// X    1. populate defaults into data struictures
// X    2. See how many PSU's are out there
// X        a.  send out IDN? qry to each PSU adr (qty 10 1-10) waiting for response for each adr for 100mS
// X        b.  each one that answers CNHLLO sets the present bit ( bit 0 ) in the status element of the struct
// X    3. Every PSU that has present bit set gets qry'd for it profile params and populates a struct
// X    4. Every PSU that answers its profile QRY ( once the structs are populated ) we set bit 2 of status reg
// X    5. Every PSU that has bit 2 set we send status qry for
// X        a.  status qry contains, 1.RectVrd, RectIrd, RectAlmReg, RefElecAvg(0,1,2,3)
//      6. If new profile is sent to PSU the struct is cleared and init process starts over
void Init_Iccp(void)
{
	// int8_t i;
	
    CreateConfigMemMap();  // when you implement the configuration memory saving restoring routine uncomment 
		
	// TODO when configuration is implemeted read rectifier type ( ESPM or LB ) and that will decide if we setup CAN2 at all and 
	// also how many current sensors we will rread and sum
		
	SysData.init_state = I_OFF;// startup init off until CAN hello from LCD controller
		
    memset(fail_lockout, 0, sizeof(fail_lockout));
    memset(lclCanBuff, 0, sizeof(lclCanBuff));
    memset(psufail_cnt, 0, sizeof(psufail_cnt));
	
    tmr_cntr    = 0;
    tmr_status  = 0;
     
        // Build default iccp profile to send to PSU(s) if they are not programmed or need to set to defaults
    #if (0)
        SysData.pflparams.refelec_min_th     = (-0.50*1000);    // default value is saved as -0.50Vdc
        SysData.pflparams.refelec_max_th     =  (0.50*1000);    // default value is saved as +0.50Vdc
        SysData.pflparams.refelec_dmd_setpt  = (0.24*1000);    // default value is saved as -0.22Vdc
        SysData.pflparams.rec_vset_min_th    =   5 * 10;        // min voltage 5.0 volts .1v/bit
        SysData.pflparams.rec_vset_max_th    =  60 * 10;        // max voltage 60.0 volts .1v/bit
        SysData.pflparams.rec_iset_min_th    =   1;             // min current 1 amp 1A/bit
        SysData.pflparams.rec_iset_max_th    =  25;             // max current 25A 1A/bit
        SysData.pflparams.rec_fs_vset        = 295;             // full scale voltage 63.0V .1V/bit
        SysData.pflparams.rec_fs_iset        = 1000;            // full scale current 125A 1A/bit
        SysData.pflparams.rec_vset           = 545;             // voltage set if in CV mode 54.5V .1V/bit
        SysData.pflparams.rec_iset           = 250;             // full scale current 50.5A 1A/bit
        SysData.pflparams.alarm_reg_mask     = 0xFFFF;          // all alarms pass through none masked
        SysData.pflparams.self_test_int      = 60;              // run self test 1 times every 60 minutes
        SysData.pflparams.control_int        =  5;              // run control routine every 5 seconds ( change rectifier current based on RefElec input)
        SysData.pflparams.adc_smp_int        = 39;              // interval the ADC values are stuffed into one index of the 128 smaple array in mS/bit 39mS= 128 samples in 5 seconds 
        SysData.pflparams.tsts_active        =  1;              // bit mapped to each test in the self test routine, to allow us to select which tests will be run
        SysData.pflparams.psu_mode           = P_CPOT;          // Default to Constant potential
        SysData.pflparams.refelec_active     = 0x0F;            // Default to ref0, ref1, ref2 and ref3 active
  #endif

    SysData.Status    = 0x0000; // CLEAR STA REG indicating no CAN_HELLO, NO Profile Rxd, No STA response
    SysData.NODE_ADDR = 0xFF;   // power up PSUADR with invalid CAN ADR for our network
                                // This serves as a way for me to know whether a PSU was 
                                // initilized correctly over CANbus
						
////	WriteProfileStructToNor(&SysData.pflparams, 0);
    
    memset(&SysData.pflparams, 0, sizeof(SysData.pflparams));
    ReadProfileStructFromFlash(&SysData.pflparams, 0);
}
void PSUStateMachine(void)
{
    switch (SysData.init_state)
	{
        case I_OFF:
            SysData.init_state = I_HLLO;
            tmr_status = tmr_cntr;  // seed timer for perspective psu
        break;
			
        case I_HLLO:
            // Check for CN_HLLO request from LCD controller
            if (SysData.Status & 0x0001 && tmr_status < tickspersec/2)
            {		
                tmr_status          = tmr_cntr;
                SysData.init_state  = I_PFL;
            }
					
            //timeout mechanism
            if (tmr_status > tickspersec/2)
            {
                SysData.init_state  = I_FAIL;
                tmr_status          = tmr_cntr;
            }
			break;
			
			case I_PFL:
            // Check for rxd CN_PFL request from LCd controller bit 2
            if (SysData.Status & 0x0004 && tmr_status < tickspersec/2)
            {
                tmr_status          = tmr_cntr;
                SysData.init_state  = I_STAQRY;
            }
					
            //timeout mechanism
            if(tmr_status > tickspersec/2)
            {
                SysData.init_state  = I_FAIL;
                tmr_status          = tmr_cntr;
            }
			break;
			
			case I_STAQRY:
             // Wait for LCD to send CN_STA request to us
             // Check for rxd CN_PFL request from LCd controller bit 2
             if (SysData.Status & 0x0010 && tmr_status < tickspersec/2)
             {
                tmr_status          = tmr_cntr;
                SysData.init_state  = I_STARSP;
             }
			break;
			
			case I_STARSP:  // Status response 
             // Check to see if we sent all status frames to LCD
            if (SysData.Status & 0x0020 && tmr_status < tickspersec/2)
            {
                tmr_status  = tmr_cntr;
                SysData.stacnt++;
							
                // Every 10 status updates do a PSU ccontrol loop
                if (SysData.stacnt > 10)
                {
                    SysData.stacnt      = 0;
                    SysData.init_state  = I_CTLPSU;
                }
                else
                {
                    SysData.init_state = I_STAQRY;
                }
            }
				  
            // timeout mechanism
            if (tmr_status > tickspersec/2)
            {
                SysData.init_state = I_FAIL;
                tmr_status = tmr_cntr;
            }
			break;
			
			case I_CTLPSU:
					 
            // If not in fault we can check the REF electrodes and adjust the DAC up/down based on value
				
            if (SysData.AlmReg & 0x000E ) // if ACOK, DCOK, SUM FLT not in fault then control PSU
            {
                tmr_status = tmr_cntr;
						
                // TODO:  	1. Do all this in seperate function... looping state machine for control integrate DAC result also
                //  		2. find the higest RefElec, find the lowest RefElec
                //  		3. make sure they are all between min and max params from profile
                //          4. adj PSU control +/- toward the direction needed to make ref elec happy			
                //  		5. Also check for faults and update thise registers, module and cabient faults
                //          6. Sum currents from all sensors intp SysData.IrdNode
                //          7. Test for OT by monitoring the temp
                //
                // quick sample, this is not real... change DAC output based on refelec
////                if (SysData.RefCell[0].avg > SysData.pflparams.refelec_min_th  &&
////                    SysData.RefCell[0].avg < SysData.pflparams.refelec_max_th)
////                {
////                    SysData.dacpgm = SysData.dacpgm + 1;//increment 1 bit per adjustment to make this very slow.
////                    // TODO:... Masking time to allow change to take effect???
////                    // call out blank out time before you allow back in here to readjust
////                    // this must be implemented to ensure we do not overshoot while waiting for Ref Electrodes to 
////                    // feedback data. Is there a lag? IDK, maybe we do not have to add this mask time at all
////                    //	testing will prove it either way.				
////                }
////                else
////                {
////                    // Set DAC 0 in fault or limit to 
////                }
                SysData.init_state = I_STAQRY;// jump back to do more status qry answers
							
                }
                else
                {
                    SysData.init_state = I_STAQRY;// jump back to do more status qry answers
                }
				  

			break;
					
			case I_FAIL:
             if (fail_lockout[0] == 0)
             {
                if (tmr_status > tickspersec/2)
                {
                    psufail_cnt[0]++;
                    tmr_status = tmr_cntr;
										
                    if (psufail_cnt[0] > 10)
                    {
                        fail_lockout[0] = 1;
                    }
                    else
                    {
                        SysData.init_state  = I_OFF;  //start over
                        SysData.Status      = 0; //clear status reg bcuz we are starting over
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
    if (!b_CANTXFlag)   // if the last time in the state machine a CANTX
                        // was queued and not sent yet, block from going back
                        // into the state machine until the CAN msg was sent
    {
        PSUStateMachine();	
    }
}
void IccpDoIccp(void)
{
    //tmr_cntr++; // 100mS tick counter for all status timer operations
    LcdControllerState();
}
/**
 *     Save profile from RAM location to Flash location
 *
 *			psu_mode_t psu_mode;      // P_CV,P_CI,P_CPOT,P_OFF
			int16_t refelec_min_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
			int16_t refelec_max_th;   // resolution 1mv/bit  -2048 = -.5Vdc  +2048= +.5Vdc
			uint16_t rec_vset;        // resolution 1mv/bit 4096 = Full Scale
			uint16_t rec_vset_max_th; // resolution 100mv/bit 4096 = Full Scale
			uint16_t rec_vset_min_th; // resolution 100mv/bit
			uint16_t rec_fs_vset;     // resolution = vset_fs / 4096 ex 63V/4096 = 15mA/bit for a 54V rect. this would be the OV point or 63v= 630bits
			uint16_t rec_iset; 		  // resolution = iset_fs/4096 ex. 125A/4096 = 30mA/bit
			uint16_t rec_iset_max_th; // resolution 1A/bit
			uint16_t rec_iset_min_th; // resolution 1A/bit
			uint16_t rec_fs_iset;     // resolution 1A/bit 
			uint16_t alarm_reg_mask;  // 0= bit masked 1= active, same bit mapping as alarm register
			uint16_t self_test_int;   // interval self test is run at in minutes/bit 0= disbaled
			uint8_t adc_smp_int;      // interval adc samples are placed into 128 sample array mS/bit 0=uses real time values for debug purposes
			uint8_t control_int;      // interval the rectifer output current is updated based on the ref elec AVG buffers. second/bit 
			uint32_t tsts_active;     // bit mapped to each test in the self test routine, to allow us to select which tests will be run
			uint8_t  refelec_active;  //
} IccpPrfParam_t;                     // Iccp Profile Parameters

**/
void WriteProfileStructToFlash(IccpPrfParam_t *pfl, uint8_t prflidx)
{
    uint8_t idx;
	
	if (prflidx == 0) {idx = PRFL0;}
//		if (prflidx == 1) {idx = PRFL1;}
//		if (prflidx == 2) {idx = PRFL2;}
//		if (prflidx == 3) {idx = PRFL3;}
//		
    CfgSetArrayData((unsigned char *)pfl, idx, 52);
}
void ReadProfileStructFromFlash(IccpPrfParam_t *pfl, uint8_t prflidx)
{
    uint8_t idx;
	
    if (prflidx == 0) {idx = PRFL0;}
//		if (prflidx == 1) {idx = PRFL1;}
//		if (prflidx == 2) {idx = PRFL2;}
//		if (prflidx == 3) {idx = PRFL3;}
//		
    CfgGetArrayData((unsigned char *)pfl, idx, 52);
}
void CopyProfileStructRam2Ram(IccpPrfParam_t *pflsrc, IccpPrfParam_t *pfldst)
{
    pfldst->psu_mode            = pflsrc->psu_mode;
    pfldst->adc_smp_int         = pflsrc->adc_smp_int;
    pfldst->alarm_reg_mask      = pflsrc->alarm_reg_mask;
    pfldst->control_int         = pflsrc->control_int;
    pfldst->rec_fs_iset         = pflsrc->rec_fs_iset;
    pfldst->rec_fs_vset         = pflsrc->rec_fs_vset;
    pfldst->rec_iset            = pflsrc->rec_iset;
    pfldst->rec_iset_max_th     = pflsrc->rec_iset_max_th;
    pfldst->rec_iset_min_th     = pflsrc->rec_iset_min_th;
    pfldst->rec_vset_max_th     = pflsrc->rec_vset_max_th;
    pfldst->rec_vset_min_th     = pflsrc->rec_vset_min_th;
    pfldst->rec_vset            = pflsrc->rec_vset;
    pfldst->refelec_active      = pflsrc->refelec_active;
    pfldst->refelec_max_th      = pflsrc->refelec_max_th;
    pfldst->refelec_min_th      = pflsrc->refelec_min_th;
    pfldst->refelec_dmd_setpt   = pflsrc->refelec_dmd_setpt;
    pfldst->self_test_int       = pflsrc->self_test_int;
    pfldst->tsts_active         = pflsrc->tsts_active;
}
/* ================================================================================================
 * @author      Neale Morris
 * @brief       Toggles the Heart beat LED periodically every 500msecs
 *  
 * @pre-req     None
 * @param       None
 * @retval      None
 * ==============================================================================================*/
static void LED_HeartBeat(void)
{
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
}
/* ================================================================================================
 * @author      Vince Keiper, Neale Morris
 * @brief       The node address of the ICCP Defender is determined by a DIP
 *			    switch that has 5 resistor dividers that are each seperated
 *			    by @200-300 bits. I use this spread to turn adc bits into 
 *              CAN address.           
 *
 *              Sw      State     V(In)     V(Conv)        [Vref = 2.500V]
 *              ---     ------    ------    --------           
 *              ALL     Open      2.500       4095
 *              1       Closed    2.272       3721
 *              & 2     Closed    1.929       3159
 *              & 3     Closed    1.462       2394
 *              & 4     Closed    1.008       1651
 *              & 5     Closed    0.718       1176
 *
 * @pre-req     ADC interupts must be setup and running prior to calling.
 * @param       adcval, uint16 pass in ADC reading of the adress dip switch
 * @retval      uint8_t adr = CANbus address of NODE
 * ==============================================================================================*/
uint8_t GetNodeAddress(uint16_t adcval)
{
    uint8_t adr = 0;
	uint16_t rangedelta = 273;

    if ((adcval > (rangedelta * 14)) && (adcval <= 4095))
    {
        adr = 0;
    }
    else if ((adcval > (rangedelta * 7)) && (adcval <= (rangedelta * 8)))
	{
		adr = 1;
	}
    else if ((adcval > (rangedelta * 8)) && (adcval <= (rangedelta * 9)))
	{
		adr = 2;
	}
    else if ((adcval > (rangedelta * 10)) && (adcval <= (rangedelta * 11)))
	{
        adr = 3;
	}
    else if ((adcval > (rangedelta * 12)) && (adcval <= (rangedelta * 13)))
	{
		adr = 4;
	}
    else if ((adcval > (rangedelta * 13)) && (adcval <= 14))
	{
		adr = 5;
	} 
    else 
    {
        adr = 0xFA;     /// Invalid input
    }

    return adr;
}
/* ================================================================================================
 * @author      Neale Morris
 * @brief       En/disable the inhibit pin when the LCD DC Off/On button is 
 *              depressed.                          
 *
 * @pre-req     Respective pin on port B configured for output.
 * @param       None
 * @retval      None
 * ==============================================================================================*/
static void RectifierOnOff(void)
{
    switch(SysData.pflparams.psu_mode)
    {
        case P_CI:
        case P_CV:
        case P_CPOT:
            if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_SET)
            {    
                __NOP();
            } 
            else 
            {
                HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
                BITSET(SysData.AlmReg, BIT_OUTPUT);
            }
        break;

        case P_OFF:
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
            BITCLR(SysData.AlmReg, BIT_OUTPUT);
        break;
        
        default:
            break;
    }
}
/* ================================================================================================
 * @author      Neale Morris
 * @brief       Reads respective GPIO pins for alarm status, delay for a 
 *              period of time and retest GPIO pins, confirming status.
 *
 * @pre-req     Respective pin on port B configured for input.
 * @param       None
 * @retval      None
 * ==============================================================================================*/
static void GetAlarmStatus(void)
{
    static uint8_t     AlarmCount;      
    static uint16_t    AlarmStatus;
    
    if (AlarmCount == 0)
    {
        AlarmCount++;

        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_SET)
        {
            BITSET(AlarmStatus, BIT_ACOK);
        } 
        else 
        {
            __NOP();
        }
        
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_SET)
        {
            BITSET(AlarmStatus, BIT_FRONTDOOR);
        }
        else 
        {
            __NOP();
        }

        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_SET)
        {
            BITSET(AlarmStatus, BIT_SURGE);     // AC SURGE
        } 
        else 
        {
            __NOP();
        }

        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_SET)
        {
            BITSET(AlarmStatus, BIT_SURGE);     // DC SURGE
        } 
        else 
        {
            __NOP();
        }

        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_SET)
        {
            BITSET(AlarmStatus, BIT_DCOK);
        } 
        else 
        {
            __NOP();
        }
    }

    else if (AlarmCount > 0)
    {
        AlarmCount = 0;
        
        if((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_SET) &&  BITCHK(AlarmStatus, BIT_ACOK))
        {
            BITSET(SysData.AlmReg, BIT_ACOK);
        } 
        else 
        {
            BITCLR(SysData.AlmReg, BIT_ACOK);
            BITCLR(AlarmStatus, BIT_ACOK);
        }

        if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_SET) && BITCHK(AlarmStatus, BIT_FRONTDOOR))
        {
            BITSET(SysData.AlmReg, BIT_FRONTDOOR);
        } 
        else 
        {
            BITCLR(SysData.AlmReg, BIT_FRONTDOOR);
            BITCLR(AlarmStatus, BIT_FRONTDOOR);
        }

        if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_SET) && BITCHK(AlarmStatus, BIT_SURGE))
        {
            BITSET(SysData.AlmReg, BIT_SURGE);      // AC SURGE
        } 
        else 
        {
            BITCLR(SysData.AlmReg, BIT_SURGE);
            BITCLR(AlarmStatus, BIT_SURGE);
        }

        if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_SET) && BITCHK(AlarmStatus, BIT_SURGE))
        {
            BITSET(SysData.AlmReg, BIT_SURGE);      // DC SURGE
        } 
        else 
        {
            BITCLR(SysData.AlmReg, BIT_SURGE);
            BITCLR(AlarmStatus, BIT_SURGE);
        }

        if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_SET) && BITCHK(AlarmStatus, BIT_DCOK))
        {
            BITSET(SysData.AlmReg, BIT_DCOK);
        } 
        else 
        {
            BITCLR(SysData.AlmReg, BIT_DCOK);
            BITCLR(AlarmStatus, BIT_DCOK);
        }
    } 
    else 
    {
        /* Closing else clause */
    }
}
/* ================================================================================================
 * @author      Neale Morris
 * @brief       Sets the Vpgrm (DAC1) output           
 *
 * @pre-req     Respective pin on port B configured for output.
 * @param       None
 * @retval      None
 * ==============================================================================================*/
void DAC_Write(void)
{
    switch (SysData.pflparams.psu_mode)
    {
        case P_CI:
            SysData.dacpgm = (uint16_t)((0.99490 * (4095 * ((double)(SysData.pflparams.rec_iset)) / 
                                    ((double)SysData.pflparams.rec_fs_iset))) - 0.51891);
            break;

        case P_CPOT:
            __NOP();
            break;
        
        case P_CV:
            SysData.dacpgm = (uint16_t)(1.16291 * (4095.0 * ((double)SysData.pflparams.rec_vset) / 
                                    ((double)SysData.pflparams.rec_fs_vset)) - 0.20202);

            if (((double)SysData.Vrdbk.collection[0] >= (SysData.dacpgm - 2)) 
                && ((double)SysData.Vrdbk.collection[0] <= (SysData.dacpgm + 2)))
            {
                SysData.dacpgm += 0;
            }
            else if ((double)SysData.Vrdbk.collection[0] < (SysData.dacpgm - 2))
            {
                if (SysData.dacpgm + 3 >= 0xFFF)    // 06/09/16 was 10
                {
                    SysData.dacpgm = 0xFFF;
                } 
                else 
                {
                    SysData.dacpgm += 3; 
                }
            }
            else if ((double)SysData.Vrdbk.collection[0] > (SysData.dacpgm + 2))
            {
                if (SysData.dacpgm - 3 >= 0)
                {
                    SysData.dacpgm -= 3;
                } 
                else 
                {
                    SysData.dacpgm = 0;
                }
            } 
            else 
            {
                __NOP();
            }
            break;

        case P_OFF:
            SysData.dacpgm = 0x0000;
            break;

        default:
            break;
    }
    
    HAL_DAC_SetValue(&hDAC1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, SysData.dacpgm); 
}
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
static uint8_t UnderVoltageFaultCheck(int16_t min)
{
    if (min > SysData.pflparams.refelec_min_th)
    {
        BITCLR(SysData.AlmReg, BIT_RECFAIL);
        BITCLR(SysData.AlmReg, BIT_UNDERVOLTAGE);
    }    
    else
    {
        BITSET(SysData.AlmReg, BIT_RECFAIL);
        BITSET(SysData.AlmReg, BIT_UNDERVOLTAGE);
    }

    /* Return 1 = Fail or 0 = Pass */
    return (BITCHK(SysData.AlmReg, BIT_UNDERVOLTAGE)) ? 0x01UL : 0x00UL;
}
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
static uint8_t OverProtectionSetpointFault(uint8_t index)
{
    static  uint8_t overProtectionCount[4] = {0};
    
    if (SysData.pflparams.refelec_active == 0x00UL)
    {
        SysData.pflparams.psu_mode = P_CI;
    }
    else
    {
        if (SysData.RefCell[index].avg > SysData.pflparams.refelec_max_th)
        {
            overProtectionCount[index]++;
            
            if (BITCHK(SysData.AlmReg, BIT_RECFAIL) && BITCHK(SysData.TestStaReg, BIT_OVERVOLTAGE))
            {
                __NOP();    // If set, do nothing!!!
            }
            else
            {
                BITSET(SysData.AlmReg, BIT_RECFAIL);
                BITSET(SysData.TestStaReg, BIT_OVERVOLTAGE);
            }
            
            if (overProtectionCount[index] >= 0x08UL)
            {
                BITCLR(SysData.pflparams.refelec_active, index);
            }
            else
            {
                /* Closing else clause */
                __NOP();
            }
        }
        else
        {
            overProtectionCount[index] = 0x00UL;
        }         
    }
            
    /* Return 1 = Fail or 0 = Pass */
    return (BITCHK(SysData.AlmReg, BIT_OVERVOLTAGE)) ? 0x01UL : 0x00UL; 
}
/* ================================================================================================
 *  @author     Neale Morris
 *  @brief      Test if reference cells are within specified range,
 *              find the most negative and positive reference electrode;  
 *              accordingly setting the DAC to drive the saturable core.
 * @pre-req     ADC interupts and DMA must be setup and running prior to calling.
 * @param       None
 * @retval      None
 * ==============================================================================================*/
static void GetRefCells(void)
{
                uint8_t index = 0;
    volatile    int16_t min = 0, max = 0;

    for (index = 0; index < 4; index++) 
    {
        /* if reference cell is not active or exhibiting over voltage do not use 
           said cell in calculation of minimum */
        if (BITCHK(SysData.pflparams.refelec_active, index) && !OverProtectionSetpointFault(index))
        {
            if (min == 0)
            {
                min = SysData.RefCell[index].avg;
                max = SysData.RefCell[index].avg;
            }
            else if (min > 0)
            {
                if (SysData.RefCell[index].avg < min)
                {
                    min = SysData.RefCell[index].avg;
                } 
                else
                {
                    max = SysData.RefCell[index].avg;
                } 
            }
            else
            {
                __NOP();  /* Closing else clause */
            }
        }
        else
        {
            /* Closing else clause */
            __NOP();
        }
    }

    if (SysData.pflparams.psu_mode == P_CPOT)
    {
        if (UnderVoltageFaultCheck(min))
        {
            if (SysData.dacpgm + 45 > 4095)
            {
                SysData.dacpgm = 4095;
            }
            else
            {
                SysData.dacpgm += 45;
            }
        }
        else
        {   
            if ((min > (SysData.pflparams.refelec_dmd_setpt - 10)) 
                && (min < (SysData.pflparams.refelec_dmd_setpt + 10)))
            {
                SysData.dacpgm += 0;
            } 
            else if (min < (SysData.pflparams.refelec_dmd_setpt - 10)) 
            {
                if (SysData.dacpgm >= 4095)
                {
                    SysData.dacpgm = 4095;
                } 
                else 
                {
                    SysData.dacpgm += 10;
                }
            } 
            else if (min > (SysData.pflparams.refelec_dmd_setpt + 10)) 
            {
                if (SysData.dacpgm < 1)
                {
                    SysData.dacpgm = 0;
                } 
                else 
                {
                    SysData.dacpgm -= 10;
                }
            }
            else
            {
                __NOP();
            }
        }
    }
    else
    {
        /* Closing else clause */
        __NOP();
    }

    HAL_DAC_SetValue(&hDAC1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, SysData.dacpgm);
}
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
void TaskDispatch(void)
{
    static  uint32_t LastTick[4] = {0};
    static  uint32_t NewTick     = 0; 
    
    NewTick = HAL_GetTick();

    if ((NewTick - LastTick[0]) > ET_RECT_ON_OFF) 
    {
        LastTick[0] = NewTick;
        RectifierOnOff();
        GetAlarmStatus();
    }
    else if ((NewTick - LastTick[1]) > ET_DACWRITE)
    {
        LastTick[1] = NewTick;
        DAC_Write();
    }
    else if ((NewTick - LastTick[2]) > ET_HEARTBEAT) 
    {
        LastTick[2] = NewTick;
        LED_HeartBeat();
    }
    else if ((NewTick - LastTick[3]) > ET_GETREFCELLS) 
    {
        LastTick[3] = NewTick;
        GetRefCells();
    } 
    else 
    {
        __NOP();
    }
}
/* ================================================================================================
 *  FINIS
 * ==============================================================================================*/

