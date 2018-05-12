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

#include "em_gpio.h"
#include "em_timer.h"
#include "em_cmu.h"

/* constant definitions ***************************************************** */

#define TIMER_AUTORELOAD_ON pdTRUE

#define ONESECONDDELAY      UINT32_C(1000)       /* one second is represented by this macro */
#define TIMERBLOCKTIME      UINT32_C(0xffff)     /* Macro used to define blocktime of a timer */
#define TIMEBASE            UINT32_C(0)          /* Macro used to define the Timebase for the ADC */
#define ADCFREQUENCY        UINT32_C(9000000)    /* Macro Used to define the frequency of the ADC Clock */
#define NUMBER_OF_CHANNELS  UINT32_C(2)

/* local variables ********************************************************** */

xTimerHandle scanAdcTimerHandle;
Current_External_Sensor current_sensor;

/* global variables ********************************************************* */

/* inline functions ********************************************************* */

/* local functions ********************************************************** */

/**
 * @brief Timer Callback; Scans and prints out the ADC Channels' Values
 */
void scanAdc(xTimerHandle pxTimer)
{
    /* Initialize the Variables */
    (void) (pxTimer);
    uint32_t _adc0ChData = 0;
    double value_in_volts = 0;

    /* Start the ADC Scan */
    ADC_Start(ADC0, adcStartScan);

        /* Wait for Valid Data */
        while (!(ADC0->STATUS & ADC_STATUS_SCANDV));

        /* Read the Scanned data */
        _adc0ChData = 0xFFF & ADC_DataScanGet(ADC0);
        value_in_volts = 2.5 * (_adc0ChData / 4095.0);

        switch (current_sensor)
        {
        case mq9:
        	value_mq9 = value_in_volts;
        	break;
        case mq135:
        	value_mq135 = value_in_volts;
        	break;
        case rain:
        	value_rain = value_in_volts;
        	break;
        default:  // sensor_switching
        	break;

        }

        printf("ADC read: %f\r\n", value_in_volts);
}

/**
 * @brief Configure the ADC to scan multiple channels;
 *        Initialize and Start a time to scan the ADC
 */
void scanAdcInit(void)
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
    adc0_scaninit_conf.input     = ADC_SCANCTRL_INPUTMASK_CH5 ;//|
//                                   ADC_SCANCTRL_INPUTMASK_CH6 ;

    ADC_InitScan(ADC0, &adc0_scaninit_conf);

}

/* Setup and Start the timer to scan the ADC Channels */
void scanAdcTask(void)
{
	    scanAdcTimerHandle = xTimerCreate(
	        (const char *) "ADC read", ONESECONDDELAY,
	        TIMER_AUTORELOAD_ON, NULL, scanAdc);

	    xTimerStart(scanAdcTimerHandle, TIMERBLOCKTIME);
}

void multiplexerSwitchingInit(void)
{
	 // Set Pins for Multiplexer to low
	    GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0);
	    GPIO_PinOutClear(gpioPortA, 0);
	    GPIO_PinModeSet(gpioPortC, 0, gpioModePushPull, 0);
	    GPIO_PinOutClear(gpioPortC, 0);
	    GPIO_PinModeSet(gpioPortC, 1, gpioModePushPull, 0);
	    GPIO_PinOutClear(gpioPortC, 1);

	    current_sensor = mq135;

	    // init timer
	    // Enable clock for TIMER0 module
//	    CMU_ClockEnable(cmuClock_TIMER0, true);
//	    TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
//	    timerInit.prescale = timerPrescale1024;
//	    timerInit.mode = timerModeUp;
//	    TIMER_IntEnable(TIMER0, TIMER_IF_OF);
//	    NVIC_EnableIRQ(TIMER0_IRQn);
//	    TIMER_TopSet(TIMER0, 10);
//	    TIMER_Init(TIMER0, &timerInit);
}

void multiplexerSwitch(void)
{

	for(;;){
	vTaskDelay (4000);

	 switch(current_sensor) {
	 	case mq9:
	 		current_sensor = switching;
	 		printf("switch\n");
	 		GPIO_PinOutClear(gpioPortA, 0);
	 		GPIO_PinOutSet(gpioPortC, 0);
	 		GPIO_PinOutClear(gpioPortC, 1);
	 		vTaskDelay (1000);
	 		current_sensor = rain;
	 		printf("rain\n");
	 		break;
	 	case mq135:
	 		current_sensor = switching;
	 		printf("switch\n");
	 		GPIO_PinOutClear(gpioPortA, 0);
	 		GPIO_PinOutClear(gpioPortC, 0);
	 		GPIO_PinOutClear(gpioPortC, 1);
	 		vTaskDelay (1000);
	 		current_sensor = mq9;
	 		printf("mq9\n");
	 		break;
	 	case rain:
	 		current_sensor = switching;
	 		printf("switch\n");
	 		GPIO_PinOutSet(gpioPortA, 0);
	 		GPIO_PinOutClear(gpioPortC, 0);
	 		GPIO_PinOutClear(gpioPortC, 1);
	 		vTaskDelay (1000);
	 		current_sensor = mq135;
	 		printf("mq135\n");
	 		break;
	 }

	}
}

void multiplexerSwitchingTask(void)
{
	  xTaskCreate(
			  multiplexerSwitch,                 // function that implements the task
	    (const char * const) "multiplexerSwitch", // a name for the task
	    configMINIMAL_STACK_SIZE,       // depth of the task stack
	    NULL,                           // parameters passed to the function
	    tskIDLE_PRIORITY,               // task priority
	    NULL                      // pointer to a task handle for late reference
	  );
}



/** ************************************************************************* */
