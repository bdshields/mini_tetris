/*
 * display.h
 *
 *  Created on: 21 May 2023
 *      Author: brandon
 */

#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_


void disp_init();
void disp_blank();

void disp_pixel_on(int x, int y);

void disp_pixel_linear(int l);


#endif /* SRC_DISPLAY_H_ */
