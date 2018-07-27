#ifndef PRIORITY_S_
#define PRIORITY_S_

/*
 * Authors: Liam Laing, George Khella, Connor Adamson
 * A basic proirity scheduler that will allow the user to set tasks 
 * in the form of void f(void) functions
 * running at frequency as assigned by the proity: 1 (high) to 10 (low)
 * This gives the user that ablity to selectivley run tasks more often.
 * Note that this is not preemptive so the user must supply tasks that will not hog the cpu
 */


// Upper bound on the number of tasks we allow to be scheduled
#define NUM_TASKS 10
#include <stdbool.h>

// Register a function to run regularly, 
// 1 means high priority, while 10 is low priority
bool registerFunction(int priority, void(function)(void));

// Loops endlessly running the tasks at different frequencies given their priority
void runScheduler(void);


#endif
