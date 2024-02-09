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
        printf("Turn light off...\n");
        sendDALI_TX(0xFE00);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        printf("incrementer: %d\n", incrementer);
        printf("incrementer2: %d\n", incrementer2);
        printf("Turn light on...\n");
        sendDALI_TX(0xFEFE);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        printf("incrementer: %d\n", incrementer);
        printf("incrementer2: %d\n", incrementer2);


        // printf("Load ddr1 with value F0\n");
        // sendDALI_TX(0xA3F0);
        // vTaskDelay(1000/portTICK_PERIOD_MS);
        // printf("Get DATA %d\n", incrementer);
        // sendDALI_TX(0xFF98); //Query ddr1
        // vTaskDelay(1000/portTICK_PERIOD_MS);
        // printf("rx_data_buffer: ");
        // for(int i = 0; i < 8; i++) {
        //     printf("%d", rx_data_buffer[i]);
        //     // printf(" : ");
        // }
        // printf("\n");


        // printf("Load ddr1 with value FF\n");
        // sendDALI_TX(0xA3FF);
        // vTaskDelay(1000/portTICK_PERIOD_MS);
        // printf("incrementer: %d\n", incrementer);
        // printf("incrementer2: %d\n", incrementer2);
        // sendDALI_TX(0xFF98); //Query ddr1
        // printf("incrementer: %d\n", incrementer);
        // printf("incrementer2: %d\n", incrementer2);
        // vTaskDelay(1000/portTICK_PERIOD_MS);

        // printf("rx_data_buffer: ");
        // for(int i = 0; i < 8; i++) {
        //     printf("%d", rx_data_buffer[i]);
        //     printf(" : ");
        // }
        // printf("\n");


        
        // printf("Load ddr1 with value 00\n");
        // sendDALI_TX(0xA300);
        // //sendDALI_TX(0xA300);
        // vTaskDelay(1000/portTICK_PERIOD_MS);
        // printf("Get DATA %d\n", incrementer);
        // sendDALI_TX(0xFF98); //Query ddr1
        // vTaskDelay(1000/portTICK_PERIOD_MS);
        // printf("rx_data_buffer 3: ");
        // for(int i = 0; i < 8; i++) {
        //     printf("%d", rx_data_buffer[i]);
        // }
        // printf("\n");
    }
}


void app_main(void)
{
    init_DALI_transmit();

    xTaskCreatePinnedToCore(taskOne, "task one", 2048, NULL, 2, NULL, 0);
    
    xTaskCreatePinnedToCore(taskTwo, "task two", 2048, NULL, 2, NULL, 0);


        
}
