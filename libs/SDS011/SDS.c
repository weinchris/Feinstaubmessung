/*
* Licensee agrees that the example code provided to Licensee has been developed and released by Bosch solely as an example to be used as a potential reference for Licensee�s application development. 
* Fitness and suitability of the example code for any use within Licensee�s applications need to be verified by Licensee on its own authority by taking appropriate state of the art actions and measures (e.g. by means of quality assurance measures).
* Licensee shall be responsible for conducting the development of its applications as well as integration of parts of the example code into such applications, taking into account the state of the art of technology and any statutory regulations and provisions applicable for such applications. Compliance with the functional system requirements and testing there of (including validation of information/data security aspects and functional safety) and release shall be solely incumbent upon Licensee. 
* For the avoidance of doubt, Licensee shall be responsible and fully liable for the applications and any distribution of such applications into the market.
* 
* 
* Redistribution and use in source and binary forms, with or without 
* modification, are permitted provided that the following conditions are 
* met:
* 
*     (1) Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer. 
* 
*     (2) Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in
*     the documentation and/or other materials provided with the
*     distribution.  
*     
*     (3)The name of the author may not be used to
*     endorse or promote products derived from this software without
*     specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR 
*  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
*  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
*  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
*  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
*  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
*  IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
*  POSSIBILITY OF SUCH DAMAGE.
*/
/*----------------------------------------------------------------------------*/
/**
* @ingroup APPS_LIST
*
* @defgroup EXTENSION_BUS_UART uartt
* @{
*
* @brief This example outlines the implementation of UART extension bus to receive and transmit data.
*
* @details Extension UART outlines the implementation of UART extension bus to receive and transmit data.
*
*
* @file SDS.c
**/

#include <stdio.h>
#include "BCDS_HAL.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "BSP_UART.h"
#include "BCDS_CmdProcessor.h"
#include "BCDS_Assert.h"
#include "BCDS_MCU_UART.h"
#include "SDS.h"

HWHandle_T uartHandle;
static uint8_t receivedByte;


int value;
uint8_t buffer[10];
float pm10 = 0;
float pm25 = 0;
int len = -1;
int pm10_serial = 0;
int pm25_serial = 0;
int checksum_is;
int checksum_ok = 0;
int error = 1;

CmdProcessor_T *AppCmdProcessorHandle; /**< Application Command Processor Handle */

static void HAL_UART_Events(UART_T uart, struct MCU_UART_Event_S event){}

void UARTrecieve(void){

	for(;;){
		Retcode_T returnValue = RETCODE_OK;
		returnValue = MCU_UART_Receive(uartHandle,  &receivedByte, 1UL);
	    		if(returnValue!=RETCODE_OK){
	    			continue;
	    		}

	    value=receivedByte;

		switch (len) {
			case (0): if (value != 170) { len = -1; }; break;
			case (1): if (value != 192) { len = -1; }; break;
			case (2): pm25_serial = value; checksum_is = value; break;
			case (3): pm25_serial += (value << 8); checksum_is += value; break;
			case (4): pm10_serial = value; checksum_is += value; break;
			case (5): pm10_serial += (value << 8); checksum_is += value; break;
			case (6): checksum_is += value; break;
			case (7): checksum_is += value; break;
			case (8): if (value == (checksum_is % 256)) { checksum_ok = 1; } else { len = -1; }; break;
			case (9): if (value != 171) { len = -1; }; break;
		}
		len++;
		if (len == 10 && checksum_ok == 1) {
			pm10 = (float)pm10_serial/10.0;
			pm25 = (float)pm25_serial/10.0;
			len = 0; checksum_ok = 0; pm10_serial = 0.0; pm25_serial = 0.0; checksum_is = 0;
			error = 0;
			printf("PM10: %f PM25: %f\n\r", pm10, pm25);
		}


}}

void UARTInit(void)
{
	printf("Initalizing UART \n\r");
	Retcode_T returnVal = RETCODE_OK;


	    returnVal = BSP_UART_Connect();
	    if (RETCODE_OK != returnVal)
	            {printf("No UART connect \n\r");}

	    uartHandle = BSP_UART_GetHandle();
	    if (0 == uartHandle)
	    {
	    	printf("No UART handle \n\r");
	    }

	    returnVal = MCU_UART_Initialize(uartHandle, HAL_UART_Events);
	    if (returnVal != RETCODE_OK)
	    {
	        printf("Init UART for receiving failed \n\r");
	    }

	    returnVal = BSP_UART_Enable();
	    if (returnVal != RETCODE_OK)
	    {
	        printf("Enabling UART for receiving failed \n\r");
	    }

	    if (returnVal == RETCODE_OK)
	    {
	    	printf("UART initialize succeeded \n\r");
	    }
}

void UARTTask(void)
{
  xTaskHandle taskHandle = NULL;
  xTaskCreate(
	UARTrecieve,                 // function that implements the task
    (const char * const) "reciveUART", // a name for the task
    configMINIMAL_STACK_SIZE,       // depth of the task stack
    NULL,                           // parameters passed to the function
    tskIDLE_PRIORITY,               // task priority
    taskHandle                      // pointer to a task handle for late reference
  );
}


/**@} */
/** ************************************************************************* */
