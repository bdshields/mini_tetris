/*
 * gfx.c
 *
 *  Created on: 3 Sept 2023
 *      Author: brandon
 */

#include <string.h>
#include "gfx.h"
#include "font.h"
#include "epaper.h"

#define MAX_SCALE (32/5)

void gfx_scale_font(uint8_t input, uint32_t *output, int scale)
{
    int bits;
    uint32_t temp=0;
    if(scale > MAX_SCALE) scale = MAX_SCALE;
    uint8_t mask = (1 << scale) -1 ;
    for(bits = 0; bits < 8; bits++)
    {
        temp <<= scale;
        if(input & 0x80)
        {
            temp |= mask;
        }
        input <<= 1;
    }
    *output = temp;
}

#define PIX_INSIDE(_x,_y) ((((_y) >= 0) && ((_y) < canvas_height))   &&   (((_x) >= 0) && ((_x) < (canvas_width*8))))
#define PIX_ADDR(_x,_y) (((_y) * canvas_width) + ((_x)/8))
#define PIX_SET(_x)  ( (uint8_t)0x80 >> ((_x) & 0x07))
#define PIX_CLR(_x)  (~PIX_SET(_x))

/**
 * \param canvas        - where to put output
 * \param canvas_width  - width of canvas array
 * \param canvas_height - height of canvas array
 * \param x - coordinate of top left corner of output
 * \param y - coordinate of top left corner of output
 *
 */
void gfx_put_char(uint8_t *canvas, int canvas_width, int canvas_height, int x, int y, char letter, int font_size)
{
    int font_x;
    int font_y;
    int output_x;
    int output_y;
    int duplicate_y;

    uint8_t *font;


    font = get_char(letter);
    output_y = y;

    // FOR each row of font pixels
    for(font_y = 0; font_y< 7; font_y++)
    {
        uint32_t scaled_x_bit_test;
        uint32_t temp;
        gfx_scale_font(font[font_y], &temp, font_size);
        for(duplicate_y = 0; duplicate_y < font_size; duplicate_y ++)
        {
            output_x = x;
            for(scaled_x_bit_test = ((uint32_t)0x00000001) << (5 * font_size); scaled_x_bit_test > 0; scaled_x_bit_test >>= 1)
            {
                if( ! PIX_INSIDE(output_x,output_y))
                {
                    break;
                }
                if(scaled_x_bit_test & temp)
                {
                    canvas[PIX_ADDR(output_x,output_y)] |= PIX_SET(output_x);
                }
                else
                {
                    canvas[PIX_ADDR(output_x,output_y)] &= ~PIX_SET(output_x);
                }
                output_x ++;
            }
            output_y++;
            if(output_y >= canvas_height)
            {
                return;
            }
        }
    }
}

void gfx_print(uint8_t *canvas, int canvas_width, int canvas_height, int x, int y, char *string, int font_size)
{
    while(*string != 0)
    {
        gfx_put_char(canvas, canvas_width, canvas_height, x, y, *string++, font_size);
        x += (5*font_size) + 1 + ((font_size)>>2);
    }
}

void gfx_paint(uint8_t *canvas, int canvas_width, int canvas_height, int start_x, int start_y, int end_x, int end_y)
{
    int internal_x;
    int internal_y;

    int step_x;
    int step_y;


}
