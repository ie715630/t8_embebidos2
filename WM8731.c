/*
 * WM8731.c
 *
 *  Created on: Nov 12, 2020
 *      Author: kevin
 */

#include "WM8731.h"

#define DEVICE_ADDR 0x1A
#define RESET_ADDR 0x0F
#define LEFT_LINE_IN 0x00
#define RIGHT_LINE_IN 0x01
#define ANALOGUE_AUDIO 0x04
#define LEFT_HP_OUT 0x02
#define RIGHT_HP_OUT 0x03
#define INTERFACE_FORMAT     0x07
#define SAMPLING_CONTROL     0x08

void init_codec()
{

    uint8_t i2c_data[2] = {RESET_ADDR, 0x00};
    i2c_multiple_write(DEVICE_ADDR, i2c_data, 2);
    
    uint8_t db = 0; //0x00 (-34.5) - 0x1F (12)
    db = (db+34.5)/1.5;
    i2c_data[0] = LEFT_LINE_IN;
    i2c_data[1] = db;
    i2c_multiple_write(DEVICE_ADDR, i2c_data, 2);

    i2c_data[0] = RIGHT_LINE_IN;
    i2c_data[1] = db;
    i2c_multiple_write(DEVICE_ADDR, i2c_data, 2);

    // uint8_t vol = 0x79 // 0dB: 0x30 (-73dB) - 0x7F (6dB)
}
