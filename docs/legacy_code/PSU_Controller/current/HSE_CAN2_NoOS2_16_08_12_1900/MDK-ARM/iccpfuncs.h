#ifndef __ICCPFUNCS_H
#define __ICCPFUNCS_H

#include <stdint.h>

#define tickspersec 10

/* Enumerations ================================================================================ */
typedef enum Tag_psu_mode_t
{
    P_CV                = 0, 
    P_CI                = 1, 
    P_CPOT              = 2, 
    P_OFF               = 3
} psu_mode_t;

typedef enum Tag_Rect_t
{
    LBLD                = 0, 
    ESPM                = 1
} Rect_t;
 
typedef enum Tag_init_state_t 
{
    I_OFF               = 0, 
    I_HLLO              = 1, 
    I_PFL               = 2, 
    I_STAQRY            = 3, 
    I_STARSP            = 4, 
    I_CTLPSU            = 5, 
    I_FAIL              = 6
} init_state_t;

typedef enum Tag_modestate_t 
{
    INIT_M              = 0, 
    RUN                 = 1, 
    FAULT               = 2, 
    CONFIG              = 3, 
    DBG                 = 4, 
    MANUAL_M            = 5, 
    SAFE_M              = 6
} modestate_t;

typedef enum Tag_rectcntl_state_t
{   INIT                = 0, 
    ON                  = 1, 
    OFF                 = 2, 
    MANUAL              = 3, 
    SAFE                = 4
} rectcntl_state_t;

enum Tag_BitsAlmReg {
    BIT_OUTPUT_X        = 0,
    BIT_DCOK            = 1,
    BIT_ACOK            = 2,
    BIT_SUMFLT          = 3,
    BIT_MANMODE         = 4,    //// Manual mode or Automated
    BIT_OUTPUT          = 5,
    BIT_RECFAIL         = 6,
    BIT_CANFLT          = 7,
    BIT_SURGE           = 8,
    BIT_IBALANCE        = 9,
    BIT_FRONTDOOR       = 10,
    BIT_LSTF            = 11,   //// Last self test failed
    BIT_OVERVOLTAGE     = 12,
    BIT_UNDERVOLTAGE    = 13,
    BIT_A               = 14,
    BIT_B               = 15,
};

enum Tag_BitsTestStaReg {
    BIT_PVTESTFAILED    = 7
};

enum Tag_ElapsedTicks { 
    ET_RECT_ON_OFF      = 99, 
    ET_DACWRITE         = 249, 
    ET_HEARTBEAT        = 499,
    ET_GETREFCELLS      = 999
};

enum Tag_Maxima { 
    MAX_REFCELLS        = 4, 
    MAX_ALARMS          = 5,
    MAX_CHANNELS        = 7,
    MAX_SAMPLES         = 129
};

/* Structures ================================================================================== */
typedef struct Tag_Record_t 
{ 
                int16_t     avg;
    volatile    int16_t     collection[129];
                uint32_t    sum; 
} Record_t;

// ICCP Run Time Profile Struct
typedef struct Tag_IccpPrfParam_t 
{
    psu_mode_t  psu_mode;               // P_CV, P_CI, P_CPOT, P_OFF
    psu_mode_t  lastpsu_mode;           // P_CV, P_CI, P_CPOT, P_OFF
    int16_t     refelec_min_th;         // resolution 1mv/bit  -2048 = -0.5Vdc  +2047= +0.5Vdc
    int16_t     refelec_max_th;         // resolution 1mv/bit  -2048 = -0.5Vdc  +2047= +0.5Vdc
    int16_t     refelec_dmd_setpt;      // programmable set point for reference electrodes
    uint16_t    rec_vset;               // resolution 1mv/bit 4095 = Full Scale
    uint16_t    rec_vset_max_th;        // resolution 100mv/bit 4095 = Full Scale
    uint16_t    rec_vset_min_th;        // resolution 100mv/bit
    uint16_t    rec_fs_vset;            // resolution = vset_fs / 4095 ex 63V/4095 = 15mA/bit.
                                        // for a 54V rect. This would be the 0V point/63v=630bits
    uint16_t    rec_iset;               // resolution = iset_fs/4095 ex. 125A/4095 = 30mA/bit
    uint16_t    rec_iset_max_th;        // resolution 1A/bit
    uint16_t    rec_iset_min_th;        // resolution 1A/bit
    uint16_t    rec_fs_iset;            // resolution 1A/bit 
    uint16_t    alarm_reg_mask;         // 0=bit masked 1=active, same bit mapping as alarm register
    uint16_t    self_test_int;          // interval self test is run at in minutes/bit 0= disbaled
    uint8_t     adc_smp_int;            // interval adc samples are placed into 128 sample array 
                                        // mS/bit 0=uses real time values for debug purposes
    uint8_t     control_int;            // interval the rectifer output current is updated based
                                        // on the ref elec AVG buffers. second/bit 
    uint32_t    tsts_active;            // bit 0: Self test - Precision voltage
                                        // bit 1: Self test - Ramp Drive
                                        // bit 2: Self test - 100 mV
                                        // bit 3: Self test - Fresh water 
                                        // bit 4:
                                        // bit 5:
    uint8_t     refelec_active;         // bit 0: Reference Cell #1
                                        // bit 1: Reference Cell #2
                                        // bit 2: Reference Cell #3
                                        // bit 3: Reference Cell #4
                                        // bit 4: Reference Cell #1 Overprotection
                                        // bit 5: Reference Cell #2 Overprotection
                                        // bit 6: Reference Cell #3 Overprotection
                                        // bit 7: Reference Cell #4 Overprotection
    uint8_t     refeleclongname[4][16]; // 16 character long name descriptor for each 
                                        // reference electrode sensor
} IccpPrfParam_t;                       // Iccp Profile Parameters

typedef struct Tag_SysData_t
{
	IccpPrfParam_t  pflparams;
	init_state_t    init_state;     // {I_OFF, I_HLLO, I_PFL, I_STA, I_FAIL } init_state_t;
	uint16_t		AlmReg;		    // bit  0: output on\off 0=off
                                    // bit  1: DCOK 
                                    // bit  2: ACOK
                                    // bit  3: RECT SUM FLT
                                    // bit  4: Manual Mode 0=auto 1=manual
                                    // bit  5: output on\off 0=off
                                    // bit  6: ref_elec 0=passing 1=at least one refelec is failed
                                    // bit  7: communications fault CAN stopped for >2 sec
                                    // bit  8: surge protector flt detected
                                    // bit  9: Current Imbalance
                                    // bit 10: Door Status 0=closed 1=open
                                    // bit 11: Last Self Test 0=failed 1=passed
                                    // bit 12: Reference cell overvoltage
                                    // bit 13: Reference cell undervoltage
                                    // bit 14:  
                                    // bit 15:
	uint32_t        TestStaReg;     // bit 0-3:  Precision voltage reference cells (0=pass, 1=fail)
                                    // bit 4-5:  Precision voltage Current monitors(0=pass, 1=fail) 
                                    // bit 6:    Precision voltage voltage monitor (0=pass, 1=fail)
                                    // bit 7:    Precision voltage test failed     (0=pass, 1=fail)
                                    // bit 8-11: Ramp drive Reference cells 100mV  (0=pass, 1=fail)
                                    // bit 12:   Reference cells fresh water
                                    // bit 13:   Rectifier fail
                                    // bit 13:
                                    // bit 14:
                                    // bit 15:
	uint8_t         ModelNumber[16];
	uint8_t		    SerialNumber[16];
	uint8_t		    FirmwareVers[16];
    uint8_t         AdcSmplIndx;
    uint8_t         Temperature;
    uint8_t	        DC_ADDR;
    uint8_t         nodeadr_adc_flag;   // confirms adc node address
	uint16_t        nodeadr_adc;
    uint16_t        IrdNode;
	Record_t        RefCell[4];
    Record_t        Vrdbk;
	Record_t        Irdbk[4];
	uint16_t	    NODE_ADDR;
	uint16_t        dacpgm;
	uint16_t        stacnt;         // counts of successful status transmissions to LCD
	uint16_t        Status;         // bit 0  received request for "cn_hello" 
                                    // bit 1  sent cn_hello response 
                                    // bit 2  rxd request for PROFILE DATA "cn_prfupl" 
                                    // bit 3  sent all profile data 
                                    // bit 4  rxd request for status "cn_staqry" 
                                    // bit 5  sent all status data  
                                    // bit 6  no LCD host error no can hello rxd after 
                                    // 1 minute power on. 
                                    // bit 7  Flash write or read error, cant get or 
                                    // set config params
                                    // bit 8  CAN2 no cn_hello response from Liqua Blades
} SysData_t;

/* Macros ====================================================================================== */
#define BITCHK(Variable, Bit_number) (((Variable) >> (Bit_number)) & 1UL)
#define BITCLR(Variable, Bit_number) ((Variable) &= ~(1UL << (Bit_number)))
#define BITINV(Variable, Bit_number) ((Variable) ^= (1UL << (Bit_number)))
#define BITSET(Variable, Bit_number) ((Variable) |= (1UL << (Bit_number)))

/**
 *  The following macros are used to converted all ADC samples, designated by (x) to the
 *  appropriate fixed point values of the named macro function.  The SCALE_CPOT_RDBK(x) macro 
 *  function executes the inverse of the level shift circuitry for each reference cell input.
 *
 *  Level Shift eqn:    Gain(x) + Offset = 2(x) + 1.243
 *  Note:               0.61050061 = 2500.0/4095.0
 *                      1.243 has been multiplied by 1000, accounting for resolution
 *                  
 */


#define SCALE_CV_RDBK(x)    (int16_t)((x) * (double)(SysData.pflparams.rec_fs_vset / 4095.0))
#define SCALE_CI_RDBK(x)    (int16_t)(10.0 * (x) * (122/4095.0))   //// 07/21/2016 was 132
#define SCALE_CPOT_RDBK(x)  (int16_t)((((0.6105) * ((double)(x))) - 1243.0)/2.0)

/* Structure variables ========================================================================== */
extern SysData_t  SysData;

/* Furnction prototypes ========================================================================= */
        void    Init_Iccp(void);
        void    IccpDoIccp(void);
        void    TaskDispatch(void);
        void    WriteProfileStructToFlash(IccpPrfParam_t *pfl, uint8_t prflidx);
        void    ReadProfileStructFromFlash(IccpPrfParam_t *pfl, uint8_t prflidx);
        void    CopyProfileStructRam2Ram(IccpPrfParam_t *pflsrc, IccpPrfParam_t *pfldst);
static  void    GetRefCells(void);
static  void    LED_HeartBeat(void);
static  void    RectifierOnOff(void);
static  void    GetAlarmStatus(void);
static  uint8_t UnderVoltageFaultCheck(int16_t);
static  uint8_t OverProtectionSetpointFault(uint8_t);
        uint8_t GetNodeAddress(uint16_t);


#endif /* __ICCPFUNCS_H */

/* ================================================================================================
 *  FINIS
 * ==============================================================================================*/

