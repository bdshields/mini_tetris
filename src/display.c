/*
 * display.c
 *
 *  Created on: 21 May 2023
 *      Author: brandon
 */


/**
 * Display is 8 columns and 16 rows.
 * Each raster is scanned row by row
 */


#include <msp430.h>
#include <stdint.h>

#include "display.h"

const uint8_t disp_row_top_ctrl[]=
{
        DISP_ROW(0),
        DISP_ROW(1),
        DISP_ROW(2),
        DISP_ROW(3),
        DISP_ROW(4),
        DISP_ROW(5),
        DISP_ROW(6),
        DISP_ROW(7),
        ROW_BLANKS(),
};
const uint8_t disp_row_bottom_ctrl[]=
{
        ROW_BLANKS(),
        DISP_ROW(0),
        DISP_ROW(1),
        DISP_ROW(2),
        DISP_ROW(3),
        DISP_ROW(4),
        DISP_ROW(5),
        DISP_ROW(6),
        DISP_ROW(7)
};

uint8_t disp_raster[DISP_MAX_Y * PARTITIONS];

#if PARTITIONS ==3
const uint8_t part_mask[]={0x07, 0x18, 0xE0};
#elif PARTITIONS == 2
const uint8_t part_mask[]={0x0F, 0xF0};
#else
const uint8_t part_mask[]={0xFF};
#endif

#if 1
const uint8_t disp_image_glass[]={
        DISP_LN(1,0,0,0,0,0,0,1),
        DISP_LN(1,0,0,0,0,0,0,1),
        DISP_LN(1,0,0,0,0,0,0,1),
        DISP_LN(1,0,0,0,0,0,0,1),
        DISP_LN(0,1,0,0,0,0,1,0),
        DISP_LN(0,0,1,0,0,1,0,0),
        DISP_LN(0,0,0,1,1,0,0,0),
        DISP_LN(0,0,0,1,1,0,0,0),
        DISP_LN(0,0,0,1,1,0,0,0),
        DISP_LN(0,0,0,1,1,0,0,0),
        DISP_LN(0,0,1,1,1,1,0,0),
        DISP_LN(0,0,0,1,1,0,0,0),
        DISP_LN(0,0,0,1,1,0,0,0),
        DISP_LN(0,0,0,1,1,0,0,0),
        DISP_LN(0,0,1,1,1,1,0,0),
        DISP_LN(1,1,0,0,0,0,1,1),
};
#else
const uint8_t disp_image_slope[]={
        DISP_LN(1,0,0,0,0,0,0,0),
        DISP_LN(1,0,0,0,0,0,0,0),
        DISP_LN(1,1,0,0,0,0,0,0),
        DISP_LN(1,1,0,0,0,0,0,0),
        DISP_LN(1,1,1,0,0,0,0,0),
        DISP_LN(1,1,1,0,0,0,0,0),
        DISP_LN(1,1,1,1,0,0,0,0),
        DISP_LN(1,1,1,1,0,0,0,0),
        DISP_LN(1,1,1,1,1,0,0,0),
        DISP_LN(1,1,1,1,1,0,0,0),
        DISP_LN(1,1,1,1,1,1,0,0),
        DISP_LN(1,1,1,1,1,1,0,0),
        DISP_LN(1,1,1,1,1,1,1,0),
        DISP_LN(1,1,1,1,1,1,1,0),
        DISP_LN(1,1,1,1,1,1,1,1),
        DISP_LN(1,1,1,1,1,1,1,1),
};
#endif

void disp_init()
{
    /*
     * Initialise GPIO
     */
    P1OUT=0;
    P2OUT=255;
    P5OUT=255;

    P1DIR=255;
    P2DIR=255;
    P5DIR=255;

    /**
     * Initialise DMA
     */
    DMACTL0 = DMA0TSEL_8 | DMA1TSEL_8 | DMA2TSEL_8;
    DMACTL1 = 0;

    DMA0SA = disp_row_top_ctrl;
    DMA0DA = &P5OUT;
    DMA0SZ = sizeof(disp_row_top_ctrl);
    DMA0CTL = DMASBDB | DMASRCINCR_3 | DMADSTINCR_0 | DMADT_4;

    DMA1SA = disp_row_bottom_ctrl;
    DMA1DA = &P2OUT;
    DMA1SZ = sizeof(disp_row_bottom_ctrl);
    DMA1CTL = DMASBDB | DMASRCINCR_3 | DMADSTINCR_0 | DMADT_4;

    DMA2SA = disp_raster;
    DMA2DA = &P1OUT;
    DMA2SZ = sizeof(disp_raster);
    DMA2CTL = DMASBDB | DMASRCINCR_3 | DMADSTINCR_0 | DMADT_4;

    DMA0CTL |= DMAEN;
    DMA1CTL |= DMAEN;
    DMA2CTL |= DMAEN;

    /**
     * Initialise Timer B
     *
     * input clock 32khz
     * Aiming for 50hz refresh rate
     */
    TBCCR0 = 32767.0 / (25 *  32 * PARTITIONS);
    TBCTL = TBSSEL_1 | ID_0;
    TBCTL |= MC_1;


    disp_copy_image(disp_image_glass);
}

/**
 * Clear the display raster
 */
void disp_blank()
{
    memset(disp_raster,0, sizeof(disp_raster));
}

/**
 * Copy an image to the display raster
 */
void disp_copy_image(raster_t image)
{
    int row;
    int part;
    for(row=0; row < DISP_MAX_Y; row++)
    {
        for(part=0; part < PARTITIONS; part++)
        {
            disp_raster[(row * PARTITIONS) + part] = image[row] & part_mask[part];
        }
    }
}

