/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "DALI_communication.h"
#include "DALI_addressing.h"
#include "DALI_diagnostics_and_maintenance.h"
#include "driver/gptimer.h"
#include "esp_log.h"
#include "Wifi_provisioning.h"
#include <wifi_provisioning/manager.h>
#include "Tcp_server.h"
#include "../managed_components/espressif__mdns/include/mdns.h"
#include "mDNS_handler.h"
#include "State_manager.h"
#include "Nvs_handler.h"

void taskOne(void *parameter)
{
    while (true)
    {
        printf("Task one...\n");
        vTaskDelay(15000 / portTICK_PERIOD_MS);
        vTaskDelete(NULL);
    }
}

void run(void *parameter)
{
    while (true)
    {
        State_t current_state = get_state();
        switch (current_state)
        {
        case SYSTEM_RUNNING_STATE:
            printf("System OK\n");
            break;

        default:
            break;
        }

        Controle_gear_values_t controle_gear_1 = fetch_controle_gear_data(0x00);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        Controle_gear_values_t controle_gear_2 = fetch_controle_gear_data(0x01);
        printObject(controle_gear_1);
        printObject(controle_gear_2);

        // uint8_t driversOnBus = commission_bus();
        // printf("Drivers on bus: %d\n", driversOnBus);

        printf("Turn light off...\n");
        send_DALI_Tx(0xFE00);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("incrementer: %d\n", incrementer);
        printf("incrementer2: %d\n", incrementer2);
        printf("Turn light on...\n");
        send_DALI_Tx(0xFEFE);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("incrementer: %d\n", incrementer);
        printf("incrementer2: %d\n", incrementer2);

        printf("Blinking lamp 0\n");
        send_DALI_Tx(0x0000);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        send_DALI_Tx(0x00FE);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("Blinking lamp 1\n");
        send_DALI_Tx(0x0200);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        send_DALI_Tx(0x02FE);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    init_state_manager();
    xTaskCreate(state_task, "state_task", 512, NULL, 5, NULL);
    init_nvs_handler();
    init_wifi_provisioning(); // Error handling is already handled in the init function
    set_state(DALI_COMMUNICATION_INIT_STATE);
    init_DALI_communication();
    set_state(ANALYZE_DALI_BUS_STATE);
    DALI_Status check = check_drivers_commissioned();

    // uint8_t masterValue = 210;
    // nvs_write_uint8("GigaTest", masterValue);

    // printf("Value read: %d\n", nvs_read_uint8("GigaTest"));

    if (check == DALI_OK)
    {
        set_state(SYSTEM_RUNNING_STATE);
        printf("All drivers commissioned\n");
    }
    else if (check == DALI_ERR_BUS_NOT_COMMISIONED)
    {
        set_state(DALI_BUS_NOT_COMMISIONED_STATE);
        printf("Bus not commissioned\n");
    }
    else if (check == DALI_ERR_NO_RESPONSE_ON_BUS)
    {
        set_state(NO_RESPONSE_ON_DALI_BUS);
        printf("No response on the bus\n");
    }
    else if (check == DALI_ERR_BUS_CORRUPTED)
    {
        set_state(DALI_BUS_CORRUPTED_STATE);
        printf("Uncommissioned driver\n");
    }
    else
    {
        printf("Unknown error: %d\n", check);
    }

    xTaskCreate(tcp_server_task, "tcp_server", 4096, NULL, 5, NULL);
    mDNS_init();
    xTaskCreatePinnedToCore(run, "task two", 2048, NULL, 2, NULL, 0);

    /* Initialize NVS partition */
}
