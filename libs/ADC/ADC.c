/* module includes ********************************************************** */

/* system header files */
#include "BCDS_Basics.h"

#include <stdint.h>
#include <stdio.h>

/* additional interface header files */
#include "FreeRTOS.h"
#include "timers.h"

/* own header files */
#include "ADC.h"

#include "em_cmu.h"
#include "em_adc.h"

/* constant definitions ***************************************************** */

#define TIMER_AUTORELOAD_ON pdTRUE

#define ONESECONDDELAY      UINT32_C(1000)       /* one second is represented by this macro */
#define TIMERBLOCKTIME      UINT32_C(0xffff)     /* Macro used to define blocktime of a timer */
#define TIMEBASE            UINT32_C(0)          /* Macro used to define the Timebase for the ADC */
#define ADCFREQUENCY        UINT32_C(9000000)    /* Macro Used to define the frequency of the ADC Clock */
#define NUMBER_OF_CHANNELS  UINT32_C(2)

/* local variables ********************************************************** */

xTimerHandle scanAdcTimerHandle;

/* global variables ********************************************************* */

/* inline functions ********************************************************* */

/* local functions ********************************************************** */

/**
 * @brief Timer Callback; Scans and prints out the ADC Channels' Values
 */
extern void scanAdc(xTimerHandle pxTimer)
{
    /* Initialize the Variables */
    (void) (pxTimer);
    uint32_t _adc0ChData = 0;
    uint8_t _channelsScanned = 0;

    /* Start the ADC Scan */
    ADC_Start(ADC0, adcStartScan);

    for (_channelsScanned = 0; _channelsScanned < NUMBER_OF_CHANNELS; _channelsScanned++) {
        /* Wait for Valid Data */
        while (!(ADC0->STATUS & ADC_STATUS_SCANDV));

        /* Read the Scanned data */
        _adc0ChData = 0xFFF & ADC_DataScanGet(ADC0);
        printf("ADC Channel %ld = %ld\r\n", ((ADC0->STATUS & _ADC_STATUS_SCANDATASRC_MASK) >> _ADC_STATUS_SCANDATASRC_SHIFT), _adc0ChData);
    }
}

/**
 * @brief Configure the ADC to scan multiple channels;
 *        Initialize and Start a time to scan the ADC
 */
extern void scanAdcInit(void)
{
    /* Initialize Configuration ADC Structures */
    ADC_Init_TypeDef     adc0_init_conf     = ADC_INIT_DEFAULT;
    ADC_InitScan_TypeDef adc0_scaninit_conf = ADC_INITSCAN_DEFAULT;

    /* Enable Clocks for the ADC */
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_ADC0, true);

    /* Configure the ADC Initialization Structure */
    adc0_init_conf.timebase = ADC_TimebaseCalc(TIMEBASE);
    adc0_init_conf.prescale = ADC_PrescaleCalc(ADCFREQUENCY, TIMEBASE);
    ADC_Init(ADC0, &adc0_init_conf);

    /* Configure the ADC Scan Structure */
    adc0_scaninit_conf.reference = adcRefVDD;
    adc0_scaninit_conf.input     = ADC_SCANCTRL_INPUTMASK_CH5 |
                                   ADC_SCANCTRL_INPUTMASK_CH6 ;

    ADC_InitScan(ADC0, &adc0_scaninit_conf);

    /* Setup and Start the timer to scan the ADC Channels */
    scanAdcTimerHandle = xTimerCreate(
        (const char *) "ADC read", ONESECONDDELAY,
        TIMER_AUTORELOAD_ON, NULL, scanAdc);

    xTimerStart(scanAdcTimerHandle, TIMERBLOCKTIME);
}

/** ************************************************************************* */
