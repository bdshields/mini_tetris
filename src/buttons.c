/*
 * buttons.c
 *
 *  Created on: 11 Dec. 2022
 *      Author: brandon
 */


#include "buttons.h"
#include "os_msg.h"

#include "os_scheduler.h"

#include "timer.h"

typedef struct button_evt_s{
    thread_t thread;
}button_evt_t;

#define BUTTON_MAX 2

button_evt_t button_list[BUTTON_MAX];
int         buttons_active;              // Number of active timers

//extern thread_t blink_thread;

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
    sched_thread_start(button_task, NULL, button_stack, 100);
}

void buttons_send_event()
{
    static os_msg_t message = {.event=E_BUTTON};
    int counter;
    for(counter = 0; counter< BUTTON_MAX; counter++)
    {
        if(button_list[counter].thread != 0)
        {
            msg_send(button_list[counter].thread, &message);
        }
    }

}

__attribute__((critical))
void buttons_request(void)
{
    int counter;
    for(counter = 0; counter< BUTTON_MAX; counter++)
    {
        if(button_list[counter].thread == 0)
        {
            button_list[counter].thread = os_get_active_thread();
            return;
        }
    }
}

__attribute__((critical))
void buttons_cancel(void)
{
    int counter;
    thread_t cur_thread= os_get_active_thread();
    for(counter = 0; counter< BUTTON_MAX; counter++)
    {
        if(button_list[counter].thread == cur_thread)
        {
            button_list[counter].thread = 0;
            return;
        }
    }
}
