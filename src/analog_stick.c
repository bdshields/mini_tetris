/*
 * analog_stick.c
 *
 *  Created on: 13 Dec. 2022
 *      Author: brandon
 */

#include <msp430.h>
#include "analog_stick.h"
#include "o_allocator.h"
#include "evt_subscribe.h"

o_allocator stick_ev_allocator;
stick_event_t stick_ev_pool[10];

DECLARE_EVT_SUB(stick_requests, 2);

void stick_send_event(int16_t x, int16_t y);

static inline int int_abs(int value)
{
    if(value > 0)
        return value;
    else
        return 0-value;
}


__attribute__((interrupt(ADC12_VECTOR)))
void isr_adc(void)
{
    int x,y;
    static int last_x, last_y, repeat = 0;

    x = (int)ADC12MEM14 - STICK_MAX_X;
    if(int_abs(x) < STICK_DEAD_BAND)
    {
        x = 0;
    }
    y = (int)ADC12MEM15 - STICK_MAX_Y;
    if(int_abs(y) < STICK_DEAD_BAND)
    {
        y = 0;
    }

#if 0
    // Raw data
    stick_send_event(x, y);
#else
    // Filtered D-PAD with delayed repetition

    if(int_abs(x >> 1) > int_abs(y))
    {
        // X is more than 2x that of Y
        x = x > 0 ? 1 : -1;
        y =0;
    }
    else if(int_abs(y >> 1) > int_abs(x))
    {
        // Y is more than 2x that of X
        y = y > 0 ? 1 : -1;
        x =0;
    }
    else
    {
        x =0;
        y =0;
    }
    if(  !  ((x == last_x) && (y == last_y)))
    {
        // If direction has changed
        stick_send_event(x, y);
        last_x = x;
        last_y = y;
        repeat = 5;
    }
    else if(repeat == 0)
    {
        // Start repeating after delay
        stick_send_event(x, y);
    }
    else
    {
        repeat--;
    }
#endif

    ADC12IFG = 0;
    ADC12CTL0 |= ADC12SC;
}


void stick_init(void)
{
    evt_sub_init(&stick_requests);
    stick_ev_allocator = o_initialise(stick_ev_pool, 10, sizeof(stick_ev_pool[0]));
    P6SEL |= 0x0C;
    P6DIR &= ~0x0C;
    // The max resistance from the analog sticks is 10K ohms.
    // Sampling time should be > 5us
    ADC12CTL0 = ADC12ON;
    ADC12CTL1 = CONSEQ_1;
    ADC12CTL0 |= SHT1_5 | SHT0_5 | MSC;
    ADC12CTL1 |= CSTARTADD_14 | SHS_0 | SHP | ADC12DIV_7 | ADC12SSEL_1;
    ADC12MCTL14 = SREF_0 | INCH_2;
    ADC12MCTL15 = SREF_0 | INCH_3 | EOS;

    ADC12IE = 0x8000;
    ADC12CTL0 |= ENC;
    ADC12CTL0 |= ADC12SC;
}

void stick_send_event(int16_t x, int16_t y)
{
    stick_event_t *event = o_allocate(stick_ev_allocator);
    if(event)
    {
        event->super.event = E_STICK_UPDATE;
        event->pos = _POS(x,y);
        evt_send(&stick_requests, event);
    }
}

void stick_request(void)
{
    evt_subscribe(&stick_requests);
}

void stick_cancel(void)
{
    evt_unsubscribe(&stick_requests);
}

