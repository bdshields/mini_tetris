/*
 * analog_stick.h
 *
 *  Created on: 13 Dec. 2022
 *      Author: brandon
 */

#ifndef SRC_ANALOG_STICK_H_
#define SRC_ANALOG_STICK_H_

void stick_init(void);

#include "os_msg.h"

typedef struct stick_event_s{
    os_msg_t super;
    int16_t  x;
    int16_t  y;
}stick_event_t;

#endif /* SRC_ANALOG_STICK_H_ */
