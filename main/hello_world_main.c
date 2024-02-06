/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "DALI_transmit.h"
#include "driver/gptimer.h"
#include "esp_log.h"


void taskOne(void *parameter){
    while (true)
    {
        printf("Task one...\n");
        vTaskDelay(15000/portTICK_PERIOD_MS);
        vTaskDelete(NULL);
    }    
}

void taskTwo(void *parameter){
    while (true)
    {
        sendDALI_TX(0xFE00);
        printf("Turn off %d\n", incrementer);
        vTaskDelay(1000/portTICK_PERIOD_MS);  
        sendDALI_TX(0xFEFE);
        printf("Turn on %d\n", incrementer);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        
        printf("Load ddr1 with value\n");
        sendDALI_TX(0xA3FF);
        //sendDALI_TX(0xA300);
        vTaskDelay(1000/portTICK_PERIOD_MS);

        printf("Get ddr1 %d\n", incrementer);
        sendDALI_TX(0xFF98); //Query ddr1

        vTaskDelay(1000/portTICK_PERIOD_MS);
        printf("rx_data_buffer: ");
        for(int i = 0; i < 8; i++) {
            printf("%d", rx_data_buffer[i]);
        }
        printf("\n");
    }
}


void app_main(void)
{
    init_DALI_transmit();

    xTaskCreatePinnedToCore(taskOne, "task one", 2048, NULL, 2, NULL, 0);
    
    xTaskCreatePinnedToCore(taskTwo, "task two", 2048, NULL, 2, NULL, 0);


        
}
