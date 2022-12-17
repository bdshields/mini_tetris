/*
 * timer.h
 *
 *  Created on: 5 Dec. 2022
 *      Author: brandon
 */

#ifndef SRC_TIMER_H_
#define SRC_TIMER_H_

#include <stdint.h>

/**
 * 1ms into u32 == 49 days of counter range
 * 10ms           497 days
 */

#define TIMER_INTERVAL  10   // interval in ms

#define TIMER_s(_sec) ((_sec) * 1000 / TIMER_INTERVAL)
#define TIMER_ms(_millisec) ((_millisec)/TIMER_INTERVAL)

void timerInit(void);
void timer_set(uint32_t value);

uint32_t timer_get_value(void);

#endif /* SRC_TIMER_H_ */
