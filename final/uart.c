/*
 * uart.c
 *
 *  Created on: 26/05/2018
 *      Authors: Liam Laing, George Khella, Connor Adamson
 *  The Tiva comes with a UART module. We can use this to display the information
 *  about the internal state of the heli to the user.
 *  Given the profound usefulness of this we have developed a module that initializes the UART,
 *  then the user can send strings to the terminal using UARTSend(char*)
 * It is important to note that there are limits on how long this string may be,
 * failure to conform to this will invoke undefined behaviour.
 * This code was based on a demo program provided to the ENCE361 2018 class
 */
#include "uart.h"

/* 
 * initialiseUSB_UART: simple procedure that turns on the peripheral, 
 * and configures the packet form that the uart will use
 */
void
initialiseUSB_UART (void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    //
    SysCtlPeripheralEnable(UART_USB_PERIPH_UART);
    SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);
    //
    // Select the alternate (UART) function for these pins.
    //
    GPIOPinTypeUART(UART_USB_GPIO_BASE, UART_USB_GPIO_PINS);
    UARTConfigSetExpClk(UART_USB_BASE, SysCtlClockGet(), BAUD_RATE,
            UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
            UART_CONFIG_PAR_NONE);
    UARTFIFOEnable(UART_USB_BASE);
    UARTEnable(UART_USB_BASE);
}

/*
 * UARTSend: writes each character to the uart buffer
 * for transmission at the peripheral's will   
 */

void
UARTSend (char *pucBuffer)
{
    // Loop while there are more characters to send.
    while(*pucBuffer)
    {
        // Write the next character to the UART Tx FIFO.
        UARTCharPut(UART0_BASE, *pucBuffer);
        pucBuffer++;
    }
}
