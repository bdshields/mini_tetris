/*
 * os_event.h
 *
 *  Created on: 10 Dec. 2022
 *      Author: brandon
 */

#ifndef SRC_OS_EVENT_H_
#define SRC_OS_EVENT_H_

#include <stdint.h>

typedef enum event_e{
    E_TIMER = 1,
    E_BUTTON,
    E_STICK_UPDATE,
}event_t;


#endif /* SRC_OS_EVENT_H_ */
