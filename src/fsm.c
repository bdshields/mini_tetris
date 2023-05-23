/*
 * fsm.c
 *
 *  Created on: 17 Dec. 2022
 *      Author: brandon
 */

#include "fsm.h"

void fsm_dispatch(fsm_t *me, os_event_t *event)
{
    state_handler prev_state;
    fsm_status_t  status;
    static os_event_t const ev_enter = {.event = E_STATE_ENTER};
    static os_event_t const ev_exit = {.event = E_STATE_EXIT};
    prev_state = me->current_state;
    status = me->current_state(me, event);

    if(status == STATE_CHANGED)
    {
        // Trigger State EXIT
        prev_state(me, &ev_exit);

        // Trigger State ENTRY
        me->current_state(me, &ev_enter);
    }
}

void fsm_cons(fsm_t *me, state_handler initial_state)
{
    me->current_state = initial_state;
}

void fsm_enter_initial_state(fsm_t *me)
{
    static os_event_t const ev_enter = {.event = E_STATE_ENTER};
    me->current_state(me, &ev_enter);
}
