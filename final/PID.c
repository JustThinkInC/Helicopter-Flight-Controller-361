#include "PID.h"

//Variables for main
static float kp = 0.6;//1;
static float ki = 0.08;//0.0009;
static float kd = 0.1;//0.8;
static signed int prevError = 0;
static signed int I = 0;
static signed int prevT = 0;

//Variables for tail
static float kpTail = 0.5;
static float kiTail = 0.0005;//0.0005;
static float kdTail = 0.1;
static signed int prevErrorTail = 0;
static signed int prevTtail = 0;
static float ITail = 0;


uint32_t pidControlMain(uint32_t target, uint32_t current, uint32_t ticks) {
    int32_t control;

    float T = (float)(ticks - prevT) / 160;
    prevT = ticks;

    signed int error = target - current;
    int32_t P = kp * error;
    int32_t D = (kd / T) * (error - prevError);
    int32_t dI = ki * error * T;

    control = P + (I + dI) + D + 40;//+ (error_integrated * ki + dI) + error_derivative * kd;

    prevError = error;

    if (control > 90) {
        control = 90;
    } else if (control < 5) {
        control = 5;
    } else {
        I += dI;
    }

    return control;
}


int pidControlTail(signed int target, signed int current, uint32_t ticks) {
   // static uint32_t Ttail;
    /*float D;
    signed int control_tail;
    //offset 23 maybe or 27

    float T = (float) (g_ulSampCnt - prevTtail) / 160;
    prevTtail = g_ulSampCnt;

    signed int error_tail = target - current;
    //I += error_tail;//error_tail * (Ttail - prevT_tail);  //I
    D = (kd_tail / T) * (error_tail - prevError_tail);// (error_tail-prevError_tail) / 160;//((2 * (error_tail - prevError_tail)) + (Ttail-prevT_tail)) / 2 / (Ttail- prevT_tail); //D
    //float dI_tail = ki_tail * error_tail * 160;
    int32_t dI = ki_tail * error_tail * T;

    control_tail = (error_tail * kp_tail) + (ITail+dI) + D;// + ki_tail * error_integrated_tail;// + (error_integrated_tail+dI_tail);// + (error_integrated_tail * ki_tail + dI_tail) + error_derivative_tail * kd_tail;

    prevError_tail = error_tail;

    if (control_tail > 90) {
        control_tail = 90;
    } else if (control_tail < 5) {
        control_tail = 5;
    } else {
       ITail+=dI;// error_integrated_tail += dI_tail;
    }

    return control_tail;*/


    int32_t control;

    float T = (float)(ticks - prevTtail) / 80;//160;
    prevTtail = ticks;

    signed int error = target - current;
    int32_t P = kpTail * error;
    int32_t D = (kdTail / T) * (error - prevErrorTail);
    int32_t dI = kiTail * error * T;

    control = P + (ITail + dI) + D;

    prevErrorTail = error;

    if (control > 90) {
        control = 90;
    } else if (control < 5) {
        control = 5;
    } else {
        ITail += dI;
    }

    return control;
}
