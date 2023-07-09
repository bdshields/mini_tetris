/*
 * raster.c
 *
 *  Created on: 28 May 2023
 *      Author: brandon
 */


#include "raster.h"

void raster_or_sprite(raster_t *raster, sprite_t *sprite, pos_t pos)
{
    int dest_row;
    int src_row;
    uint8_t row_mask;
    if(pos.x >= 0)
    {
        row_mask = ((1 << sprite->width) - 1) << pos.x;
    }
    else
    {
        row_mask = ((1 << sprite->width) - 1) >> (0-pos.x);
    }

    if(pos.y < 0)
    {
        src_row = 0-pos.y;
        pos.y=0;
    }
    else
    {
        src_row = 0;
    }


    for (dest_row = pos.y; dest_row<DISP_MAX_Y && src_row < sprite->height; dest_row++, src_row++)
    {
        // clear destination pixels
        //(*raster)[dest_row] &= ~row_mask;
        if(pos.x >= 0)
        {
            (*raster)[dest_row] |= sprite->pixels[src_row] << pos.x;
        }
        else
        {
            (*raster)[dest_row] |= sprite->pixels[src_row] >> (0-pos.x);
        }
    }
}

void raster_or_raster(raster_t *raster, raster_t *src_raster)
{
    int dest_row;

    for (dest_row = 0; dest_row<DISP_MAX_Y; dest_row++)
    {
        (*raster)[dest_row] |= (*src_raster)[dest_row];
    }
}

int raster_check_overlap(raster_t *raster, sprite_t *sprite, pos_t pos)
{
    int dest_row;
    int src_row;
    int result = 0;

    if(pos.y < 0)
    {
        src_row = 0-pos.y;
        pos.y=0;
    }
    else
    {
        src_row = 0;
    }


    for (dest_row = pos.y; dest_row<DISP_MAX_Y && src_row < sprite->height; dest_row++, src_row++)
    {
        // Check overlap
        if(pos.x >= 0)
        {
            result |= (*raster)[dest_row] & (sprite->pixels[src_row] << pos.x);
        }
        else
        {
            result |= (*raster)[dest_row] & (sprite->pixels[src_row] >> (0-pos.x));
        }
    }
    return !!result;
}

int raster_check_outside(sprite_t *sprite, pos_t pos)
{
    int counter;
    uint8_t mask;
    int result=0;

    // Check y < 0
    if(pos.y < 0)
    {
        if((pos.y + sprite->height) >= 0)
        {
            for(counter = 0; counter < (0-pos.y); counter++)
            {
                result |= sprite->pixels[counter] ? 0x01 : 0;
            }
        }
        else
        {
            // no part of sprite is within bounds
            result |= 0x10;
            goto done;
        }
    }

    // Check y > DISP_MAX_Y
    if((pos.y + sprite->height) > DISP_MAX_Y)
    {
        if(pos.y < DISP_MAX_Y)
        {
            for(counter = (DISP_MAX_Y - pos.y); counter < sprite->height; counter++)
            {
                result |= sprite->pixels[counter] ? 0x02 : 0;
            }
        }
        else
        {
            // no part of sprite is within bounds
            result |= 0x10;
            goto done;
        }
    }

    // Check x < 0
    if(pos.x < 0)
    {
        if((pos.x + sprite->width) > 0)
        {
            mask = (1 << (0 - pos.x)) - 1;
            for(counter = 0; counter < sprite->height; counter++)
            {
                result |= (sprite->pixels[counter] & mask) ? 0x04 : 0;
            }
        }
        else
        {
            // no part of sprite is within bounds
            result |= 0x10;
            goto done;
        }
    }

    // Check x > DISP_MAX_X
    if((pos.x + sprite->width) > DISP_MAX_X)
    {
        if(pos.x < DISP_MAX_X)
        {
            mask = 0xFF << (pos.x + sprite->width) - DISP_MAX_X;
            for(counter = 0; counter < sprite->height; counter++)
            {
                result |= (sprite->pixels[counter] & mask) ? 0x08 : 0;
            }
        }
        else
        {
            // no part of sprite is within bounds
            result |= 0x10;
            goto done;
        }
    }
done:
    return result;
}

