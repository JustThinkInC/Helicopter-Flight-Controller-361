/*
 * uart.h
 *
 *  Created on: 26/05/2018
 *      Authors: Liam Laing, George Khella, Connor Adamson
 *  The tiva comes with uart modual. We can use this to display the information about the internal state of the heli to the user. Given the profound usefulness of this we have developed a modual that initalizes the uart,
 *  then the user can send strings to the terminal using UARTSend(char*)
 * It is important to note that there are limits on how long this string may be, faluure to conform to this will invoke undefined behaviour
 * This code was based on a demo program provided to the ENCE361 2018 class
 */

#ifndef UART_H_
#define UART_H_

#include <buttons4.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "stdio.h"
#include "stdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"

#define SYSTICK_RATE_HZ 100
#define SLOWTICK_RATE_HZ 4
#define MAX_STR_LEN 16

//---USB Serial comms: UART0, Rx:PA0 , Tx:PA1
#define BAUD_RATE 9600
#define UART_USB_BASE           UART0_BASE
#define UART_USB_PERIPH_UART    SYSCTL_PERIPH_UART0
#define UART_USB_PERIPH_GPIO    SYSCTL_PERIPH_GPIOA
#define UART_USB_GPIO_BASE      GPIO_PORTA_BASE
#define UART_USB_GPIO_PIN_RX    GPIO_PIN_0
#define UART_USB_GPIO_PIN_TX    GPIO_PIN_1
#define UART_USB_GPIO_PINS      UART_USB_GPIO_PIN_RX | UART_USB_GPIO_PIN_TX


/* enables and configures the uart for transmission, this includes packet format and BAUD_RATE 
 */
void initialiseUSB_UART (void);
/*
 * takes the str provided by the user and writes it to the uarts buffer for transmission at will, not that the uart must be inited before this can be meaningfuly invoked.
 */
void UARTSend (char *pucBuffer);

#endif /* UART_H_ */
