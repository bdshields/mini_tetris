#include <msp430.h>
#include <stdint.h>

#include "os_scheduler.h"
#include "os_msg.h"
#include "os_event.h"

#include "timer.h"
#include "buttons.h"
#include "analog_stick.h"
#include "o_allocator.h"

void __attribute__ ((noinline)) delay(uint32_t delay)
{
    volatile uint32_t counter = delay;
    while(counter--)
    {
        
    }
}



os_msg_p   blink_messages[10];
uint16_t blink_stack[100];

void blink(void* context)
{
    os_msg_p msg;
    uint32_t    delays[]={TIMER_ms(500), TIMER_ms(250), TIMER_ms(50)};
    int delay_index=0;
    uint32_t delay;
    msg_register(blink_messages, 10);
    timer_set(TIMER_ms(500));
    while(1)
    {
        msg = msg_get();
        switch(msg->event)
        {
        case E_TIMER:
            P6OUT ^= 0x01;
            timer_set(delay);
            break;
        case E_BUTTON:
            delay_index ^= 0x01;
            delay = delays[delay_index];
            break;
        case E_STICK_UPDATE:
            delay = delays[((stick_event_t*)msg)->x < -500 ? 0 : ((stick_event_t*)msg)->x < 500 ? 1 : 2];
            break;
        }
        o_free(msg);
    }
}

thread_t blink_thread;

int main(void)
{
    DCOCTL |= 0xE0;
    BCSCTL1 |= 0x03;

    P6DIR |= 0x01;

    scheduler_init();
    timerInit();

    blink_thread = sched_thread_start(blink, 12345, blink_stack, 100);
    buttons_init();
    stick_init();

    while(1)
    {
        delay(500000);
        
    }
    return 0;
}
