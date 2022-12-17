/*
 * message.h
 *
 *  Created on: 5 Dec. 2022
 *      Author: brandon
 */

#ifndef SRC_OS_MSG_H_
#define SRC_OS_MSG_H_

#include <stdint.h>

#include "o_allocator.h"
#include "os_event.h"

#define MSG_QUEUE_MAX   3

typedef struct os_msg_s{
    o_base_obj_t super;
    event_t      event;
}os_msg_t;

typedef void* thread_t;
typedef void* msg_queue_p;
typedef os_msg_t *os_msg_p;

int msg_register(os_msg_p *buffer, uint16_t length);
void msg_send(thread_t thread, os_msg_p msg);
os_msg_p msg_get(void);


#endif /* SRC_OS_MSG_H_ */
