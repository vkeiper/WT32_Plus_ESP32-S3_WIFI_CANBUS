#include <stm32f4xx.h>
#include "main.h"
#include "SelfTest.h"
#include "iccpfuncs.h"
#include "iccp_canfuncs.h"

/* Handles ===================================================================================== */
    extern  ADC_HandleTypeDef   hADC1;
    extern  DAC_HandleTypeDef   hDAC1;
    extern  TIM_HandleTypeDef   hTIM3;

/* Objects ===================================================================================== */
    SelfTest_t SelfTest;

/* Variables =================================================================================== */
                uint8_t RampState = RAMPSTATE_INIT;
    volatile    uint8_t TestState = PVSTATE_INIT;
    static      uint32_t t_Current500mS;

/*=================================================================================================
 *  Description:
 *===============================================================================================*/
static uint8_t RefCell100mVCheck(void)
{   
    uint8_t loop_index, retval, refcell_status = 0x00;
    
    t_Current500mS = HAL_GetTick();

    if (HAL_GetTick() > t_Current500mS + 500UL)
    {
        t_Current500mS = HAL_GetTick();

        for (loop_index = 0; loop_index < 4; loop_index++)
        {
            if (SysData.RefCell[loop_index].avg > REFCELL_NEG100MV)
            {
                BITSET(SysData.AlmReg, BIT_RECFAIL);
                BITSET(SysData.TestStaReg, (loop_index + OFFSET_8));
                BITSET(refcell_status, loop_index);
            }
            else
            {
                __NOP();
            }
        }
        
        if (0x00 == refcell_status)
        {
            retval = RETVAL_RAMPALLPASSED;
        }
        else if (0x0F == refcell_status)
        {
            retval = RETVAL_RAMPALLFAILED;
        }
        else
        {
            retval = RETVAL_RAMPSOMEFAILED;
        }
    }
    else
    {
        /* Closing else clause */
        __NOP();
    }

    return retval;
}
/*=================================================================================================
 *  Description:
 *===============================================================================================*/
static uint8_t RampTest(void)
{
           uint8_t index; 
    static uint8_t multiple = 0, retval_ramp, retval_100mv;

    switch (RampState)
    {
        case RAMPSTATE_INIT:
            /* Enable rectifier */
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); 

            /* Timer(3) timebase being reconfigured for normal sampling time. */
            HAL_TIM_Base_Stop_IT(&hTIM3);
            ClearADC_Params();
            hTIM3.Init.Prescaler    = 224;
            hTIM3.Init.Period       = 15624;     // 29.06 mS
            HAL_TIM_Base_Start_IT(&hTIM3);
            t_Current500mS          = HAL_GetTick();
            RampState               = RAMPSTATE_100MVCHK;
            retval_ramp             = RETVAL_NOTDONE;
            break;
    
        case RAMPSTATE_100MVCHK:
            retval_100mv = RefCell100mVCheck();
            if (RETVAL_RAMPALLPASSED == retval_100mv)
            {
                RampState = RAMPSTATE_DONEPASSED;           // Passed, do not continue self test.
            }
            else if (RETVAL_RAMPALLFAILED == retval_100mv)
            {
                RampState = RAMPSTATE_DONEFAILEDNONCRIT;    // All failed, do not continue self test.
            }
            else if (RETVAL_RAMPSOMEFAILED == retval_100mv) // One or more reference cells failed
            {
                RampState = RAMPSTATE_FWTESTINIT;
            }
            else
            {
                /* Closing else clause */
                __NOP();
            }
            break;

        case RAMPSTATE_FWTESTINIT:
            HAL_DAC_SetValue(&hDAC1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (DAC50PCT + DAC1PCT));
            RampState = RAMPSTATE_FWTEST;
            break;

        case RAMPSTATE_FWTEST:
            if (HAL_GetTick() > (t_Current500mS + 500UL))
            {
                t_Current500mS = HAL_GetTick(); 

                if ((SysData.Vrdbk.avg > FRESHWATERVOLTAGE) && ((SysData.Irdbk[0].avg + SysData.Irdbk[1].avg) < HALFCURRENT))   
                {
                    RampState = RAMPSTATE_DONEPASSED;
                }
                else    // Not fresh water continue with test!!!
                {
                    RampState = RAMPSTATE_RECFAIL;
                }
            }
            else
            {
                /* Closing else clause */
                __NOP();
            }
            break;

        case RAMPSTATE_RECFAIL:
            //// Assume DAC output still set at 51% from state above; Rectifier fail test
            if ((SysData.Vrdbk.avg < FRESHWATERVOLTAGE) && ((SysData.Irdbk[0].avg + SysData.Irdbk[1].avg) < HALFCURRENT))   
            {
                RampState = RAMPSTATE_DONEFAILEDCRIT;
            }
            else
            {
                RampState = RAMPSTATE_DACINC;
            }
            break;

        case RAMPSTATE_DACINC:
            HAL_DAC_SetValue(&hDAC1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, ((multiple + 1) * DACSTEP));
            t_Current500mS  = HAL_GetTick();
            RampState       = RAMPSTATE_DACWAIT;
            break;

        case RAMPSTATE_DACWAIT:
            if (HAL_GetTick() > (t_Current500mS + 500UL)) 
            {
                t_Current500mS = HAL_GetTick();
                SelfTest.RampTestRefCellBuf[0][multiple] = SysData.RefCell[0].avg;
                SelfTest.RampTestRefCellBuf[1][multiple] = SysData.RefCell[1].avg;
                SelfTest.RampTestRefCellBuf[2][multiple] = SysData.RefCell[2].avg;
                SelfTest.RampTestRefCellBuf[3][multiple] = SysData.RefCell[3].avg;
                
                if (multiple == 0x00UL)
                {
                    multiple++;
                    RampState = RAMPSTATE_DACINC;
                }
                else
                {
                    RampState = RAMPSTATE_REFREGCHK;
                }
            }
            else
            {
                /* Closing else clause */
                __NOP();
            } 
            break;

       case RAMPSTATE_REFREGCHK:
            for (index = 0; index < 4; index++)
            {
                if (BITCHK(SysData.TestStaReg, (index + OFFSET_8)))
                {
                    if ((SelfTest.RampTestRefCellBuf[index][(multiple - 1)]
                        - SelfTest.RampTestRefCellBuf[index][multiple]) > 30)
                    {
                        BITCLR(SysData.TestStaReg, (index + OFFSET_8));
                    }
                    else
                    {
                        /* Closing else clasue */
                        __NOP();
                    }
                }
                else
                {
                    /* Closing esle clause */
                    __NOP();
                }
            }
                
            if ((multiple <= OFFSET_8) && !BITCHK(SysData.TestStaReg, 8) && !BITCHK(SysData.TestStaReg, 9) 
                && !BITCHK(SysData.TestStaReg, 10) && !BITCHK(SysData.TestStaReg, 11))
            {
                multiple = 0;
                BITCLR(SysData.AlmReg, BIT_RECFAIL);
                RampState = RAMPSTATE_DONEPASSED;
            }
            else if ((multiple <= OFFSET_8) && (BITCHK(SysData.TestStaReg, 8) || BITCHK(SysData.TestStaReg, 9)
                    || BITCHK(SysData.TestStaReg, 10) || BITCHK(SysData.TestStaReg, 11)))
            {
                multiple++;
                RampState = RAMPSTATE_DACINC;
            }
            else // If multiple is greater than eight
            {
                multiple = 0;
                
                // All reference cells failed
                if (BITCHK(SysData.TestStaReg, 8) && BITCHK(SysData.TestStaReg, 9) 
                    && BITCHK(SysData.TestStaReg, 10) && BITCHK(SysData.TestStaReg, 11))
                {
                    RampState = RAMPSTATE_DONEFAILEDCRIT;
                }
                else // One or more reference cell failed
                {
                    RampState = RAMPSTATE_DONEFAILEDNONCRIT;

                    if (BITCHK(SysData.TestStaReg, 8))
                    {
                        BITCLR(SysData.pflparams.refelec_active, 0);
                    }
                    else if (BITCHK(SysData.TestStaReg, 9))
                    {
                        BITCLR(SysData.pflparams.refelec_active, 1);
                    }
                    else if (BITCHK(SysData.TestStaReg, 10))
                    {
                        BITCLR(SysData.pflparams.refelec_active, 2);
                    }
                    else if (BITCHK(SysData.TestStaReg, 11))
                    {
                        BITCLR(SysData.pflparams.refelec_active, 3);
                    }
                    else
                    {
                        /* Closing else clause */
                        __NOP();
                    }
                }
            }
            break;

        // Only state machine exit codes below here!!!!
        case RAMPSTATE_DONEPASSED:
            RampState = RAMPSTATE_INIT;
            retval_ramp = RETVAL_RAMPTESTPASSED;
            break;

        case RAMPSTATE_DONEFAILEDNONCRIT:
            RampState   = RAMPSTATE_INIT;
            retval_ramp = RETVAL_RAMPFAILEDALL;
            break;

        case RAMPSTATE_DONEFAILEDCRIT:        // Enter this state if we fail the rectifier fails
            RampState   = RAMPSTATE_INIT;
            retval_ramp = RETVAL_RAMPRECFAILED;
            break;

        case RAMPSTATE_DONEFAILEDSOME:
            RampState   = RAMPSTATE_INIT;
            retval_ramp = RETVAL_RAMPFAILEDSOME;
            break;

        default:
            break;
    }// end switch    

    return retval_ramp;
}   
/*=================================================================================================
 *  Description:
 *===============================================================================================*/
static void PrecisionVoltageIsGood(uint8_t channel)
{
    uint8_t bitIndex;

    switch (channel)
    {
        case 0: //// Reference cell #1
            if ((SelfTest.CollectedDataADC[channel] > REFCELL_PCT_LOW(PRC_REFCELL) &&
                (SelfTest.CollectedDataADC[channel] < REFCELL_PCT_HGH(PRC_REFCELL))))
            {
                BITCLR(SysData.TestStaReg, 0);
            }
            else
            {
                BITSET(SysData.TestStaReg, 0);
            }
            break;

        case 1: //// Reference cell #2
            if ((SelfTest.CollectedDataADC[channel] > REFCELL_PCT_LOW(PRC_REFCELL) &&
                (SelfTest.CollectedDataADC[channel] < REFCELL_PCT_HGH(PRC_REFCELL))))
            {
                BITCLR(SysData.TestStaReg, 1);
            }
            else
            {
                BITSET(SysData.TestStaReg, 1);
            }
            break;

        case 2: //// Reference cell #3
            if ((SelfTest.CollectedDataADC[channel] > REFCELL_PCT_LOW(PRC_REFCELL) &&
                (SelfTest.CollectedDataADC[channel] < REFCELL_PCT_HGH(PRC_REFCELL))))
            {
                BITCLR(SysData.TestStaReg, 2);
            }
            else
            {
                BITSET(SysData.TestStaReg, 2);
            }
            break;
        
        case 3: //// Reference cell #4
            if ((SelfTest.CollectedDataADC[channel] > REFCELL_PCT_LOW(PRC_REFCELL) &&
                (SelfTest.CollectedDataADC[channel] < REFCELL_PCT_HGH(PRC_REFCELL))))
            {
                BITCLR(SysData.TestStaReg, 3);
            }
            else
            {
                BITSET(SysData.TestStaReg, 3);
            }
            break;
        
        case 4: //// Current monitor #1
            if ((SelfTest.CollectedDataADC[channel] > REFCELL_PCT_LOW(PRC_CURRENT) &&
                (SelfTest.CollectedDataADC[channel] < REFCELL_PCT_HGH(PRC_CURRENT))))
            {
                BITCLR(SysData.TestStaReg, 4);
            }
            else
            {
                BITSET(SysData.TestStaReg, 4);
            }
            break;

        case 5: //// Current monitor #2
            if ((SelfTest.CollectedDataADC[channel] > REFCELL_PCT_LOW(PRC_CURRENT) &&
                (SelfTest.CollectedDataADC[channel] < REFCELL_PCT_HGH(PRC_CURRENT))))
            {
                BITCLR(SysData.TestStaReg, 5);
            }
            else
            {
                BITSET(SysData.TestStaReg, 5);
            }
            break;
        
        case 6:
            // KD made changes to board, channel 7 is now voltage monitor!!!
            __NOP();
            break;

        case 7: //// Voltage monitor #1
            if ((SelfTest.CollectedDataADC[channel] > REFCELL_PCT_LOW(PRC_VOLTAGE) &&
                (SelfTest.CollectedDataADC[channel] < REFCELL_PCT_HGH(PRC_VOLTAGE))))
            {
                BITCLR(SysData.TestStaReg, 6);
            }
            else
            {
                BITSET(SysData.TestStaReg, 6);
            }
            break;

        default:
            break;
    }

    for (bitIndex = 0x00UL; ((bitIndex < 0x07UL) && (channel > MAX_CHANNELS)); bitIndex++)
    {
        if (BITCHK(SysData.TestStaReg, bitIndex))
        {
            BITSET(SysData.TestStaReg, BIT_PVTESTFAILED);
            break;
        }
        else
        {
            /* Closing else clause */
            __NOP();
        }
    }
}
/*=================================================================================================
 *  Description:
 *===============================================================================================*/
static void PrecisionVoltageAverage(uint8_t Channel)
{
    switch (Channel)
    {
        case 0:
            SelfTest.CollectedDataADC[Channel] = SysData.RefCell[0].avg;
            break;
        
        case 1:
            SelfTest.CollectedDataADC[Channel] = SysData.RefCell[1].avg;
            break;

        case 2:
            SelfTest.CollectedDataADC[Channel] = SysData.RefCell[2].avg;
            break;

        case 3:
            SelfTest.CollectedDataADC[Channel] = SysData.RefCell[3].avg;
            break;

        case 4:
            SelfTest.CollectedDataADC[Channel] = SysData.Irdbk[0].avg;
            break;

        case 5:
            SelfTest.CollectedDataADC[Channel] = SysData.Irdbk[1].avg;
            break;
       
        case 6:
                // KD made changes to board, channel 7 is now voltage monitor!!!
            break;

        case 7:
            SelfTest.CollectedDataADC[Channel] = SysData.Vrdbk.avg;
            break;

        default:
            break;
    }
}
/*=================================================================================================
 * @author  Neale Morris
 * @brief   Performs ICCP self test; executes on initial power up and every 8 hours of operation
 *          MUX: Inhibit -> PC6
 *          MUX: A       -> PC7
 *          MUX: B       -> PC8
 *          MUX: C       -> PC9
 *
 *          INH     C       B       A   Channel
 *          -----------------------------------------------------
 *          H       -       -       -   Not applicable
 *          L       L       L       L   0 (Reference Cell 1)
 *          L       L       L       H   1 (Reference Cell 2)
 *          L       L       H       L   2 (Reference Cell 3)
 *          L       L       H       H   3 (Reference Cell 4)
 *          L       H       L       L   4 (1 Current XDCR ESPM)
 *                                        (1 Current XDCR LQBLD)
 *          L       H       L       H   5 (2 Current XDCR ESPM)
 *                                        (2 Current XDCR LQBLD)
 *          L       H       H       L   6 (3 Current XDCR LQBLD)
 *          L       H       H       H   7 (4 Current XDCR LQBLD)      
 *===============================================================================================*/
static void PrecisionVoltageChannel(uint8_t channel)
{   
    switch (channel)
    {
        case 0: //// Reference Cell 1
            HAL_GPIO_WritePin(GPIOC, INHBT, SETMUX);
            HAL_GPIO_WritePin(GPIOC, CHNLA, RSTMUX);
            HAL_GPIO_WritePin(GPIOC, CHNLB, RSTMUX);
            HAL_GPIO_WritePin(GPIOC, CHNLC, RSTMUX);
            HAL_GPIO_WritePin(GPIOC, INHBT, RSTMUX);
            break;

        case 1: //// Reference Cell 2
            HAL_GPIO_WritePin(GPIOC, INHBT, SETMUX);
            HAL_GPIO_WritePin(GPIOC, CHNLA, SETMUX);
            HAL_GPIO_WritePin(GPIOC, CHNLB, RSTMUX);
            HAL_GPIO_WritePin(GPIOC, CHNLC, RSTMUX);
            HAL_GPIO_WritePin(GPIOC, INHBT, RSTMUX);
            break;

        case 2: //// Reference Cell 3
            HAL_GPIO_WritePin(GPIOC, INHBT, SETMUX);
            HAL_GPIO_WritePin(GPIOC, CHNLA, RSTMUX);
            HAL_GPIO_WritePin(GPIOC, CHNLB, SETMUX);
            HAL_GPIO_WritePin(GPIOC, CHNLC, RSTMUX);
            HAL_GPIO_WritePin(GPIOC, INHBT, RSTMUX);
            break;

        case 3: //// Reference Cell 4
            HAL_GPIO_WritePin(GPIOC, INHBT, SETMUX);
            HAL_GPIO_WritePin(GPIOC, CHNLA, SETMUX);
            HAL_GPIO_WritePin(GPIOC, CHNLB, SETMUX);
            HAL_GPIO_WritePin(GPIOC, CHNLC, RSTMUX);
            HAL_GPIO_WritePin(GPIOC, INHBT, RSTMUX);
            break;

        case 4: //// Current Monitor 1
            HAL_GPIO_WritePin(GPIOC, INHBT, SETMUX);
            HAL_GPIO_WritePin(GPIOC, CHNLA, RSTMUX);
            HAL_GPIO_WritePin(GPIOC, CHNLB, RSTMUX);
            HAL_GPIO_WritePin(GPIOC, CHNLC, SETMUX);
            HAL_GPIO_WritePin(GPIOC, INHBT, RSTMUX);
            break;

        case 5: //// Current Monitor 2
            HAL_GPIO_WritePin(GPIOC, INHBT, SETMUX);
            HAL_GPIO_WritePin(GPIOC, CHNLA, SETMUX);
            HAL_GPIO_WritePin(GPIOC, CHNLB, RSTMUX);
            HAL_GPIO_WritePin(GPIOC, CHNLC, SETMUX);
            HAL_GPIO_WritePin(GPIOC, INHBT, RSTMUX);
            break;

        case 6: // KD made changes to board, channel 7 is now voltage monitor!!!
            __NOP();
            break;
        
        case 7: // Voltage Monitor
            HAL_GPIO_WritePin(GPIOC, CHNLA, SETMUX);
            HAL_GPIO_WritePin(GPIOC, CHNLB, SETMUX);
            HAL_GPIO_WritePin(GPIOC, CHNLC, SETMUX);
            HAL_GPIO_WritePin(GPIOC, INHBT, RSTMUX);
            break;

        default:
            HAL_GPIO_WritePin(GPIOC, INHBT, SETMUX);    //// Disable multiplexor!!!!
            break;
    }
}
/* ================================================================================================
 *  Description:
 * ==============================================================================================*/
uint8_t RunSelfTest(void)
{
    static  uint8_t     retval_st, channel;
    static  uint32_t    t_Current128mS;

    switch (TestState)
    {   
        case PVSTATE_INIT:
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);   //// Disable rectifier

            /* Reconfigured timer for precision voltage acquisitions */
            HAL_TIM_Base_Stop_IT(&hTIM3);
            ClearADC_Params();
            channel                 = 0;
            hTIM3.Init.Prescaler    = 39;
            hTIM3.Init.Period       = 2249;     //// 1 ms  
            HAL_TIM_Base_Init(&hTIM3);
            HAL_TIM_Base_Start_IT(&hTIM3);
                    
            if (TMRSELFTEST == AppState)
            {
                CANFunction_205_TX(TESTBGN);    //// Timer initiated CAN report
            } 
            else 
            { 
                /* Closing else clause */
                __NOP();
            }

            SysData.pflparams.lastpsu_mode  = SysData.pflparams.psu_mode;
            t_Current128mS                  = HAL_GetTick();
            TestState                       = PVSTATE_TEST;
            retval_st                       = RETVAL_NOTDONE;
            break;
        
        case PVSTATE_TEST:
            PrecisionVoltageChannel(channel);   //// Select precision voltage channel.
            if ((HAL_GetTick() > (t_Current128mS + 128UL)) && (channel <= MAX_CHANNELS))
            {
                t_Current128mS = HAL_GetTick();
                __NOP();
                PrecisionVoltageAverage(channel);
                PrecisionVoltageIsGood(channel);
                channel++;
            }
            else if (channel > MAX_CHANNELS)
            {   //// Did we fail the precision voltage test.
                if (!BITCHK(SysData.TestStaReg, BIT_PVTESTFAILED))
                {
                    TestState = RAMPSTATE_TEST;
                }
                else
                {
                    TestState = STATE_FAILEDMANMODE;
                }
            }
            else
            {
                /* Closing else clause */
                __NOP();
            }
            break;

        case RAMPSTATE_TEST:
            retval_st = RampTest();
            switch (retval_st)
            {
                case RETVAL_RAMPTESTPASSED:             //// 0x03UL
                    TestState = STATE_PASSEDTEST;
                    break;

                case RETVAL_RAMPFAILEDALL:              //// 0x04UL
                    TestState = STATE_FAILEDMANMODE;
                    break;

                case RETVAL_RAMPFAILEDSOME:             //// 0x05UL
                    TestState = STATE_FAILEDNONCRIT;
                    break;

                case RETVAL_RAMPRECFAILED:              //// 0x06UL
                    TestState = STATE_FAILEDCRIT;
                    break;

                default:
                    /* RETVAL_NOTDONE */
                    break;
            }
            break;

        case STATE_FAILEDMANMODE:
            SysData.pflparams.psu_mode = P_CI;  //// May need to set default current!
            CANFunction_205_TX(SENDRPT);
            TestState = PVSTATE_INIT;
            retval_st = RETVAL_DONE;
            break;

        case STATE_FAILEDCRIT:
            SysData.pflparams.psu_mode = P_CI;            
            CANFunction_205_TX(SENDRPT);
            TestState = PVSTATE_INIT;
            retval_st = RETVAL_FAILED;
            break;
        
        case STATE_FAILEDNONCRIT:
        case STATE_PASSEDTEST:
            SysData.pflparams.psu_mode = SysData.pflparams.lastpsu_mode;
            CANFunction_205_TX(SENDRPT);
            TestState = PVSTATE_INIT;
            retval_st = RETVAL_DONE;
            break;

        default:
            assert_param("No equivalent run self test state");
            break;
    }
    
    return retval_st;
}
/* ================================================================================================
 *  FINIS
 * ==============================================================================================*/

