/*
 * display.c
 *
 *  Created on: 21 May 2023
 *      Author: brandon
 */


#include <msp430.h>
#include <stdint.h>

void disp_init()
{
    P1OUT=0;
    P2OUT=255;
    P5OUT=255;

    P1DIR=255;
    P2DIR=255;
    P5DIR=255;
}

void disp_blank()
{
    P1OUT=0;
    P2OUT=255;
    P5OUT=255;
}



void disp_pixel_on(int x, int y)
{
    uint8_t x_mask = 1 << x;
    uint8_t y_mask;
    if(y > 7)
    {
        y -= 8;
        y_mask = 1 << y;
        P2OUT &= ~y_mask;
    }
    else
    {
        y_mask = 1 << y;
        P5OUT &= ~y_mask;
    }
    P1OUT |= x_mask;


}

void disp_pixel_linear(int l)
{
    disp_pixel_on(l & 0x07, l >> 3);
}
