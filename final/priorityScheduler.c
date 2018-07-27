#include "priorityScheduler.h" 

/*
 * Authors: Liam Laing, George Khella, Connor Adamson
 * A basic proirity scheduler that will allow the user to set tasks
 * in the form of void f(void) functions
 * running at frequency as assigned by the proity: 1 (high) to 10 (low)
 * This gives the user that ablity to selectivley run tasks more often.
 * Note that this is not preemptive so the user must supply tasks that will not hog the cpu
 */


// Internal register for a task, the user does not provide these structures 
typedef struct task_s {
    void(*function)(void);
    // bound from 1 to 10
    int priority;
} task;

// Max number of tasks a user ca set the scheduler to iterate over 
static task tasks[NUM_TASKS];

// Increased toward NUM_TASKS as new tasks are registered
static int reg_so_far = 0;
static int internal_counter = 0;

/* 
 * The user provides a void f(void) function that will execute more often 
 * the lower the proirity number is (boundered to 1)
 * the higher the value the fewer times it is run by the schedualer
 * HIGH prioirty tasks, ie related to heli positioning should be run more often
 */
bool registerFunction(int priority, void(function)(void)){
    if(reg_so_far >= NUM_TASKS){
        return false;
    }
    tasks[reg_so_far] = {function, priority};
    reg_so_far++;
    return true;
}

/*
 * Simply throws the cpu into a infinite loop
 * Calls the functions and tracks a counter to determine the 
 * amount of time that is spent carrying out that task
 * Note that this dos not stop ISR from executing as they can cut in at any point 
 */
void runScheduler(void){
    int i = 0;
    while(true){    
        for(i=0; i < NUM_TASKS; i++){
            if(tasks[i].priority <= internal_counter){
                tasks[i].function();
            }
            i = ++i % NUM_TASKS;
        }
    }
    
}
