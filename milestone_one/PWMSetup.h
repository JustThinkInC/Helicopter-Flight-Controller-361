/*
 * PWMSetup.h
 *
 *  Created on: 4/05/2018
 *      Author: ljl48
 */

#ifndef PWMSETUP_H_
#define PWMSETUP_H_
// PWM configuration
#define PWM_START_RATE_HZ  250
#define PWM_RATE_STEP_HZ   50
#define PWM_RATE_MIN_HZ    50
#define PWM_RATE_MAX_HZ    400
#define PWM_FIXED_DUTY     67
#define PWM_DIVIDER_CODE   SYSCTL_PWMDIV_4
#define PWM_DIVIDER        4

#define PWM_MAIN_BASE        PWM0_BASE
#define PWM_MAIN_GEN         PWM_GEN_2
#define PWM_MAIN_OUTNUM      PWM_OUT_5
#define PWM_MAIN_OUTBIT      PWM_OUT_5_BIT
#define PWM_MAIN_PERIPH_PWM  SYSCTL_PERIPH_PWM0
#define PWM_MAIN_PERIPH_GPIO SYSCTL_PERIPH_GPIOC
#define PWM_MAIN_GPIO_BASE   GPIO_PORTF_BASE
#define PWM_MAIN_GPIO_CONFIG GPIO_PF1_M1PWM5
#define PWM_MAIN_GPIO_PIN    GPIO_PIN_1

typedef struct PWM_config_s {
    int PWM_Periph_PWM;
    int PWM_Periph_GPIO;
    int PWM_GPIO_Config;
    int PWM_Base;
    int PWM_Pin;
} PWM_config;


void initialisePWM (PWM_config info);
void setPWM (uint32_t u32Freq, uint32_t u32Duty);
void initSinglePWM(PWM_config);




#endif /* PWMSETUP_H_ */
