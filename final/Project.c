// Functional flight controller for helicopter rig.
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
#include "buttons6.h"

// Modules developed by group
#include "PWMSetup.h"
#include "PID.h"
#include "uart.h"
#include "priorityScheduler.h"

//*****************************************************************************
// Constants
//*****************************************************************************
#define BUF_SIZE 20
#define SAMPLE_RATE_HZ 160
#define SWITCH_ONE GPIO_PIN_4

//*****************************************************************************
// Global variables
//*****************************************************************************
static circBuf_t g_inBuffer;    // Buffer of size BUF_SIZE integers (sample values)
static uint32_t g_ulSampCnt;    // Counter for the interrupts
static signed int yaw = 0;      // Yaw value of helicopter
static signed int prevYaw = 0;  // Previuos Yaw
static uint8_t PrevChAB = 0x00; //Previous state of channels A & B
static uint16_t heightPercentage = 0; //Percentage of height

static uint16_t currentHeight = 0;
static uint16_t baseHeight = 0;
bool landed = true;
bool stabilizer;

static uint32_t mainFreq = PWM_START_RATE_HZ;
static uint32_t tailFreq = PWM_START_RATE_HZ;
static uint32_t mainDutyCycle = PWM_FIXED_DUTY;
static uint32_t tailDutyCycle = PWM_FIXED_DUTY;

uint32_t targetHeight = 0;
signed int targetTail = 0;
signed int degs = 0;

static uint32_t slowTick = 0;
#define MAX_TICK 100
bool isSlowTick = false;

static bool foundRef = false;


/*
 * This is an ISR that fires when the state of the yaw referance changes.
 * We only want to reset the degrees of the heli when we first encounter the initial position of the camera.
 * We don't need to find this position again beyond the first time. 
 */
void refFound(void) {
    IntMasterDisable();
    if(GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4) == false && !foundRef){
        degs = 0;
        yaw = 0;
        targetTail *= -1;
        foundRef = true;
    }
    GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_4);
    
    IntMasterEnable();
}

//*****************************************************************************
//
// The interrupt handler for the for SysTick interrupt.
// handels some counters mostly 
//*****************************************************************************
void
SysTickIntHandler(void)
{
    // Initiate a conversion
    ADCProcessorTrigger(ADC0_BASE, 3);
    g_ulSampCnt++;
    slowTick ++;
    if (slowTick > MAX_TICK){
        slowTick = 0;
        isSlowTick = true;
    }

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
//
// The handler for the yaw monitor.
//
//*****************************************************************************
void
yawIntHandler(void) {

    uint16_t chAB = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    if (chAB != PrevChAB) {
        //00
        if (PrevChAB == 0x00) {
            if (chAB == 0x01) {
                yaw++;
            } else if (chAB == 0x02) {
                yaw--;
            }
        //01
        } else if (PrevChAB == 0x01) {
            if (chAB == 0x03) {
                yaw++;
            } else if (chAB == 0x00) {
                yaw--;
            }
        //11
        } else if (PrevChAB == 0x03) {
            if (chAB == 0x02) {
                yaw++;
            } else if (chAB == 0x01) {
                yaw--;
            }
        //10
        } else if (PrevChAB == 0x02) {
            if (chAB == 0x00) {
                yaw++;
            } else if (chAB == 0x03) {
                yaw--;
            }
        }
        PrevChAB = chAB;
    }
    GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
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

    // added to config the adc with a prescaler
    SysCtlPWMClockSet(PWM_DIVIDER_CODE);
}

/*
 * initADC: gets the ADC redy to convert the signal of the height sensor  
 */
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

/*
 * intRef: prepares the refernace and sets up a GPIO interupt for the heli hitting the referance position
 */
void
intRef(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
    GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_DIR_MODE_IN);
    GPIOIntRegister(GPIO_PORTC_BASE, refFound);
    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_4 , GPIO_RISING_EDGE);
    GPIOIntEnable(GPIO_PORTC_BASE, GPIO_PIN_4);
}

void
initYaw(void)
{

    // The GPIO PB0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);

    //Configure GPIO
    GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_DIR_MODE_IN);
    GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_DIR_MODE_IN);

    PrevChAB = GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Register the interrupt handler
    GPIOIntRegister(GPIO_PORTB_BASE, yawIntHandler);
    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_BOTH_EDGES);

    // Enable interrupts for GPIO Port B sequence 3 (clears any outstanding interrupts)
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
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
displayVal(uint16_t meanVal, signed int degs, uint32_t mainDuty, uint32_t tailDuty)
{
    char heightString[17];  // 16 characters across the display
    OLEDStringDraw("                ", 0, 0); // Clear the display
    usnprintf(heightString, sizeof(heightString), "Alt = %d %s", meanVal, "%");

    char yawString[17];
    OLEDStringDraw("                ", 0, 1);
    usnprintf(yawString, sizeof(yawString), "Yaw = %d %s", degs, "deg");

    char mainMotorString[17];
    OLEDStringDraw("                ", 0, 2);
    usnprintf(mainMotorString, sizeof(mainMotorString), "Main = %d %s", mainDuty, "%");

    char tailMotorString[17];
    OLEDStringDraw("                ", 0, 3);
    usnprintf(tailMotorString, sizeof(tailMotorString), "Tail = %d %s", tailDuty, "%");
    // Update line on display.
    OLEDStringDraw(heightString, 0, 0);
    OLEDStringDraw(yawString, 0, 1);
    OLEDStringDraw(mainMotorString, 0, 2);
    OLEDStringDraw(tailMotorString, 0, 3);
}

/*
 * buttonPress: used to alter the state of several set points for the heli depending on button presses
 */
void
buttonPress() {
    if (checkButton(UP) == PUSHED && heightPercentage < 100) {
       if (heightPercentage + 10 < 100) {
            targetHeight += 10;
            mainDutyCycle = pidControlMain(targetHeight, heightPercentage, g_ulSampCnt);
            setPWM(mainFreq, mainDutyCycle);
        } else {
            targetHeight += 100-heightPercentage;
            mainDutyCycle = pidControlMain(targetHeight, heightPercentage, g_ulSampCnt);
            setPWM(mainFreq, mainDutyCycle);
        }
    } else if (checkButton(DOWN) == PUSHED && heightPercentage >= 0) {
        //decrease height by 10%
        if (heightPercentage - 10 > 0) {
           targetHeight -= 10;
           mainDutyCycle = pidControlMain(targetHeight, heightPercentage ,g_ulSampCnt);
           setPWM(mainFreq, mainDutyCycle);
        } else {
           targetHeight -= heightPercentage;
           mainDutyCycle = pidControlMain(targetHeight, heightPercentage, g_ulSampCnt);
           setPWM(mainFreq, mainDutyCycle);
        }

    } else if (checkButton(LEFT) == PUSHED) {
        //rotate 15degs ccw
        degs = -1 * (yaw * 4 + (5/2)) / 5;
        targetTail -= 15;
        tailDutyCycle = pidControlTail(targetTail, degs, g_ulSampCnt);
        setPWM_Tail(tailFreq, tailDutyCycle);
     } else if (checkButton(RIGHT) == PUSHED) {
        degs = -1 * (yaw * 4 + (5/2)) / 5;
        targetTail +=15;
        tailDutyCycle = pidControlTail(targetTail, degs, g_ulSampCnt);
        setPWM_Tail(tailFreq, tailDutyCycle);
     } else if (checkButton(SLID) == PUSHED || checkButton(SLID) == RELEASED) {
         stateMachine();
     } else if (checkButton(SLID2) == PUSHED || checkButton(SLID2) == RELEASED) {
         UARTSend("Soft Reset \r");
         SysCtlReset();
     }
}

// Background task: calculate the (approximate) mean of the values in the
// circular buffer and display it, together with the sample number.
void
ADCSampling() {
    int32_t sum;
    uint8_t i = 0;
    uint16_t currentHeight = 0;

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
        heightPercentage = ((baseHeight - currentHeight) * 100) / (1000);
        heightPercentage = (currentHeight > baseHeight) ? 0 : heightPercentage;
        heightPercentage = (heightPercentage > 100) ? 100 : heightPercentage;
    }
}

/*
 * StateMachine: manages a statemachine depending on the static variables
 * for the button states and if the references are found.
 * Used to take off and land the heli
 */
void stateMachine() {
    switch (landed) {
    case true:
        UARTSend("select up press \r");
        //Take off
        setPWM(mainFreq, 15);
        landed = false;
        stabilizer = true;

        while(!foundRef){
            targetTail += 1;
            tailDutyCycle = pidControlTail(targetTail, degs, g_ulSampCnt);
            setPWM_Tail(tailFreq, tailDutyCycle);
            SysCtlDelay(SysCtlClockGet()/100);
        }
        break;
    case false:
        //land
        UARTSend("Mode: Landing \r");
        while (heightPercentage > 0) {
            targetHeight = 0;
            targetTail *= -1;
            setPWM(mainFreq, 0);
            setPWM_Tail(tailFreq, 0);
            #ifdef DEBUG_ONLY
                UARTSend("select down press \r");
            #endif

        }stabilizer = false;
        landed = true;
        setPWM(mainFreq, 0);
    }
}

/*
 * displayUART: formats the output for dispatch of unified information to the serial output to the sharelab interface 
 */
void displayUART(void) {
    char string[16];
    usnprintf(string, sizeof(string), "Alt[%d] %d \r", targetHeight, heightPercentage);
    UARTSend(string);
    usnprintf(string, sizeof(string), "Yaw[%d] %d \r", targetTail, degs);
    UARTSend(string);
    usnprintf(string, sizeof(string), "Main Duty: %d \r", mainDutyCycle);
    UARTSend(string);
    usnprintf(string, sizeof(string), "Tail Duty: %d \r", tailDutyCycle);
    if (landed) {
        usnprintf(string, sizeof(string), "Mode: landed \r");
    } else {
        usnprintf(string, sizeof(string), "Mode: Flying \r");
    }
}

/*
 * stabalize: used to get pid control updates and set them 
 */
void stabalize(void){
    if (stabilizer) {
            mainDutyCycle = pidControlMain(targetHeight, heightPercentage, g_ulSampCnt);
            setPWM(mainFreq, mainDutyCycle);

            degs = -1 * (yaw * 4 + (5/2)) / 5;

            tailDutyCycle = pidControlTail(targetTail, degs, g_ulSampCnt);
            setPWM_Tail(tailFreq, tailDutyCycle);
        }
}
/*
 * Initialising the various peripherals, 
 * registering the core functions for use by the scheduler,
 * then entering the main loop
 */
int
main(void)
{

    SysCtlPeripheralReset (PWM_MAIN_PERIPH_GPIO); // Used for PWM output
    SysCtlPeripheralReset (PWM_MAIN_PERIPH_PWM);  // Main Rotor PWM

    SysCtlPeripheralReset (PWM_TAIL_PERIPH_GPIO); // Used for PWM output
    SysCtlPeripheralReset (PWM_TAIL_PERIPH_PWM);  // Main Rotor PWM

    SysCtlPeripheralReset (UP_BUT_PERIPH);        // UP button GPIO
    SysCtlPeripheralReset (DOWN_BUT_PERIPH);      // DOWN button GPIO


    //Initialisation
    initClock();
    initButtons();
    initADC();
    intRef();
    initYaw();
    initDisplay();
    initCircBuf(&g_inBuffer, BUF_SIZE);
    initialiseUSB_UART();

    // added for pwm
    initialisePWM();
    initialisePWM_Tail();

    GPIOPadConfigSet(GPIO_PORTF_BASE, SWITCH_ONE, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);

    // Initialisation is complete, so turn on the output.
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, true);
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, true);

    // Enable interrupts to the processor.
    IntMasterEnable();
    
    // set up the priority scheduler 
    
    registerFunction(1, ADCSampling);
    registerFunction(1, stabalize);
    registerFunction(5, buttonPress);
    registerFunction(10, displayUART);
    
    // run the registered tasks forever
    runScheduler();
}

