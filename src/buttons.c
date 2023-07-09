/*
 * buttons.c
 *
 *  Created on: 11 Dec. 2022
 *      Author: brandon
 */


#include "buttons.h"
#include "os_msg.h"
#include "evt_subscribe.h"
#include "os_scheduler.h"

#include "timer.h"

DECLARE_EVT_SUB(button_requests, 2);

os_msg_p   button_messages[10];

void buttons_send_event();

void button_task(void* context)
{
    int assert_counter;
    os_msg_p msg;
    msg_register(button_messages, 10);
    timer_set(TIMER_ms(10));
    while(1)
    {
        msg = msg_get();
        switch(msg->event)
        {
        case E_TIMER:
            if(P6IN & 0x02)
            {
                assert_counter = 0;
            }
            else
            {
                assert_counter++;
            }
            if(assert_counter == 3)
            {
                buttons_send_event();
            }
            timer_set(TIMER_ms(10));
            break;
        }
    }
}

uint16_t button_stack[100];
void buttons_init(void)
{
    evt_sub_init(&button_requests);
    sched_thread_start(button_task, NULL, button_stack, 100);
}

void buttons_send_event()
{
    static os_msg_t message = {.event=E_BUTTON};
    evt_send(&button_requests, &message);
}

void buttons_request(void)
{
    evt_subscribe(&button_requests);
}

void buttons_cancel(void)
{
    evt_unsubscribe(&button_requests);
}
