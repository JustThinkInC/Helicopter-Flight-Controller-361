/*
 * PWMSetup.h
 *
 *  Created on: 4/05/2018
 *      Authors: Liam Laing, George Khella, Connor Adamson
 *  Used to configure the PWM generators using setPWM* and set using the setPWM* to alter the form of the PWM generated
 *  We use this set of functions to drive the two heli motors.
 */



#ifndef PWMSETUP_H_
#define PWMSETUP_H_


#include <stdint.h>

// PWM configuration
#define PWM_START_RATE_HZ  250
#define PWM_RATE_STEP_HZ   50
#define PWM_RATE_MIN_HZ    50
#define PWM_RATE_MAX_HZ    400
#define PWM_FIXED_DUTY     5
#define PWM_DIVIDER_CODE   SYSCTL_PWMDIV_4
#define PWM_DIVIDER        4

//  PWM Hardware Details M0PWM7 (gen 3)
//  ---Main Rotor PWM: PC5, J4-05
#define PWM_MAIN_BASE        PWM0_BASE
#define PWM_MAIN_GEN         PWM_GEN_3
#define PWM_MAIN_OUTNUM      PWM_OUT_7
#define PWM_MAIN_OUTBIT      PWM_OUT_7_BIT
#define PWM_MAIN_PERIPH_PWM  SYSCTL_PERIPH_PWM0
#define PWM_MAIN_PERIPH_GPIO SYSCTL_PERIPH_GPIOC
#define PWM_MAIN_GPIO_BASE   GPIO_PORTC_BASE
#define PWM_MAIN_GPIO_CONFIG GPIO_PC5_M0PWM7
#define PWM_MAIN_GPIO_PIN    GPIO_PIN_5


//  PWM Hardware Details M1PWM5 (gen 3)
//  ---Main Rotor PWM: PC5, J4-05
#define PWM_TAIL_BASE        PWM1_BASE
#define PWM_TAIL_GEN         PWM_GEN_2
#define PWM_TAIL_OUTNUM      PWM_OUT_5
#define PWM_TAIL_OUTBIT      PWM_OUT_5_BIT
#define PWM_TAIL_PERIPH_PWM  SYSCTL_PERIPH_PWM1
#define PWM_TAIL_PERIPH_GPIO SYSCTL_PERIPH_GPIOF
#define PWM_TAIL_GPIO_BASE   GPIO_PORTF_BASE
#define PWM_TAIL_GPIO_CONFIG GPIO_PF1_M1PWM5
#define PWM_TAIL_GPIO_PIN    GPIO_PIN_1

/* 
 * initialisePWM is the primary configuration function that sets up the peripheral pwm generator.
 * It sets its output on GPIO J3-10.
 * Must be called before user can set desired frequency and duty cycle to drive the motor
 */
void initialisePWM (void);

/* setPWM is used to set the form of the pwm that is seen on J3-10 on the Tiva.
 * The user can provide a frequency (recommended at least 200Hz for heli flight) and a duty cycle. 
 * The latancy in motor response leads to partial powering instead of binary operation 
 */
void setPWM (uint32_t u32Freq, uint32_t u32Duty);



/* initialisePWM_Tail is used to configure a seperate PWM generator to drive the tail motor.
 * This function must be called before the user can configure the use of the tail motor
 */
void initialisePWM_Tail(void);

/*
 * setPWM_Tail is used to alter the form of the PWM wave that is used to drive the tail rotor
 * It is recommended that the frequency of the pwm be at least 200hz,
 * as the motor can not turn on and off completly 200 times a second,
 * and this will allow for non binary modes of the tail motor
 */
void setPWM_Tail (uint32_t ui32Freq, uint32_t ui32Duty);

#endif /* PWMSETUP_H_ */
