#include <msp430.h>
#include <stdint.h>

#include "os_scheduler.h"
#include "os_msg.h"
#include "os_event.h"

#include "timer.h"
#include "buttons.h"
#include "analog_stick.h"
#include "o_allocator.h"


#include "button_tests.h"
#include "display.h"

void __attribute__ ((noinline)) delay(uint32_t delay)
{
    volatile uint32_t counter = delay;
    while(counter--)
    {
        
    }
}


int main(void)
{
    DCOCTL |= 0xE0;
    BCSCTL1 |= 0x03;

    P6DIR |= 0x01;

    scheduler_init();
    timerInit();

    buttons_init();
    stick_init();

    button_test_init();

    while(1)
    {
        delay(2500);

    }
    return 0;
}
