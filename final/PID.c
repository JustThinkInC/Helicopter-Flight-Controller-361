#include "PID.h"

//Variables for main
static float kp = 0.7;
static float ki = 0.009;
static float kd = 0.5;
static signed int prevError = 0;
static float error_integrated = 0;

//Variables for tail
static float kp_tail = 0.7;
//static float ki_tail = 1;
static float kd_tail = 1;
static signed int prevError_tail = 0;
static float error_integrated_tail = 0;

uint32_t pidControlMain(uint32_t target, uint32_t current){
    float error_derivative_main;
    signed int control_main;


    signed int error_main = target - current;
    error_integrated += error_main; //I
    error_derivative_main = (error_main-prevError) / 160;//D
    float dI_main = ki * error_main * 160;
    control_main = (error_main * kp) + (kd*error_derivative_main) + (ki * error_integrated);// + ki_tail * error_integrated_tail;// + (error_integrated_tail+dI_tail);// + (error_integrated_tail * ki_tail + dI_tail) + error_derivative_tail * kd_tail;

    prevError = error_main;

    if (control_main > 90) {
        control_main = 90;
    } else if (control_main < 5) {
        control_main = 5;
    } else  {
        error_integrated += dI_main;
    }

    return control_main;
}


int pidControlTail(signed int target, signed int current) {
   // static uint32_t Ttail;
    float error_derivative_tail;
    signed int control_tail;


    signed int error_tail = target - current;
    error_integrated_tail += error_tail;//error_tail * (Ttail - prevT_tail);  //I
    error_derivative_tail = (error_tail-prevError_tail) / 160;//((2 * (error_tail - prevError_tail)) + (Ttail-prevT_tail)) / 2 / (Ttail- prevT_tail); //D
    //float dI_tail = ki_tail * error_tail * 160;
    control_tail = (error_tail * kp_tail) + (kd_tail *error_derivative_tail);// + ki_tail * error_integrated_tail;// + (error_integrated_tail+dI_tail);// + (error_integrated_tail * ki_tail + dI_tail) + error_derivative_tail * kd_tail;

    prevError_tail = error_tail;

    if (control_tail > 90) {
        control_tail = 90;
    } else if (control_tail < 5) {
        control_tail = 5;
    } else {
       // error_integrated_tail += dI_tail;
    }

    return control_tail;
}
