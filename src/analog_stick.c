/*
 * analog_stick.c
 *
 *  Created on: 13 Dec. 2022
 *      Author: brandon
 */

#include <msp430.h>
#include "analog_stick.h"
#include "o_allocator.h"

o_allocator stick_ev_allocator;
stick_event_t stick_ev_pool[10];

extern thread_t blink_thread;

__attribute__((interrupt(ADC12_VECTOR)))
void isr_adc(void)
{
    stick_event_t *event = o_allocate(stick_ev_allocator);
    if(event)
    {
        event->super.event = E_STICK_UPDATE;
        event->x = (int16_t)ADC12MEM14 - 0x07FF;
        event->y = (int16_t)ADC12MEM15 - 0x07FF;
        msg_send(blink_thread,(os_msg_p)event);
    }

    ADC12IFG = 0;
    ADC12CTL0 |= ADC12SC;
}


void stick_init(void)
{
    stick_ev_allocator = o_initialise(stick_ev_pool, 10, sizeof(stick_ev_pool[0]));
    P6SEL |= 0x0C;
    P6DIR &= ~0x0C;
    // The max resistance from the analog sticks is 10K ohms.
    // Sampling time should be > 5us
    ADC12CTL0 = ADC12ON;
    ADC12CTL1 = CONSEQ_1;
    ADC12CTL0 |= SHT1_5 | SHT0_5 | MSC;
    ADC12CTL1 |= CSTARTADD_14 | SHS_0 | SHP | ADC12DIV_7;
    ADC12CTL1 |= ADC12SSEL_1;
    ADC12MCTL14 = SREF_0 | INCH_2;
    ADC12MCTL15 = SREF_0 | INCH_3 | EOS;

    ADC12IE = 0x8000;
    ADC12CTL0 |= ENC;
    ADC12CTL0 |= ADC12SC;
}
