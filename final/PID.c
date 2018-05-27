#include "PID.h"

//Variables for main
static uint32_t kp = 4;
static uint32_t ki = 2;
static uint32_t kd = 1;
static uint32_t prevError = 0;
static uint32_t prevT = 0;
static uint32_t mainT = 0;
static uint32_t error_integrated = 0;

//Variables for tail
static float kp_tail = 0.7;
static float ki_tail = 1;
static float kd_tail = 1;
static signed int prevError_tail = 0;
static uint32_t prevT_tail = 0;
static uint32_t Ttail = 0;
static float error_integrated_tail = 0;

uint32_t proportionalControl(uint32_t target, uint32_t current){

    uint32_t error_derivative;
    uint32_t control;


    uint32_t error = target - current;
    error_integrated += error * (mainT - prevT);  //I
    error_derivative = ((2 * (error - prevError)) + (mainT-prevT)) / 2 / (mainT- prevT); //D
    uint32_t dI = ki * error * mainT;
    control = error * kp + (error_integrated * ki + dI) + error_derivative * kd;

    prevError = error;
    prevT = mainT;

    if (control > 90) {
        control = 90;
    } else if (control < 5) {
        control = 5;
    } else {
        error_integrated += dI;
    }

    return control;
}


int pidControlTail(signed int target, signed int current) {
    static uint32_t Ttail;
    float error_derivative_tail;
    signed int control_tail;


    signed int error_tail = target - current;
    error_integrated_tail += error_tail;//error_tail * (Ttail - prevT_tail);  //I
    error_derivative_tail = (error_tail-prevError_tail) / 160;//((2 * (error_tail - prevError_tail)) + (Ttail-prevT_tail)) / 2 / (Ttail- prevT_tail); //D
    float dI_tail = ki_tail * error_tail * 160;
    control_tail = (error_tail * kp_tail) + (kd_tail *error_derivative_tail);// + ki_tail * error_integrated_tail;// + (error_integrated_tail+dI_tail);// + (error_integrated_tail * ki_tail + dI_tail) + error_derivative_tail * kd_tail;

    prevError_tail = error_tail;
    prevT_tail = Ttail;

    if (control_tail > 90) {
        control_tail = 90;
    } else if (control_tail < 5) {
        control_tail = 5;
    } else {
       // error_integrated_tail += dI_tail;
    }

    return control_tail;
}
