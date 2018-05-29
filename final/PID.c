#include "PID.h"

//Variables for main
static float kp = 0.8;
static float ki = 0.08;
static float kd = 0.1;
static signed int prevError = 0;
static signed int I = 0;
static signed int prevT = 0;

//Variables for tail
static float kpTail = 0.15;
static float kiTail = 0.0005;
static float kdTail = 0.05;
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

    control = P + (I + dI) + D + 35;//+ (error_integrated * ki + dI) + error_derivative * kd;

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
    int32_t control;

    float T = (float)(ticks - prevTtail) / 160;//160;
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
