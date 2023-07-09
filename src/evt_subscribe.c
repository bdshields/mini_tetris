/*
 * evt_subscribe.c
 *
 *  Created on: 14 Jun. 2023
 *      Author: brandon
 */

#include "evt_subscribe.h"

void evt_sub_init(evt_sub_t *subs)
{
    int counter;
    for(counter=0; counter<subs->size; counter++)
    {
        subs->thread[counter] = 0;
    }
}

__attribute__((critical))
void evt_subscribe(evt_sub_t *subs)
{
    int counter;
    for(counter = 0; counter< subs->size; counter++)
    {
        if(subs->thread[counter] == 0)
        {
            subs->thread[counter] = os_get_active_thread();
            return;
        }
    }
}

__attribute__((critical))
void evt_unsubscribe(evt_sub_t *subs)
{
    int counter;
    thread_t cur_thread= os_get_active_thread();
    for(counter = 0; counter< subs->size; counter++)
    {
        if(subs->thread[counter] == cur_thread)
        {
            subs->thread[counter] = 0;
            return;
        }
    }
}

__attribute__((critical))
void evt_send(evt_sub_t *subs, os_msg_t *message)
{
    int counter;
    for(counter = 0; counter< subs->size; counter++)
    {
        if(subs->thread[counter] != 0)
        {
            msg_send(subs->thread[counter], message);
        }
    }
}

