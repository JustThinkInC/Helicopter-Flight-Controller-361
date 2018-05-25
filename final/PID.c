#include "PID.h"

/*typedef struct_s {
    static uint32_t currentHeight;
    static uint32_t currentYaw;
    static uint32_t targetHeight;
    static uint32_t targetYaw;
    static int currentError;
} PID;
*/

static uint32_t proportion = 8;
static uint32_t ki = 7;
static uint32_t kd = 2;
static uint32_t prevError = 0;
static uint32_t prevT = 0;

uint32_t proportionalControl(uint32_t target, uint32_t current){
    static uint32_t I;
    static uint32_t T;
    uint32_t error = target - current;
    uint32_t P = proportion * error;
    uint32_t dI = error * (T-prevT);
   // I += error * (T-prevT);
    uint32_t D = (error - prevError) / (T - prevT);
    uint32_t control = P+(I+dI)+D;
    prevError = error;
    prevT = T;

    if  (control > 95) {
        control = 95;
    } else if  (control < 5) {
        control = 5;}
    else {
        I += dI;
    }


    //uint32_t D = ((2*kd+T)/T/2) * (error-prevError);


    return control;
}
