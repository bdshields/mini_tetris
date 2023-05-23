/*
 * os_event.h
 *
 *  Created on: 10 Dec. 2022
 *      Author: brandon
 */

#ifndef SRC_OS_EVENT_H_
#define SRC_OS_EVENT_H_

#include <stdint.h>
#include "o_allocator.h"

typedef enum event_e{
    E_STATE_INITIAL = 1,
    E_STATE_ENTER,
    E_STATE_EXIT,
    E_TIMER,
    E_BUTTON,
    E_STICK_UPDATE,
}event_t;

typedef struct os_event_s{
    o_base_obj_t super;  // A super object to help with dynamically allocated event objects
    event_t      event;
}os_event_t;




#endif /* SRC_OS_EVENT_H_ */
