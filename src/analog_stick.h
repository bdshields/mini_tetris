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

#include "pos.h"

#define STICK_MAX_X  0x07FF
#define STICK_MAX_Y  0x07FF

#define STICK_DEAD_BAND 100


typedef struct stick_event_s{
    os_msg_t super;
    pos_t    pos;
}stick_event_t;



void stick_request(void);
void stick_cancel(void);


#endif /* SRC_ANALOG_STICK_H_ */
