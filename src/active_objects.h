/*
 * active_objects.h
 *
 *  Created on: 17 Dec. 2022
 *      Author: brandon
 */

#ifndef SRC_ACTIVE_OBJECTS_H_
#define SRC_ACTIVE_OBJECTS_H_

#include "fsm.h"
#include "os_scheduler.h"
#include "os_msg.h"

#define AO_MSG_SIZE 10
#define AO_STACK_SIZE 100

typedef struct ao_s{
    fsm_t    super;
    thread_t task;
    os_msg_p  msg_queue[AO_MSG_SIZE];
    uint16_t stack[AO_STACK_SIZE];
}ao_t;


void ao_cons(ao_t *me, state_handler initial_state);


#endif /* SRC_ACTIVE_OBJECTS_H_ */
