#include "priorityScheduler.h" 

/*
 * Authors: Liam Laing, George Khella, Connor Adamson
 * A masic proirity schedualer that will allow the user to set tasks in the form of void f(void) functions running at some some frequency as assigned by the proity:
 * 1 hight to 10 low 
 * this gives the user that ablity to selectivley run tasks more often, note that this not preemptive so the user must suply tasks that will not hog the cpu
 */



// internal regester for a task, the user does not provide these stuctures 
typedef struct task_s {
    void(*function)(void);
    // bound from 1 to 10
    int priority;
} task;

// the max number of tasks a user ca set the scedualer to iterate over 
static task tasks[NUM_TASKS];
// increased toward NUM_TASKS as new tasks are regestered
static int reg_so_far = 0;
static int internal_counter = 0;

/* 
 * The user provides a void f(void) function that will exicute moree often the lower the proirity number is (boundered to 1)
 * the higher the value the fewer times it is run my the schedualer
 * HIGH prioirty tasks, ie related to heli positioning should be run more often
 */
bool regesterFunction(int priority, void(function)(void)){
    if(reg_so_far >= NUM_TASKS){
        return false;
    }
    tasks[reg_so_far] = {function, priority};
    reg_so_far++;
    return true;
}

/*
 * Simply thoughs the cpu into a infinate loop that calls the functions and tracks a counter to determine the amount of time that is spent carrying out that task
 * Note that this dos not stop ISR from exicuting as they can cut in at any* point 
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
