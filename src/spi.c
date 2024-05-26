/*
 * spi.c
 *
 *  Created on: 13 Aug 2023
 *      Author: brandon
 */

#include <msp430.h>
#include "spi.h"

void spi_init(void)
{
    // Clock and MOSI
    P3SEL |= 0x0A;
    P3DIR |= 0x0A;

    // put in reset
    U0CTL |= SWRST;

    U0CTL |= CHAR | SYNC | MM;
    // Mode 0
    U0TCTL |= SSEL1 | STC | CKPH;
    // run at 2Mhz
    U0MCTL = 0;
    U0BR1 = 0;
    U0BR0 = 4;

    U0ME |= USPIE0;

    U0CTL &= ~SWRST;
}

void spi_trx(uint8_t const *tx_buffer, uint8_t *rx_buffer, int length)
{
    U0IFG &= ~URXIFG0;
    while(length > 0)
    {
        // load TX buffer
        U0TXBUF = *tx_buffer++;
        // wait for RX
        while( ! (U0IFG & URXIFG0)) ;
        // Save RX
        if(rx_buffer)
        {
            *rx_buffer++ = U0RXBUF;
        }
        else
        {
            U0IFG &= ~URXIFG0;
        }
        length--;
    }
}
