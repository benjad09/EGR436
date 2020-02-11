/*
 * Timer.h
 *
 *  Created on: Jan 7, 2020
 *      Author: bendr
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "msp.h"

void Init_SYSTICK(void);

uint64_t GetUpTime(void);

#endif /* TIMER_H_ */
