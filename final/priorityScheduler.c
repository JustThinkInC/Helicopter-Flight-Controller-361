#include "priorityScheduler.h" 


// internal regester for a task 
typedef struct task_s {
    void(*function)(void);
    // bound from 1 to 10
    int priority;
} task;

static task tasks[NUM_TASKS];
static int reg_so_far = 0;
static int internal_counter = 0;

bool regesterFinction(int priority, void(function)(void)){
    if(reg_so_far >= NUM_TASKS){
        return false;
    }
    tasks[reg_so_far] = {function, priority};
    reg_so_far++;
    return true;
}

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
