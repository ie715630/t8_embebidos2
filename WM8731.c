/*
 * WM8731.c
 *
 *  Created on: Nov 12, 2020
 *      Author: kevin
 */

#include "WM8731.h"

#define DEVICE_ADDR 0x1A

void init_codec()
{

    uint8_t reset_reg_as_data[] = {(0x0F<<1)};
    i2c_multiple_write(DEVICE_ADDR, reset_reg_as_data, 1);

//    PRINTF("Reset success");

}
