/*
 * evt_subscribe.h
 *
 *  Created on: 14 Jun. 2023
 *      Author: brandon
 */

#ifndef SRC_EVT_SUBSCRIBE_H_
#define SRC_EVT_SUBSCRIBE_H_

#include "os_msg.h"


typedef struct evt_sub_s{
    unsigned int size;
    thread_t thread[];
}evt_sub_t;

#define DECLARE_EVT_SUB(_name, _size) static evt_sub_t _name={.size = _size, .thread[_size]={0}}

void evt_sub_init(evt_sub_t *subs);
void evt_subscribe(evt_sub_t *subs);
void evt_unsubscribe(evt_sub_t *subs);
void evt_send(evt_sub_t *subs, os_msg_t *message);


#endif /* SRC_EVT_SUBSCRIBE_H_ */
