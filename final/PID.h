/*
 * PID.h
 *
 Developed by Liam Laing, George Khella, Connor Adamson
 * Implaments a PID system that takes in the current and desired positions and returns a duty cycle that should be set to get closer to the set point
 */ 

#ifndef PID_H_
#define PID_H_
#include <stdint.h>
#include <uart.h>
// pidControlMain(
//                    Target: this is the goal height that that we want to reach 
//                    Current : this is the position of the heli bounded from 0 
//                    Ticks : This is the counter that is used to maintain the current time 
//               )
// This is a function used to determine what duty cycle to set the main motor to 
uint32_t pidControlMain(uint32_t target, uint32_t current, uint32_t g_ulSampCnt);
// pidControlTail(
//                    Target: this is the goal yaw that that we want to reach 
//                    Current : this is the yaw of the heil 
//                    Ticks : This is the counter that is used to maintain the current time 
//               )
// This is a function used to determine what duty cycle to set the tail motor to 
int pidControlTail(signed int target, signed int current, uint32_t g_ulSampCnt);
#endif /* PID_H_ */
