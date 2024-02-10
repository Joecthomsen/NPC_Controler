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
#include "DALI_addressing.h"
#include "driver/gptimer.h"
#include "esp_log.h"

void taskOne(void *parameter)
{
    while (true)
    {
        printf("Task one...\n");
        vTaskDelay(15000 / portTICK_PERIOD_MS);
        vTaskDelete(NULL);
    }
}

void taskTwo(void *parameter)
{
    while (true)
    {

        initDALIAddressing();
        generateRandomDALIAddress();
        address24_t address = findLowestAddress(0, 0xFFFFFF);
        printf("Address: %lx\n", address);
        vTaskDelay(5000 / portTICK_PERIOD_MS);

        printf("query randAddr..\n");
        sendDALI_TX(0xFFC2);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if (newDataAvailable())
        {
            printf("Hex value: %x\n", getNewData());
            clearNewDataFlag();
        }

        printf("Turn light off...\n");
        sendDALI_TX(0xFE00);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        printf("incrementer: %d\n", incrementer);
        printf("incrementer2: %d\n", incrementer2);
        printf("Turn light on...\n");
        sendDALI_TX(0xFEFE);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("incrementer: %d\n", incrementer);
        printf("incrementer2: %d\n", incrementer2);

        // printf("Load ddr1 with value F0\n");
        // sendDALI_TX(0xA3F0);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
        // sendDALI_TX(0xFF98); // Query ddr1
        // vTaskDelay(1000 / portTICK_PERIOD_MS);

        // if (newDataAvailable())
        // {
        //     printf("Hex value: %x\n", getNewData());
        //     clearNewDataFlag();
        // }

        // printf("Load ddr1 with value 33\n");
        // sendDALI_TX(0xA333);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
        // sendDALI_TX(0xFF98); // Query ddr1
        // vTaskDelay(1000 / portTICK_PERIOD_MS);

        // if (newDataAvailable())
        // {
        //     printf("Hex value: %x\n", getNewData());
        //     clearNewDataFlag();
        // }

        // printf("Load ddr1 with value 00\n");
        // sendDALI_TX(0xA300);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
        // sendDALI_TX(0xFF98); // Query ddr1
        // vTaskDelay(1000 / portTICK_PERIOD_MS);

        // if (newDataAvailable())
        // {
        //     printf("Hex value: %x\n", getNewData());
        //     clearNewDataFlag();
        // }

        // printf("Load ddr1 with value AA\n");
        // sendDALI_TX(0xA3AA);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
        // sendDALI_TX(0xFF98); // Query ddr1
        // vTaskDelay(1000 / portTICK_PERIOD_MS);

        // if (newDataAvailable())
        // {
        //     printf("Hex value: %x\n", getNewData());
        //     clearNewDataFlag();
        // }

        // printf("Load ddr1 with value FF\n");
        // sendDALI_TX(0xA3FF);
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
        // sendDALI_TX(0xFF98); // Query ddr1
        // vTaskDelay(1000 / portTICK_PERIOD_MS);

        // if (newDataAvailable())
        // {
        //     printf("Hex value: %x\n", getNewData());
        //     clearNewDataFlag();
        // }
    }
}

void app_main(void)
{
    init_DALI_transmit();

    xTaskCreatePinnedToCore(taskOne, "task one", 2048, NULL, 2, NULL, 0);

    xTaskCreatePinnedToCore(taskTwo, "task two", 2048, NULL, 2, NULL, 0);
}
