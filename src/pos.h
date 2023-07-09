/*
 * pos.h
 *
 *  Created on: 14 Jun. 2023
 *      Author: brandon
 */

#ifndef SRC_POS_H_
#define SRC_POS_H_

#define _POS(_x,_y) (pos_t){.x = _x, .y = _y}

typedef struct pos_s{
    int x;
    int y;
}pos_t;


#endif /* SRC_POS_H_ */
