/*
 * epaper.c
 *
 *  Created on: 13 Aug 2023
 *      Author: brandon
 */

/**
 * Driver for epd1in54b
 * https://www.waveshare.com
 */

#include <msp430.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "spi.h"
#include "timer.h"
#include "epaper.h"
#include "gfx.h"

#define CS_PIN   0x01
#define DC_PIN   0x04
#define RST_PIN  0x40
#define BUSY_PIN 0x80




// EPD1IN54B commands
#define PANEL_SETTING                               0x00
#define POWER_SETTING                               0x01
#define POWER_OFF                                   0x02
#define POWER_OFF_SEQUENCE_SETTING                  0x03
#define POWER_ON                                    0x04
#define POWER_ON_MEASURE                            0x05
#define BOOSTER_SOFT_START                          0x06
#define DEEP_SLEEP                                  0x07
#define DATA_START_TRANSMISSION_1                   0x10
#define DATA_STOP                                   0x11
#define DISPLAY_REFRESH                             0x12
#define DATA_START_TRANSMISSION_2                   0x13
#define VCOM1_LUT                                   0x20
#define WHITE_LUT                                   0x21
#define BLACK_LUT                                   0x22
#define GRAY1_LUT                                   0x23
#define GRAY2_LUT                                   0x24
#define VCOM2_LUT                                   0x25
#define RED0_LUT                                    0x26
#define RED1_LUT                                    0x27
#define PLL_CONTROL                                 0x30
#define TEMPERATURE_SENSOR_COMMAND                  0x40
#define TEMPERATURE_SENSOR_CALIBRATION              0x41
#define TEMPERATURE_SENSOR_WRITE                    0x42
#define TEMPERATURE_SENSOR_READ                     0x43
#define VCOM_AND_DATA_INTERVAL_SETTING              0x50
#define LOW_POWER_DETECTION                         0x51
#define TCON_SETTING                                0x60
#define TCON_RESOLUTION                             0x61
#define SOURCE_AND_GATE_START_SETTING               0x62
#define GET_STATUS                                  0x71
#define AUTO_MEASURE_VCOM                           0x80
#define VCOM_VALUE                                  0x81
#define VCM_DC_SETTING_REGISTER                     0x82
#define PROGRAM_MODE                                0xA0
#define ACTIVE_PROGRAM                              0xA1
#define READ_OTP_DATA                               0xA2

void ep_SendCommand(unsigned char command);
void ep_SendData(unsigned char data);
void ep_SendCmdData(unsigned char command, const uint8_t data[], unsigned short length);
void ep_WaitUntilIdle(void);
void ep_Reset(void);

void ep_setlut(void);
void ep_setlut_quick(void);

void ep_delay(uint32_t milliSeconds)
{
    uint32_t until = timer_get_value() + milliSeconds + TIMER_INTERVAL;
    while(1)
    {
        if(timer_get_value() > until)
        {
            break;
        }
    }
}

void ep_init(void)
{
    P3DIR |= (CS_PIN | DC_PIN | RST_PIN);
    P3OUT |= (RST_PIN | CS_PIN);
    spi_init();

    /* EPD hardware init start */
    ep_Reset();
    ep_SendCmdData(POWER_SETTING, (const uint8_t[]){0x07, 0x00, 0x08, 0x00}, 4);
    ep_SendCmdData(BOOSTER_SOFT_START, (const uint8_t[]){0x07, 0x07, 0x07}, 3);

    ep_SendCommand(POWER_ON);

    ep_WaitUntilIdle();

    ep_SendCmdData(PANEL_SETTING, (const uint8_t[]){0xcf}, 1);
    ep_SendCmdData(VCOM_AND_DATA_INTERVAL_SETTING, (const uint8_t[]){0x07}, 1);
    ep_SendCmdData(PLL_CONTROL, (const uint8_t[]){0x39}, 1);
    ep_SendCmdData(TCON_RESOLUTION, (const uint8_t[]){0xC8, 0x00, 0xC8}, 3);

    ep_SendCmdData(VCM_DC_SETTING_REGISTER, (const uint8_t[]){0x0E}, 1);

    ep_setlut();
}

/**
 * convert 1 bit input data, to 2 bit output data
 */
uint16_t ep_bitmap(uint8_t data)
{
    int bits;
    uint16_t scaled=0;
    for(bits = 0; bits < 8; bits++)
    {
        scaled <<= 2;
        if(data & 0x80)
        {
            scaled |= 0x03;
        }
        data <<= 1;
    }
    return scaled;
}

/**
 * Display frame.
 * each sequence of pixels is divided into groups of 4, 4 shades each pixel
 * white == 0b11
 * black == 0b00
 */
void ep_DisplayFrame(uint8_t *black, int width, int height)
{
    int x;
    int y;
    int y2=0;
    uint8_t shade;
    ep_SendCommand(DATA_START_TRANSMISSION_1);
    for(y=0; y < EPAPER_HEIGHT; y++)
    {
        for(x=0; x < (EPAPER_WIDTH)/8; x++)
        {
        //    if((y < height) && (x < width))
            {
                uint16_t data;
                data = ep_bitmap(black[(y2 * width) + x]);
                ep_SendData((data >>8) & 0xFF);
                ep_SendData(data & 0xFF);
            }
//            else
//            {
//                ep_SendData(0);
//                ep_SendData(0);
//            }
        }
        y2++;
        if(y2 > (height-1))
        {
            y2 = 0;
        }
    }
    ep_delay(10);
    ep_SendCommand(DATA_START_TRANSMISSION_2);
    shade = 0;
    for(y=0; y < EPAPER_HEIGHT; y++)
    {
        for(x=0; x < (EPAPER_WIDTH + 8)/8; x++)
        {
            ep_SendData(shade);
        }
    }
    ep_delay(10);
    ep_SendCommand(DISPLAY_REFRESH);
    ep_WaitUntilIdle();
}




/**
 * Display frame.
 * each sequence of pixels is divided into groups of 4, 4 shades each pixel
 * white == 0b11
 * black == 0b00
 */
void ep_update_display(ep_getdata_fp data_callback, int quick)
{
    int display_line;
    int retrieved_lines;
    int x;
    int y;
    uint8_t shade;
#define RETRIEVAL_RATE 2
    uint8_t raster[RETRIEVAL_RATE][(EPAPER_WIDTH)/8];
    if(quick)
    {
        ep_setlut_quick();
    }
    else
    {
        ep_setlut();
    }
    ep_SendCommand(DATA_START_TRANSMISSION_1);
    display_line = 0;
    while(display_line < EPAPER_HEIGHT)
    {
        // Get data from client
        retrieved_lines = EPAPER_HEIGHT - display_line;
        if(retrieved_lines > RETRIEVAL_RATE)
        {
            retrieved_lines = RETRIEVAL_RATE;
        }
        memset(raster,0x00, sizeof(raster));
        data_callback(raster, EPAPER_WIDTH/8, retrieved_lines, display_line);

        // Transfer data to display
        y=0;
        while(y < retrieved_lines)
        {
            for(x=0; x < (EPAPER_WIDTH)/8; x++)
            {
                uint16_t data;
                data = ep_bitmap(raster[y][x]);
                ep_SendData((data >>8) & 0xFF);
                ep_SendData(data & 0xFF);
            }
            y++;
        }
        display_line += retrieved_lines;
    }
    ep_delay(10);
    ep_SendCommand(DATA_START_TRANSMISSION_2);
    shade = 0x00;
    for(y=0; y < EPAPER_HEIGHT; y++)
    {
        for(x=0; x < (EPAPER_WIDTH + 8)/8; x++)
        {
            ep_SendData(shade);
        }
    }
    ep_delay(10);
    ep_SendCommand(DISPLAY_REFRESH);

    ep_WaitUntilIdle();
}

/**
 * 3 BYTE Pattern
 *        | b7 | b6 | b5 | b4 | b3 | b2 | b1 | b0 |
 *        |  Level  |      number of frames       |
 *        |  Level  |      number of frames       |
 *        |     Repeat Pattern                    |
 */

                               /*               RPT               RPT              RPT               RPT             RPT*/

const unsigned char lut_vcom0[] = {0x0E, 0x14, 0x01, 0x0A, 0x06, 0x04, 0x0A, 0x0A, 0x0F, 0x03, 0x03, 0x0C, 0x06, 0x0A, 0x00};

const unsigned char lut_white[] = {0x0E, 0x14, 0x01, 0x0A, 0x46, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x86, 0x0A, 0x04};
const unsigned char lut_black[] = {0x0E, 0x14, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x4A, 0x04};
const unsigned char lut_gray1[] = {0x8E, 0x94, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x0A, 0x04};
const unsigned char lut_gray2[] = {0x8E, 0x94, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x0A, 0x04};

const unsigned char lut_vcom1[] = {0x03, 0x1D, 0x01, 0x01, 0x08, 0x23, 0x37, 0x37, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const unsigned char lut_red0[] = {0x83, 0x5D, 0x01, 0x81, 0x48, 0x23, 0x77, 0x77, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const unsigned char lut_red1[] = {0x03, 0x1D, 0x01, 0x01, 0x08, 0x23, 0x37, 0x37, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


/**
 * QUICK UPDATE
 */
                                     /*               RPT               RPT              RPT               RPT               RPT*/
const unsigned char lut_vcom0_quick[] = {0x06, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const unsigned char lut_white_quick[] = {0x86, 0x0A, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const unsigned char lut_black_quick[] = {0x06, 0x4A, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const unsigned char lut_gray1_quick[] = {0x06, 0x0A, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const unsigned char lut_gray2_quick[] = {0x06, 0x0A, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const unsigned char lut_vcom1_quick[] = {0x37, 0x37, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const unsigned char lut_red0_quick[] = {0x77, 0x77, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const unsigned char lut_red1_quick[] = {0x37, 0x37, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void ep_setlut(void)
{
    ep_SendCmdData(VCOM1_LUT, lut_vcom0, sizeof(lut_vcom0));
    ep_SendCmdData(WHITE_LUT, lut_white, sizeof(lut_white));
    ep_SendCmdData(BLACK_LUT, lut_black, sizeof(lut_black));
    ep_SendCmdData(GRAY1_LUT, lut_gray1, sizeof(lut_gray1));
    ep_SendCmdData(GRAY2_LUT, lut_gray2, sizeof(lut_gray2));

    ep_SendCmdData(VCOM2_LUT, lut_vcom1, sizeof(lut_vcom1));
    ep_SendCmdData(RED0_LUT, lut_red0, sizeof(lut_red0));
    ep_SendCmdData(RED1_LUT, lut_red1, sizeof(lut_red1));
}

void ep_setlut_quick(void)
{
    ep_SendCmdData(VCOM1_LUT, lut_vcom0_quick, sizeof(lut_vcom0_quick));
    ep_SendCmdData(WHITE_LUT, lut_white_quick, sizeof(lut_white_quick));
    ep_SendCmdData(BLACK_LUT, lut_black_quick, sizeof(lut_black_quick));
    ep_SendCmdData(GRAY1_LUT, lut_gray1_quick, sizeof(lut_gray1_quick));
    ep_SendCmdData(GRAY2_LUT, lut_gray2_quick, sizeof(lut_gray2_quick));

    ep_SendCmdData(VCOM2_LUT, lut_vcom1_quick, sizeof(lut_vcom1_quick));
    ep_SendCmdData(RED0_LUT, lut_red0_quick, sizeof(lut_red0_quick));
    ep_SendCmdData(RED1_LUT, lut_red1_quick, sizeof(lut_red1_quick));
}

void ep_spiTransfer(uint8_t data)
{
    uint8_t data_out[1];

    data_out[0] = data;
    P3OUT &= ~CS_PIN;
    spi_trx(data_out, NULL, 1);
    P3OUT |= CS_PIN;
}

void ep_SendCommand(unsigned char command) {
    P3OUT &= ~DC_PIN;
    ep_spiTransfer(command);
}

/**
 *  @brief: basic function for sending data
 */
void ep_SendData(unsigned char data) {
    P3OUT |= DC_PIN;
    ep_spiTransfer(data);
}

void ep_SendCmdData(unsigned char command, const uint8_t data[], unsigned short length)
{
    ep_SendCommand(command);
    while(length-- > 0)
    {
        ep_SendData(*data++);
    }
}

void ep_Reset(void) {
    P3OUT |= RST_PIN;
    ep_delay(200);
    P3OUT &= ~RST_PIN;                //module reset
    ep_delay(20);
    P3OUT |= RST_PIN;
    ep_delay(200);
}

void ep_WaitUntilIdle(void) {
    while( ! (P3IN & BUSY_PIN));
}

