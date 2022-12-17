/*
 * timer.c
 *
 *  Created on: 5 Dec. 2022
 *      Author: brandon
 */

#include <msp430.h>
#include <stdint.h>
#include "os_msg.h"
#include "os_scheduler.h"
#include "timer.h"


#define TIMER_MAX   10

typedef struct timer_evt_s{
    thread_t thread;
    uint32_t counter;
}timer_evt_t;

timer_evt_t timer_list[TIMER_MAX];
int         timers_active;              // Number of active timers

uint32_t    timerVal;

__attribute__((interrupt(TIMERA0_VECTOR)))
void isr_tmrA(void)
{
    static os_msg_t message = {.event=E_TIMER};
    int counter;
    int active_remaining = timers_active;
    timerVal += TIMER_INTERVAL;
    for(counter = 0; (counter< TIMER_MAX) && (active_remaining > 0); counter++)
    {
        if(timer_list[counter].counter)
        {
            active_remaining --;
            if(--timer_list[counter].counter == 0)
            {
                timers_active--;
                msg_send(timer_list[counter].thread, &message);
            }
        }
    }
}

void timerInit(void)
{
    int counter;
    TACCR0 = 0x7FFF / (1000 / TIMER_INTERVAL);
    TACTL = TASSEL_1 | ID_0;
    TACTL |= MC_1;

    timers_active = 0;
    // set all timers to 0
    for(counter = 0; counter< TIMER_MAX; counter++)
    {
        timer_list[counter].counter = 0;
    }

    TACCTL0 = CCIE;
}

__attribute__((critical))
void timer_set(uint32_t value)
{
    int counter;
    if(value == 0)
    {
        // assume we want atleast 1 tick
        value = 1;
    }
    for(counter = 0; counter< TIMER_MAX; counter++)
    {
        if(timer_list[counter].counter == 0)
        {
            timer_list[counter].counter = value;
            timer_list[counter].thread = os_get_active_thread();
            timers_active++;
            return;
        }
    }
}

__attribute__((critical))
uint32_t timer_get_value(void)
{
    return timerVal;
}
