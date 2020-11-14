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

uint8_t init_codec()
{
	// slew rate
	volatile freertos_i2c_flag_t transfer_result;
    uint8_t return_value = 1;

    uint8_t i2c_data[2] = {RESET_ADDR, 0x00};
    transfer_result = i2c_multiple_write(DEVICE_ADDR, i2c_data, 2);
    if (transfer_result == freertos_i2c_fail)
    {
       return_value = 0;
    }

    uint8_t db = 0; //0x00 (-34.5) - 0x1F (12)
    db = (db+34.5)/1.5;
    i2c_data[0] = LEFT_LINE_IN;
    i2c_data[1] = db;
    transfer_result = i2c_multiple_write(DEVICE_ADDR, i2c_data, 2);
    if (transfer_result == freertos_i2c_fail)
    {
       return_value = 0;
    }

    i2c_data[0] = RIGHT_LINE_IN;
    i2c_data[1] = db;
    transfer_result = i2c_multiple_write(DEVICE_ADDR, i2c_data, 2);
    if (transfer_result == freertos_i2c_fail)
    {
       return_value = 0;
    }
    
    i2c_data[0] = 0xC;
	i2c_data[1] = 0x00;
	transfer_result = i2c_multiple_write(DEVICE_ADDR, i2c_data, 2);
	if (transfer_result == freertos_i2c_fail)
	{
	   return_value = 0;
	}

    // uint8_t vol = 0x79 // 0dB: 0x30 (-73dB) - 0x7F (6dB)
    return return_value;
}
