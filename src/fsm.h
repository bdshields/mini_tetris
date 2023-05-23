/*
 * fsm.h
 *
 *  Created on: 17 Dec. 2022
 *      Author: brandon
 */

#ifndef SRC_FSM_H_
#define SRC_FSM_H_

#include "os_event.h"

typedef enum fsm_status_e{
    EVENT_HANDLED,
    STATE_CHANGED,
    EVENT_IGNORED,
}fsm_status_t;

typedef struct fsm_s fsm_t;
typedef fsm_status_t (*state_handler)(fsm_t *me, os_event_t *event);

struct fsm_s{
    state_handler current_state;
};

void fsm_cons(fsm_t *me, state_handler initial_state);
void fsm_enter_initial_state(fsm_t *me);

void fsm_dispatch(fsm_t *me, os_event_t *event);

// Macro to set new state, AND provide a return status of STATE_CHANGED
#define FSM_TRANS(_new_state) {me->current_state = _new_state, STATE_CHANGED}

#endif /* SRC_FSM_H_ */
