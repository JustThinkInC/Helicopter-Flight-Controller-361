/*
 * PID.h
 *
 *  Created on: 21/05/2018
 *      Author: ljl48
 */

#ifndef PID_H_
#define PID_H_
#include <stdint.h>
#include <uart.h>
//int pidControlMain(uint32_t target, uint32_t current);
uint32_t pidControlMain(uint32_t target, uint32_t current, uint32_t g_ulSampCnt);
int pidControlTail(signed int target, signed int current, uint32_t g_ulSampCnt);
#endif /* PID_H_ */
