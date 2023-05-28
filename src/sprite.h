/*
 * sprite.h
 *
 *  Created on: 28 May 2023
 *      Author: brandon
 */

#ifndef SRC_SPRITE_H_
#define SRC_SPRITE_H_

#include <stdint.h>

typedef struct sprite_s{
    uint8_t width;
    uint8_t height;
    uint8_t pixels[];
}sprite_t;


#endif /* SRC_SPRITE_H_ */
