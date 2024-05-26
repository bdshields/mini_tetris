/*
 * epaper.h
 *
 *  Created on: 19 Aug 2023
 *      Author: brandon
 */

#ifndef SRC_EPAPER_H_
#define SRC_EPAPER_H_

#define EPAPER_WIDTH 200
#define EPAPER_HEIGHT 200

void ep_init(void);
void ep_DisplayClear(void);
void ep_DisplayFrame(uint8_t *black, int width, int height);


typedef void (*ep_getdata_fp)(uint8_t *raster, int canvas_width, int canvas_height, int y_offset);

void ep_update_display(ep_getdata_fp data_callback, int refresh);

#endif /* SRC_EPAPER_H_ */
