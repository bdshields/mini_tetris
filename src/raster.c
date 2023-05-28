/*
 * raster.c
 *
 *  Created on: 28 May 2023
 *      Author: brandon
 */


#include "raster.h"

void raster_paste_sprite(raster_t *raster, sprite_t *sprite, int x, int y)
{
    int dest_row;
    int src_row;
    uint8_t row_mask = ((1 << sprite->width) - 1) << x;
    if(y < 0)
    {
        src_row = 0-y;
        y=0;
    }
    else
    {
        src_row = 0;
    }


    for (dest_row = y; dest_row<DISP_MAX_Y && src_row < sprite->height; dest_row++, src_row++)
    {
        // clear destination pixels
        (*raster)[dest_row] &= ~row_mask;
        (*raster)[dest_row] |= sprite->pixels[src_row] << x;
    }
}
