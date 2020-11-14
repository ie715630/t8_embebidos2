/*
 * freertos_i2c.c
 *
 *  Created on: 11 oct. 2020
 *      Author: kcdia
 */

#include "freertos_i2c.h"

#include "fsl_i2c.h"
#include "fsl_clock.h"
#include "fsl_port.h"

#include "FreeRTOS.h"
#include "semphr.h"

#define SERIAL_PORTS 3

typedef struct
{
    uint8_t is_initialized;
    i2c_master_handle_t handle;
    SemaphoreHandle_t mutex;
    SemaphoreHandle_t transfer_sem;
} freertos_i2c_handle_t;

static freertos_i2c_handle_t i2c_handle = {0};
uint8_t g_ack_received = 0;

freertos_i2c_flag_t init_i2c0_with_default_config(void)
{
    i2c_handle.is_initialized = 0;

    /* Create mutex for general i2c use and semaphore for i2c transfers */
    i2c_handle.mutex = xSemaphoreCreateMutex();
    i2c_handle.transfer_sem = xSemaphoreCreateBinary();

    /* Initialize pins for I2C0 */
    CLOCK_EnableClock(kCLOCK_PortE);
//    PORT_SetPinMux(PORTE, 24, kPORT_MuxAlt5);
//    PORT_SetPinMux(PORTE, 25, kPORT_MuxAlt5);

    port_pin_config_t config;
    config.slewRate = 1;
    config.pullSelect = 2;
    config.mux = kPORT_MuxAlt5;

    PORT_SetPinConfig(PORTE, 24, &config);
    PORT_SetPinConfig(PORTE, 25, &config);


    // pull up, slew rate, y deshabilitar params y desconectar codec


    /* Use default config values for i2c master and enable interruption */
    i2c_master_config_t mstr_cfg;
    I2C_MasterGetDefaultConfig(&mstr_cfg);
    I2C_MasterInit(I2C0, &mstr_cfg, CLOCK_GetFreq(I2C0_CLK_SRC));
    NVIC_SetPriority(I2C0_IRQn, 5);

    /* Create transfer handle */
    I2C_MasterTransferCreateHandle(I2C0, &i2c_handle.handle, fsl_i2c_callback,
            NULL);

    i2c_handle.is_initialized = 1;

    return freertos_i2c_success;
}
void fsl_i2c_callback(I2C_Type *base, i2c_master_handle_t *handle,
        status_t status, void *userData)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (kStatus_Success == status)
    {
        xSemaphoreGiveFromISR(i2c_handle.transfer_sem,
                &xHigherPriorityTaskWoken);
        g_ack_received = 1;
    }
    else
    {
        xSemaphoreGiveFromISR(i2c_handle.transfer_sem,
                &xHigherPriorityTaskWoken);
        g_ack_received = 0;
    }

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken);
}

freertos_i2c_flag_t i2c_multiple_write(uint8_t slave_addr, uint8_t * write_data,
        uint8_t write_size)
{
    if(i2c_handle.is_initialized)
    {
        i2c_master_transfer_t transfer_content;

        transfer_content.slaveAddress = slave_addr;
        transfer_content.direction = kI2C_Write;
        transfer_content.subaddressSize = 0;
        transfer_content.data = write_data;
        transfer_content.dataSize = write_size;
        transfer_content.flags = kI2C_TransferDefaultFlag;

        xSemaphoreTake(i2c_handle.mutex, portMAX_DELAY);
        I2C_MasterTransferNonBlocking(I2C0, &i2c_handle.handle, &transfer_content);
        xSemaphoreTake(i2c_handle.transfer_sem, portMAX_DELAY);
        xSemaphoreGive(i2c_handle.mutex);

        if (g_ack_received)
        {
        	return freertos_i2c_success;
        }
        else
        {
        	return freertos_i2c_fail;
        }

    }
    else
    {
        return freertos_i2c_fail;
    }
}

freertos_i2c_flag_t i2c_multiple_read(uint8_t slave_addr, uint8_t wr_command,
        uint8_t * read_data, uint8_t read_size)
{	if(i2c_handle.is_initialized)
    {
        i2c_master_transfer_t wr_transfer_content;
        i2c_master_transfer_t rd_transfer_content;

        wr_transfer_content.slaveAddress = slave_addr;
        wr_transfer_content.direction = kI2C_Write;
        wr_transfer_content.subaddressSize = 0;
        wr_transfer_content.data = &wr_command;
        wr_transfer_content.dataSize = 1;
        wr_transfer_content.flags = kI2C_TransferNoStopFlag;

        xSemaphoreTake(i2c_handle.mutex, portMAX_DELAY);
        I2C_MasterTransferNonBlocking(I2C0, &i2c_handle.handle,
                &wr_transfer_content);
        xSemaphoreTake(i2c_handle.transfer_sem, portMAX_DELAY);

        rd_transfer_content.slaveAddress = slave_addr;
        rd_transfer_content.direction = kI2C_Read;
        rd_transfer_content.subaddressSize = 0;
        rd_transfer_content.data = read_data;
        rd_transfer_content.dataSize = read_size;
        rd_transfer_content.flags = kI2C_TransferRepeatedStartFlag;

        I2C_MasterTransferNonBlocking(I2C0, &i2c_handle.handle,
                &rd_transfer_content);
        xSemaphoreTake(i2c_handle.transfer_sem, portMAX_DELAY);
        xSemaphoreGive(i2c_handle.mutex);

        return freertos_i2c_success;
    }
    else
    {
        return freertos_i2c_fail;
    }
}
