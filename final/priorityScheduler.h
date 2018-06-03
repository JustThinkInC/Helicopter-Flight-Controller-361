#ifndef PRIORITY_S_
#define PRIORITY_S_
// Upper bound on the bumber of task we allow to be schedualed
#define NUM_TASKS 10
#include <stdbool.h>
bool regesterFinction(int priority, void(function)(void));
void runScheduler(void);


#endif
