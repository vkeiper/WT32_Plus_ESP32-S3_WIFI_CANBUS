#ifndef __MAIN_H
#define __MAIN_H

#include <stdint.h>

/* Variables =================================================================================== */
extern uint8_t AppState;

/* MACROS ====================================================================================== */

#define ADC_SAMPLE_CYCLES ADC_SAMPLETIME_480CYCLES

/* function prototypes ========================================================================= */
static  void    ADC_Config(void);
static  void    DAC_Config(void);
static  void    GPIO_Config(void);
static  void    TIM3_Config(void);
static  void    GPIO_Defaults(void);
static  void    SystemClock_Config(void);
        void    DAC_Write(void);
        void    myErr_Handler(void);
        void    DataCollection(void);
        void    ClearADC_Params(void);

#endif /* __MAIN_H */

/* ================================================================================================
 *  FINIS
 * ==============================================================================================*/

