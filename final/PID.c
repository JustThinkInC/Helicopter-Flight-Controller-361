#include "PID.h"

/*typedef struct_s {
    static uint32_t currentHeight;
    static uint32_t currentYaw;
    static uint32_t targetHeight;
    static uint32_t targetYaw;
    static int currentError;
} PID;
*/
int proportionalControl(uint32_t target, uint32_t current){
    int error = target - current;
   // PID->currentError = error;
    return error;
}
