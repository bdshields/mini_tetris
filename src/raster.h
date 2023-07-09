/*
 * raster.h
 *
 *  Created on: 28 May 2023
 *      Author: brandon
 */

#ifndef SRC_RASTER_H_
#define SRC_RASTER_H_

#include <stdint.h>
#include <string.h>
#include "display_geometry.h"
#include "pos.h"
#include "sprite.h"

typedef uint8_t raster_t[DISP_MAX_Y];

void raster_paste_sprite(raster_t *raster, sprite_t *sprite, pos_t pos);
void raster_or_raster(raster_t *raster, raster_t *src_raster);

inline void raster_clear(raster_t *raster)
{
    memset(*raster, 0, sizeof(raster_t));
}

int raster_check_overlap(raster_t *raster, sprite_t *sprite, pos_t pos);
int raster_check_outside(sprite_t *sprite, pos_t pos);


#endif /* SRC_RASTER_H_ */
