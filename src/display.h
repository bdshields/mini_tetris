/*
 * display.h
 *
 *  Created on: 21 May 2023
 *      Author: brandon
 */

#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_

#include "display_geometry.h"
#include "raster.h"

#define PARTITIONS 3



#if  PARTITIONS == 2
#define ROW_REPEAT(_row) (_row),(_row)
#elif  PARTITIONS == 3
#define ROW_REPEAT(_row) (_row),(_row),(_row)
#else
#define ROW_REPEAT(_row) (_row)
#endif

#define LN_PART(_0,_1,_2,_3,_4,_5,_6,_7) (_0|_1|_2|_3|_4|_5|_6|_7)


// MACRO to allow future pixel shading
#define PIXEL_GRAD(_value) (!!_value)
// MACRO to map constant image data to raw data
#define DISP_LN(_0,_1,_2,_3,_4,_5,_6,_7) LN_PART(PIXEL_GRAD(_0) , (PIXEL_GRAD(_1) << 1) , (PIXEL_GRAD(_2) << 2) , (PIXEL_GRAD(_3) << 3) ,\
                                            (PIXEL_GRAD(_4) << 4) , (PIXEL_GRAD(_5) << 5) , (PIXEL_GRAD(_6) << 6) , (PIXEL_GRAD(_7) << 7))

#define DISP_ROW(_row) ROW_REPEAT(~(0x01 << _row))
#define ROW_BLANKS() ROW_REPEAT(0xFF),ROW_REPEAT(0xFF),ROW_REPEAT(0xFF),ROW_REPEAT(0xFF),  \
                                            ROW_REPEAT(0xFF),ROW_REPEAT(0xFF),ROW_REPEAT(0xFF),ROW_REPEAT(0xFF)



void disp_init();
void disp_blank();

void disp_copy_image(raster_t image);


#endif /* SRC_DISPLAY_H_ */
