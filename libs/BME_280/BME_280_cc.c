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
 *  @file        
 *
 * Demo application of printing BME280 Environmental data on serialport(USB virtual comport)
 * every one second, initiated by Environmental timer(freertos)
 * 
 * ****************************************************************************/


/* module includes ********************************************************** */

/* own header files */
#include "XdkSensorHandle.h"
#include "BME_280_ih.h"
#include "BME_280_ch.h"

/* system header files */
#include <stdio.h>
#include "BCDS_Basics.h"

/* additional interface header files */
#include "BSP_BoardType.h"
#include "BCDS_BSP_LED.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "BCDS_Environmental.h"
#include "BCDS_Retcode.h"

/* constant definitions ***************************************************** */

/* local variables ********************************************************** */
/* global variables ********************************************************* */
Environmental_Data_T bme280s = { INT32_C(0), UINT32_C(0), UINT32_C(0) };
Environmental_LsbData_T bme280lsb = { INT32_C(0), INT32_C(0), INT32_C(0) };
Environmental_OverSampling_T bme280os = ENVIRONMENTAL_BME280_OVERSAMP_4X;
Environmental_FilterCoefficient_T bme280coeff = ENVIRONMENTAL_BME280_FILTER_COEFF_4;
/* inline functions ********************************************************* */

/* local functions ********************************************************** */

/* global functions ********************************************************* */

/**
 * @brief The function initializes BME(Environmental)
 */
extern void bme_280_init(void)
{
    /* Return value for Timer start */
    Retcode_T returnValue = (Retcode_T) RETCODE_FAILURE;
    Retcode_T returnVal = RETCODE_OK;

    /*initialize Environmental sensor*/
    returnValue = Environmental_init(xdkEnvironmental_BME280_Handle);
    if (RETCODE_OK == returnValue)
    {
        printf("Environmental Sensor initialization Success\r\n");
    }
    else
    {
        printf("Environmental Sensor initialization Failed\r\n");
        returnVal = BSP_LED_Switch(BSP_XDK_LED_R, BSP_LED_COMMAND_ON);
        if (RETCODE_OK != returnVal)
        {
            printf("Turning on of RED LED failed\r\n");

        }
    }

    returnValue = Environmental_setOverSamplingHumidity(
            xdkEnvironmental_BME280_Handle, bme280os);
    if ((RETCODE_OK != returnValue)
            || (ENVIRONMENTAL_OVERSAMP_OUT_OF_RANGE == bme280os))
    {
        returnVal = BSP_LED_Switch(BSP_XDK_LED_R, BSP_LED_COMMAND_ON);
        if (RETCODE_OK != returnVal)
        {
            printf("Turning on of RED LED failed");

        }
    }

    returnValue = Environmental_setOverSamplingPressure(
            xdkEnvironmental_BME280_Handle, bme280os);
    if ((RETCODE_OK != returnValue)
            || (ENVIRONMENTAL_OVERSAMP_OUT_OF_RANGE == bme280os))
    {
        returnVal = BSP_LED_Switch(BSP_XDK_LED_R, BSP_LED_COMMAND_ON);
        if (RETCODE_OK != returnVal)
        {
            printf("Turning on of RED LED failed");

        }
    }

    returnValue = Environmental_setOverSamplingTemperature(
            xdkEnvironmental_BME280_Handle, bme280os);
    if ((RETCODE_OK != returnValue)
            || (ENVIRONMENTAL_OVERSAMP_OUT_OF_RANGE == bme280os))
    {
        returnVal = BSP_LED_Switch(BSP_XDK_LED_R, BSP_LED_COMMAND_ON);
        if (RETCODE_OK != returnVal)
        {
            printf("Turning on of RED LED failed");

        }
    }

    returnValue = Environmental_setFilterCoefficient(
            xdkEnvironmental_BME280_Handle, bme280coeff);
    if ((RETCODE_OK != returnValue)
            || (ENVIRONMENTAL_FILTER_COEFF_OUT_OF_RANGE == bme280coeff))
    {
        returnVal = BSP_LED_Switch(BSP_XDK_LED_R, BSP_LED_COMMAND_ON);
        if (RETCODE_OK != returnVal)
        {
            printf("Turning on of RED LED failed");

        }
    }
}

/** The function to get the Environmental data
 * @brief Gets the data from BME280 Environmental
 *
 * @param[in] pvParameters Rtos task should be defined with the type void *(as argument)
 */
void bme280_getSensorValues(void *pvParameters)
{
    Retcode_T returnValue = (Retcode_T) RETCODE_FAILURE;
    (void) pvParameters;
    returnValue = Environmental_readDataLSB(xdkEnvironmental_BME280_Handle,
            &bme280lsb);
    if (RETCODE_OK != returnValue)
    {
        printf("environmentalReadDataLSB FAILED\r\n");
    }
    returnValue = Environmental_readData(xdkEnvironmental_BME280_Handle,
            &bme280s);
    if (RETCODE_OK != returnValue)
    {
        printf("environmentalReadData FAILED\r\n");
    }
}

/**
 *  @brief API to Deinitialize the PED module
 */
extern void bme_280_deInit(void)
{
    Retcode_T returnValue = (Retcode_T) RETCODE_FAILURE;

    returnValue = Environmental_deInit(xdkEnvironmental_BME280_Handle);
    if (RETCODE_OK == returnValue)
    {
        printf("Environmental sensor Deinit Success\r\n");
    }
    else
    {
        printf("Environmental sensor Deinit Failed\r\n");
    }
}

/** ************************************************************************* */
