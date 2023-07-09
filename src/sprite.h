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


//#define _S(_0,_1,_2,_3,_4,_5,_6,_7) (|||||||||||_0<<)

#endif /* SRC_SPRITE_H_ */
