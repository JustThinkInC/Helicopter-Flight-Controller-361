/*
 * PID.h
 *
 *  Created on: 21/05/2018
 *      Author: ljl48
 */

#ifndef PID_H_
#define PID_H_
#include <stdint.h>

int proportionalControl(uint32_t target, uint32_t current);

#endif /* PID_H_ */
