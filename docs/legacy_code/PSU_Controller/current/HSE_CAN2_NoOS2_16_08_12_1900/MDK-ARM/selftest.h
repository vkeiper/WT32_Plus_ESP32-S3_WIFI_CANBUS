#ifndef __SELFTEST_H
#define __SELFTEST_H

#include <stdint.h>

/* Variables =================================================================================== */
extern volatile uint8_t TestState;
extern          uint8_t RampState;

/* Structures ================================================================================== */
typedef struct Tag_SelfTest_t
{
    int16_t RampTestRefCellBuf[4][8];
    int16_t CollectedDataADC[8];
    uint8_t  AdcSmplIndx;
} SelfTest_t;

/* Macros ====================================================================================== */
#define INHBT   GPIO_PIN_6
#define CHNLA   GPIO_PIN_7
#define CHNLB   GPIO_PIN_8
#define CHNLC   GPIO_PIN_9
#define SETMUX  GPIO_PIN_SET
#define RSTMUX  GPIO_PIN_RESET

/* ICCP constants */
#define OFFSET_8                    8UL
#define DIFF30MV                    30UL

#define DACSTEP                     256UL
#define DAC1PCT                     41UL
#define DAC25PCT                    1023UL
#define DAC50PCT                    2047UL

#define PRC_REFCELL                 3125UL 
#define PRC_CURRENT                 2350UL
#define PRC_VOLTAGE                 2650UL

#define HALFCURRENT                 1250UL
#define REFCELL_NEG100MV            1708UL
#define FRESHWATERVOLTAGE           140UL 

/* CAN BUS self test report DLC */
#define TESTBGN                     0x00UL
#define SENDRPT                     0x08UL

/* Application state constants */
#define RUNAPP                      0x00UL
#define SAFEMODE                    0x01UL
#define RUNSELFTEST                 0x02UL
#define TMRSELFTEST                 0x03UL

/* Finite state machine Precision voltage constants */
#define PVSTATE_INIT                0x00UL
#define PVSTATE_TEST                0x01UL
#define RAMPSTATE_TEST              0x02UL
#define STATE_FAILEDMANMODE         0x03UL
#define STATE_PASSEDTEST            0x04UL
#define STATE_FAILEDNONCRIT         0x05UL
#define STATE_FAILEDCRIT            0x06UL

/* Finite state machine return contstants */
#define RETVAL_DONE                 0x00UL
#define RETVAL_FAILED               0x01UL
#define RETVAL_NOTDONE              0x02UL
#define RETVAL_RAMPTESTPASSED       0x03UL
#define RETVAL_RAMPFAILEDALL        0x04UL
#define RETVAL_RAMPFAILEDSOME       0x05UL
#define RETVAL_RAMPRECFAILED        0x06UL
#define RETVAL_RAMPALLPASSED        0x07UL
#define RETVAL_RAMPALLFAILED        0x08UL
#define RETVAL_RAMPSOMEFAILED       0x09UL
    
/* Finite state machine Ramp test state constants */ 
#define RAMPSTATE_INIT              0x00UL
#define RAMPSTATE_100MVCHK          0x01UL
#define RAMPSTATE_DONEPASSED        0x02UL
#define RAMPSTATE_DONEFAILEDNONCRIT 0x03UL
#define RAMPSTATE_FWTESTINIT        0x04UL
#define RAMPSTATE_FWTEST            0x05UL
#define RAMPSTATE_RECFAIL           0x06UL
#define RAMPSTATE_DACINC            0x07UL
#define RAMPSTATE_DACWAIT           0x08UL
#define RAMPSTATE_REFREGCHK         0x09UL
#define RAMPSTATE_DONEFAILEDCRIT    0x0AUL
#define RAMPSTATE_DONEFAILEDSOME    0x0BUL
#define RAMPSTATE_DONEFAILEDALL     0x0CUL
                    
#define REFCELL_PCT_LOW(x) (uint16_t)((0.9) * ((double)(x)))
#define REFCELL_PCT_HGH(x) (uint16_t)((1.1) * ((double)(x)))

/* Variables =================================================================================== */
extern SelfTest_t SelfTest;

/* function prototypes ========================================================================= */
        uint8_t RunSelfTest(void);
static  void    PrecisionVoltageIsGood(uint8_t);
static  void    PrecisionVoltageChannel(uint8_t);
static  void    PrecisionVoltageAverage(uint8_t);
static  uint8_t RampTest(void);
static  uint8_t RefCell100mVCheck(void);
static  uint8_t PrecisionVoltage(uint8_t);

#endif /* __SELFTEST_H */

/* ================================================================================================
 *  FINIS
 * ==============================================================================================*/

