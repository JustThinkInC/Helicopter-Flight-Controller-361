#ifndef PRIORITY_S_
#define PRIORITY_S_

/*
 * Authors: Liam Laing, George Khella, Connor Adamson
 * A masic proirity schedualer that will allow the user to set tasks in the form of void f(void) functions running at some some frequency as assigned by the proity:
 * 1 hight to 10 low 
 * this gives the user that ablity to selectivley run tasks more often, note that this not preemptive so the user must suply tasks that will not hog the cpu
 */



// Upper bound on the bumber of task we allow to be schedualed
#define NUM_TASKS 10
#include <stdbool.h>
// regester a function to run reqularly, 
// 1 means high prioity, while 10 is low prioirty and will be run less often 
bool regesterFunction(int priority, void(function)(void));
// loops endlessly running the tasks at differental frequenzyes given there priority
void runScheduler(void);


#endif
