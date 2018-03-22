// Milestone1.c - Measure helicopter height by sampling AIN9
//
// Authors:  George Khella, Liam Laing, Connor Adamson
// Original Author: P. J. Bones
//


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "circBufT.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "buttons4.h"

//*****************************************************************************
// Constants
//*****************************************************************************
#define BUF_SIZE 10
#define SAMPLE_RATE_HZ 160

//*****************************************************************************
// Global variables
//*****************************************************************************
static circBuf_t g_inBuffer;    // Buffer of size BUF_SIZE integers (sample values)
static uint32_t g_ulSampCnt;	// Counter for the interrupts

//*****************************************************************************
//
// The interrupt handler for the for SysTick interrupt.
//
//*****************************************************************************
void
SysTickIntHandler(void)
{
	// Initiate a conversion
	ADCProcessorTrigger(ADC0_BASE, 3);
	g_ulSampCnt++;

	// Button polling
	updateButtons();
}

//*****************************************************************************
//
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//
//*****************************************************************************
void
ADCIntHandler(void)
{
	uint32_t ulValue;

	// Get the single sample from ADC0.  ADC_BASE is defined in
	// inc/hw_memmap.h
	ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);
	
	// Place it in the circular buffer (advancing write index)
	writeCircBuf(&g_inBuffer, ulValue);
	
	// Clean up, clearing the interrupt
	ADCIntClear(ADC0_BASE, 3);
}

//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick), ADC, display
//*****************************************************************************
void
initClock(void)
{
	// Set the clock rate to 20 MHz
	SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
		SYSCTL_XTAL_16MHZ);
	
	// Set up the period for the SysTick timer.  The SysTick timer period is
	// set as a function of the system clock.
	SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
	
	// Register the interrupt handler
	SysTickIntRegister(SysTickIntHandler);
	
	// Enable interrupt and device
	SysTickIntEnable();
	SysTickEnable();
}

void
initADC(void)
{
	
	// The ADC0 peripheral must be enabled for configuration and use.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

	// Enable sample sequence 3 with a processor signal trigger.  Sequence 3
	// will do a single sample when the processor sends a signal to start the
	// conversion.
	ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

	
	// Configure step 0 on sequence 3
	ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE | ADC_CTL_END);
	
	// Since sample sequence 3 is now configured, it must be enabled.
	ADCSequenceEnable(ADC0_BASE, 3);

	// Register the interrupt handler
	ADCIntRegister(ADC0_BASE, 3, ADCIntHandler);
	
	// Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
	ADCIntEnable(ADC0_BASE, 3);
}

void
initDisplay(void)
{
	// Intialise the Orbit OLED display
	OLEDInitialise();
}

//*****************************************************************************
//
// Function to display the mean ADC value (10-bit value, note) and sample count.
//
//*****************************************************************************
void
displayMeanVal(uint16_t meanVal, char* units, uint32_t count)
{
	char string[17];  // 16 characters across the display
	OLEDStringDraw("                ", 0, 1); // Clear the display
	// Form a new string for the line.
	usnprintf(string, sizeof(string), "Alt = %d%s", meanVal, units);
	// Update line on display.
	OLEDStringDraw(string, 0, 1);
}


int
main(void)
{
	//Counter in the for loop calculating mean of circular buffer values
	uint16_t i;
	int32_t sum;

	initClock();
	initButtons();
	initADC();
	initDisplay();
	initCircBuf(&g_inBuffer, BUF_SIZE);

	// Enable interrupts to the processor.
	IntMasterEnable();

	uint16_t baseHeight = 0;
	uint16_t currentHeight;
	uint8_t displayStage = 0;
	uint16_t percentage = 0;

	while (1)
	{
		// Background task: calculate the (approximate) mean of the values in the
		// circular buffer and display it, together with the sample number.
		sum = 0;
		for (i = 0; i < BUF_SIZE; i++) {
			sum = sum + readCircBuf(&g_inBuffer);
		}

		// Calculate and display the rounded mean of the buffer contents
		currentHeight = (2 * sum + BUF_SIZE) / 2 / BUF_SIZE;

		if (baseHeight == 0) {
			baseHeight = currentHeight;
		}
		else {
			// Calculate helicopter altitude as a rounded percentage
			percentage = ((2 * (baseHeight - currentHeight) / 8) + 1) / 2;
			// This avoids an overflow due to negative value in above calculation
			// Negative value would occur if calibration was done at the non-landed altitude (e.g. half height), 
			// hence any value below that would result in negative values
			percentage = (currentHeight > baseHeight) ? 0 : percentage;
		}

		// Reinitialise the base height
		if (checkButton(LEFT) == PUSHED) {
			baseHeight = currentHeight;
			percentage = 0;
		}
		// Increment the display stage in range of 0 to 2
		if (checkButton(UP) == PUSHED) {
			displayStage = ++displayStage % 3;
		}


		// Update display based on current display stage
		switch (displayStage) {
		case 0:
			displayMeanVal(percentage, "%", g_ulSampCnt); // Display altitude reading as percentage
			break;
		case 1:
			displayMeanVal(currentHeight, " ", g_ulSampCnt); // Display the mean height reading
			break;
		case 2:
			initDisplay(); // Reset and turn off the display
		}


		SysCtlDelay(SysCtlClockGet() / 15); // Update display at 10 Hz
	}
}

