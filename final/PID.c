#include "PID.h"

/*typedef struct_s {
    static uint32_t currentHeight;
    static uint32_t currentYaw;
    static uint32_t targetHeight;
    static uint32_t targetYaw;
    static int currentError;
} PID;
*/

static uint32_t kp = 5;
static uint32_t ki = 1;
static uint32_t kd = 5;
static uint32_t prevError = 0;
static uint32_t prevT = 0;

uint32_t proportionalControl(uint32_t target, uint32_t current){
    static uint32_t T;
    static uint32_t error_integrated;
    uint32_t error_derivative;
    uint32_t control;


    uint32_t error = target - current;
    error_integrated += error * (T - prevT);  //I
    error_derivative = ((2 * (error - prevError)) + (T-prevT)) / 2 / (T- prevT); //D
    uint32_t dI = ki * error * T;
    control = error * kp + (error_integrated * ki + dI) + error_derivative * kd;

    prevError = error;
    prevT = T;

    if (control > 80) {
        control = 80;
    } else if (control < 5) {
        control = 5;
    } else {
        error_integrated += dI;
    }

    return control;
}
