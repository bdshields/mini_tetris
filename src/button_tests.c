/*
 * button_test.c
 *
 *  Created on: 5 Dec. 2022
 *      Author: brandon
 */

#include "active_objects.h"

#include "timer.h"
#include "buttons.h"


typedef struct button_ao_s{
    ao_t super;
    uint32_t delay;
    int led_start;
    int delay_index;
}button_ao_t;

button_ao_t button_ao;

fsm_status_t button_test_active(button_ao_t *me, os_event_t *event);

void button_test_init(void)
{
    ao_cons((ao_t*)&button_ao, button_test_active);
}


fsm_status_t button_test_active(button_ao_t *me, os_event_t *event)
{
    static uint32_t  const  delays[]={TIMER_ms(500), TIMER_ms(250), TIMER_ms(50)};
    fsm_status_t status;
    int temp;
    switch(event->event)
    {
    case E_STATE_ENTER:
        me->delay_index = 0;
        me->led_start = 5;
        buttons_request();

        me->delay = delays[me->delay_index];
        timer_set(me->delay);
        status = EVENT_HANDLED;
        break;
    case E_STATE_EXIT:
        buttons_cancel();
        status = EVENT_HANDLED;
        break;
    case E_TIMER:
        P6OUT ^= 0x01;
        timer_set(me->delay);
        status = EVENT_HANDLED;
        break;
    case E_BUTTON:
        me->delay_index ^= 0x01;
        me->delay = delays[me->delay_index];
        status = EVENT_HANDLED;
        break;
    default:
        status = EVENT_IGNORED;
        break;
    }
    return status;
}



