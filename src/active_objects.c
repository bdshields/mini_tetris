/*
 * active_objects.c
 *
 *  Created on: 17 Dec. 2022
 *      Author: brandon
 */

#include "active_objects.h"
#include "os_msg.h"

void ao_task(void *me)
{
    os_msg_p msg;
    ao_t *ao = (ao_t *)me;
    msg_register(ao->msg_queue, AO_MSG_SIZE);

    fsm_enter_initial_state(me);
    while(1)
    {
        msg = msg_get();
        fsm_dispatch((fsm_t *)me, (os_event_t*)msg);
        o_free(msg);
    }
}

void ao_cons(ao_t *me, state_handler initial_state)
{
    fsm_cons(me, initial_state);
    me->task = sched_thread_start(ao_task, me, me->stack, AO_STACK_SIZE);

}
