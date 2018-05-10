/*
* Licensee agrees that the example code provided to Licensee has been developed and released by Bosch solely as an example to be used as a potential reference for Licensee’s application development. 
* Fitness and suitability of the example code for any use within Licensee’s applications need to be verified by Licensee on its own authority by taking appropriate state of the art actions and measures (e.g. by means of quality assurance measures).
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
* @ingroup EXTENSION_BUS_UART
*
* @defgroup BSP_EXT_UART BSP_EXT_UART
* @{
*
* @brief UART hardware interface module
*
* @details This file provides the Implementation of UART hardware interface module which contains API's having implementation which are closer to hardware.
*
*
* @file BSP_UART.c
**/

#include "BCDS_MCU_UART_Handle.h"
#include "BSP_UART.h"
#include "em_gpio.h"
#include "em_cmu.h"


#define EXT_SERIAL_UART_PORT                                                    UART1
#define EXT_SERIAL_UART_CLOCK												    (cmuClock_UART1)
#define EXT_SERIAL_UART_RX_IRQN  										    	UART1_RX_IRQn
#define EXT_SERIAL_UART_TX_IRQN  										    	UART1_TX_IRQn
#define EXT_UART_RX_INTERRUPT_PRIORITY                                          UINT32_C(5)
#define EXT_UART_TX_INTERRUPT_PRIORITY                                          EXT_UART_RX_INTERRUPT_PRIORITY

#define EXT_UART_RX_PORT                                                       (gpioPortB)
#define EXT_UART_RX_PIN                                                        (10)
#define EXT_UART_RX_MODE                                                       (gpioModeInput)


#define EXT_UART_TX_PORT                                                       (gpioPortB)
#define EXT_UART_TX_PIN                                                        (9)
#define EXT_UART_TX_MODE                                                       (gpioModePushPullDrive)


static UART_IRQ_Callback_T EXT_UART_RxISRCallback;
static UART_IRQ_Callback_T EXT_UART_TxISRCallback;
struct MCU_UART_Handle_S EXT_UART_Handle;

/*
 * enumeration of custom return codes
 */
enum BSP_EXT_UART_Retcode_E
{
    RETCODE_BSP_EXT_SERIAL_NOT_INITIALIZED = RETCODE_FIRST_CUSTOM_CODE,
};



Retcode_T BSP_UART_Connect(void)
{
    GPIO_PinModeSet(EXT_UART_RX_PORT, EXT_UART_RX_PIN, EXT_UART_RX_MODE, 0);
    GPIO_PinModeSet(EXT_UART_TX_PORT, EXT_UART_TX_PIN, EXT_UART_TX_MODE, 0);
    CMU_ClockEnable(EXT_SERIAL_UART_CLOCK, true);
    EXT_SERIAL_UART_PORT->ROUTE = UART_ROUTE_RXPEN | UART_ROUTE_TXPEN | UART_ROUTE_LOCATION_LOC2;
    CMU_ClockEnable(EXT_SERIAL_UART_CLOCK, false);
    return RETCODE_OK;
}


Retcode_T BSP_UART_Enable(void)
{
    Retcode_T retcode = RETCODE_OK;
    /* Dont enable if the callback has not been registered */
    if (NULL == EXT_UART_Handle.IrqRxCallback || NULL == EXT_UART_Handle.IrqTxCallback)
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_BSP_EXT_SERIAL_NOT_INITIALIZED);
    }

    if (RETCODE_OK == retcode)
    {
        /*
         * Enable the UART MCU resource
         */
        CMU_ClockEnable(EXT_SERIAL_UART_CLOCK, true);
        /*
         * Clear Pending Interrupt flags
         */
        /* clear interrupt flags */
        EXT_SERIAL_UART_PORT->IFC = _USART_IF_MASK;
        /*
         * Enable receive interrupt flags
         */
        /* Enable the UART RX Not Empty Interrupt */
        EXT_SERIAL_UART_PORT->IEN = USART_IEN_RXDATAV | USART_IEN_RXFULL | USART_IEN_RXOF | USART_IEN_FERR;
        /*
         * Associate the ISR callbacks
         */
        EXT_UART_RxISRCallback = EXT_UART_Handle.IrqRxCallback;
        EXT_UART_TxISRCallback = EXT_UART_Handle.IrqTxCallback;

        /*
         * Set the priority for the UART interface
         */
        NVIC_SetPriority(EXT_SERIAL_UART_RX_IRQN, EXT_UART_RX_INTERRUPT_PRIORITY);
        NVIC_SetPriority(EXT_SERIAL_UART_TX_IRQN, EXT_UART_TX_INTERRUPT_PRIORITY);
        /*
         * Clear Pending interrupt requests
         */
        NVIC_ClearPendingIRQ(EXT_SERIAL_UART_RX_IRQN);
        NVIC_ClearPendingIRQ(EXT_SERIAL_UART_TX_IRQN);
        /*
         * Enable the interrupt requests
         */
        NVIC_EnableIRQ(EXT_SERIAL_UART_RX_IRQN);
        NVIC_EnableIRQ(EXT_SERIAL_UART_TX_IRQN);

        /* Enable the UART */
        /*Enable transmitter and receiver*/
        EXT_SERIAL_UART_PORT->CMD = (USART_CMD_RXEN | USART_CMD_TXEN);
        /*
         * Check if the USART has been enabled
         */
        if (((EXT_SERIAL_UART_PORT->STATUS & _USART_STATUS_RXENS_MASK) == 0) || ((EXT_SERIAL_UART_PORT->STATUS & _USART_STATUS_TXENS_MASK) == 0))
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
        }
    }
    return retcode;
}


Retcode_T BSP_UART_Disable(void)
{
    Retcode_T retcode = RETCODE_OK;

   /* Disable USART interface */
    EXT_SERIAL_UART_PORT->CMD = USART_CMD_RXDIS | USART_CMD_TXDIS | USART_CMD_MASTERDIS;
    /*
     * Check if the USART has been disabled
     */
    if (((EXT_SERIAL_UART_PORT->STATUS & _USART_STATUS_RXENS_MASK) != 0) || ((EXT_SERIAL_UART_PORT->STATUS & _USART_STATUS_TXENS_MASK) != 0))
    {
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);
    }
    if (RETCODE_OK == retcode)
    {
        NVIC_DisableIRQ(EXT_SERIAL_UART_RX_IRQN);
        NVIC_DisableIRQ(EXT_SERIAL_UART_TX_IRQN);
        EXT_UART_RxISRCallback = NULL;
        EXT_UART_TxISRCallback = NULL;
    }
    return retcode;
 }


Retcode_T BSP_UART_Disconnect(void)
{
    GPIO_PinModeSet(EXT_UART_RX_PORT, EXT_UART_RX_PIN, gpioModeDisabled, 0);
    GPIO_PinModeSet(EXT_UART_TX_PORT, EXT_UART_TX_PIN, gpioModeDisabled, 0);
    CMU_ClockEnable(EXT_SERIAL_UART_CLOCK, true);
    EXT_SERIAL_UART_PORT->ROUTE = _UART_ROUTE_RESETVALUE;
    CMU_ClockEnable(EXT_SERIAL_UART_CLOCK, false);
    return RETCODE_OK;
}


HWHandle_T BSP_UART_GetHandle(void)
{

    EXT_UART_Handle.Uart_ptr = UART1;
    EXT_UART_Handle.TransferMode = BCDS_HAL_TRANSFER_MODE_INTERRUPT;
    return (HWHandle_T) &EXT_UART_Handle;
}

void UART1_TX_IRQHandler(void)
{
    if (NULL != EXT_UART_Handle.IrqTxCallback)
    {
        /* All UART interrupt processing is to be done at the MCU level */
        EXT_UART_TxISRCallback((UART_T) &EXT_UART_Handle);
    }

}

void UART1_RX_IRQHandler(void)
{
    if (NULL != EXT_UART_Handle.IrqRxCallback)
    {
        /* All UART interrupt processing is to be done at the MCU level */
        EXT_UART_RxISRCallback((UART_T) &EXT_UART_Handle);
    }
}

/**@} */
/** ************************************************************************* */



