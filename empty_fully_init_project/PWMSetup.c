# include "PWMSetup.h"


void initialisePWM(PWM_config info){
    SysCtlPeripheralEnable(info.PWM_Type_Periph_PWM);
    SysCtlPeripheralEnable(info.PWM_Type_Periph_GPIO);

    GPIOPinConfigure(info.PWM_Type_GPIO_Config);

    GPIOPinTypePWM(info.PWM_Type_Base, info.PWM_Type_Pin);
}

void initSinglePWM(PWM_config info){
    GPIOPinTypePWM(info.PWM_Type_Base, info.PWM_Type_Pin);
    PWMGenConfigure(info.PWM_Type_Base, info.PWM_Type_Pin,
                   PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenEnable(info.PWM_Type_Base, info.PWM_Type_Pin);

}

/********************************************************
 * Function to set the freq, duty cycle of M0PWM7
 ********************************************************/
void
setPWM (uint32_t ui32Freq, uint32_t ui32Duty)
{
    // Calculate the PWM period corresponding to the freq.
    uint32_t ui32Period = SysCtlClockGet() / PWM_DIVIDER / ui32Freq;

    PWMGenPeriodSet(PWM_MAIN_BASE, PWM_MAIN_GEN, ui32Period);
    PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUTNUM,
        ui32Period * ui32Duty / 100);
}
