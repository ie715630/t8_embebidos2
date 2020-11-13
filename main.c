/**
 * @file    t8.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK66F18.h"
#include "fsl_debug_console.h"

/* TODO: insert other include files here. */
#include "fsl_i2c.h"
#include "semphr.h"
#include "freertos_i2c.h"
#include "FreeRTOS.h"
#include "task.h"
#include "WM8731.h"
/* TODO: insert other definitions and declarations here. */

SemaphoreHandle_t modules_initialized;

void init_modules(void * params);

void audio_codec(void *params);

/*
 * @brief   Application entry point.
 */
int main(void) {

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    BOARD_InitDebugConsole();

    modules_initialized= xSemaphoreCreateBinary();

    xTaskCreate(init_modules, "Init", 200, NULL, 1, NULL);

    vTaskStartScheduler();

    while(1) {
        __asm volatile ("nop");
    }
    return 0 ;
}

void init_modules(void * params)
{

    init_i2c0_with_default_config();
    vTaskDelay(pdMS_TO_TICKS(100));

	init_codec();

    xSemaphoreGive(modules_initialized);
    vTaskSuspend(NULL);
    
    for(;;)
    {
        // Nothing
    }
}

void audio_codec(void *params)
{
	xSemaphoreTake(modules_initialized, portMAX_DELAY);
	for(;;)
	{
		//Audio functions
		vTaskDelay(pdMS_TO_TICKS(300));
	}

}
