/*
 * gfx.h
 *
 *  Created on: 3 Sept 2023
 *      Author: brandon
 */

#ifndef SRC_GFX_H_
#define SRC_GFX_H_

#include <stdint.h>
void gfx_print(uint8_t *canvas, int canvas_width, int canvas_height, int x, int y, char *string, int font_size);

#endif /* SRC_GFX_H_ */
