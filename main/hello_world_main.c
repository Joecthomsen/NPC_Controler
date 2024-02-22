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

static const char *TAG = "app_main";

uint8_t short_addresses_on_bus_count = 0;
uint8_t short_addresses_on_bus[64];

uint8_t uncommissioned_devices_on_bus_count = 0;
address24_t uncommissioned_devices_on_bus_addresses[64];

void process_DALI_response(DALI_Status response);

uint8_t error_counter = 0;
bool await_user_action = false;

void taskOne(void *parameter)
{
    while (true)
    {
        printf("Task one...\n");
        vTaskDelay(15000 / portTICK_PERIOD_MS);
        vTaskDelete(NULL);
    }
}

void app_main(void)
{
    init_state_manager();
    xTaskCreate(state_task, "state_task", 2048, NULL, 5, NULL);
    State_t current_state;
    while (true)
    {
        current_state = get_state();
        switch (current_state)
        {
            // ****************************   Normal states    ****************************

        case NVS_INIT_STATE:
            init_nvs_handler();
            ESP_LOGI(TAG, "NVS initialized");
            set_state(AWAIT_WIFI_PROVISIONING_STATE);
            break;

        case AWAIT_WIFI_PROVISIONING_STATE:
            init_wifi_provisioning();
            ESP_LOGI(TAG, "Wifi provisioning initialized");
            set_state(DALI_COMMUNICATION_INIT_STATE);
            break;

        case DALI_COMMUNICATION_INIT_STATE:
            init_DALI_communication();
            ESP_LOGI(TAG, "DALI communication initialized");
            set_state(TCP_SERVER_INIT_STATE);
            break;

        case TCP_SERVER_INIT_STATE:
            xTaskCreate(tcp_server_task, "tcp_server", 2048, NULL, 5, NULL);
            ESP_LOGI(TAG, "TCP server initialized");
            set_state(MDNS_INIT_STATE);
            break;

        case MDNS_INIT_STATE:
            mDNS_init();
            ESP_LOGI(TAG, "mDNS initialized");
            set_state(ANALYZE_DALI_BUS_STATE);
            break;

        case ANALYZE_DALI_BUS_STATE:
            ESP_LOGI(TAG, "Analyzing DALI bus");
            DALI_Status check = check_drivers_commissioned(&short_addresses_on_bus_count, short_addresses_on_bus, &uncommissioned_devices_on_bus_count, uncommissioned_devices_on_bus_addresses);
            ESP_LOGI(TAG, "Short addresses on bus: %u", short_addresses_on_bus_count);
            ESP_LOGI(TAG, "Uncommissioned devices on bus: %u", uncommissioned_devices_on_bus_count);
            for (uint8_t i = 0; i < short_addresses_on_bus_count; i++)
            {
                ESP_LOGI(TAG, "Short address on bus: %u", short_addresses_on_bus[i]);
            }
            for (uint8_t i = 0; i < uncommissioned_devices_on_bus_count; i++)
            {
                ESP_LOGI(TAG, "Uncommissioned device on bus: %lu", uncommissioned_devices_on_bus_addresses[i]);
            }
            process_DALI_response(check); // Assert if OK or some ERROR and set state.
            break;

        case DALI_COMMUNICATION_OK_STATE:
            set_state(SYSTEM_RUNNING_STATE);
            break;

        case SYSTEM_RUNNING_STATE:
            ESP_LOGI(TAG, "System running");
            for (size_t i = 0; i < short_addresses_on_bus_count; i++)
            {
                Controle_gear_values_t controle_gear = fetch_controle_gear_data(short_addresses_on_bus[i]);
                printObject(controle_gear);
            }
            vTaskDelay(ONE_HOUR);
            break;

            // ****************************   Error states    ****************************

        case NO_WIFI_STATE: // This state is already handled in the Wifi_provisioning.c
            ESP_LOGE(TAG, "No wifi");
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            break;

        case DALI_BUS_CORRUPTED_STATE:
            ESP_LOGE(TAG, "DALI bus corrupted");
            if (error_counter < 3)
            {
                error_counter++;
                ESP_LOGI(TAG, "Trying to re-analyze DALI bus");
                set_state(ANALYZE_DALI_BUS_STATE);
                vTaskDelay(3000 / portTICK_PERIOD_MS);
            }
            else
            {
                send_tcp_message("{\"status\":\"DALI bus corrupted\"}");
                error_counter = 0;
            }
            break;

        case DALI_BUS_NOT_COMMISIONED_STATE:
            ESP_LOGE(TAG, "DALI bus not commissioned");
            if (await_user_action)
            {
                send_tcp_message("{\"status\":\"DALI bus not commissioned\"}");
                await_user_action = true;
            }
            break;

        case NO_RESPONSE_ON_DALI_BUS:
            ESP_LOGE(TAG, "No response on DALI bus");
            if (error_counter < 3)
            {
                vTaskDelay(3000 / portTICK_PERIOD_MS);
                ESP_LOGI(TAG, "Trying to re-analyze DALI bus");
                set_state(ANALYZE_DALI_BUS_STATE);
                error_counter++;
            }
            else
            {
                send_tcp_message("{\"status\":\"No response on DALI bus\"}");
                error_counter = 0;
            }
            break;

        default:
            ESP_LOGE(TAG, "Unknown state at line %d in run_task", __LINE__);
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            break;
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void process_DALI_response(DALI_Status response)
{
    if (response == DALI_OK)
    {
        ESP_LOGI(TAG, "DALI bus OK");
        set_state(DALI_COMMUNICATION_OK_STATE);
    }
    else if (response == DALI_ERR_BUS_NOT_COMMISIONED)
    {
        ESP_LOGE(TAG, "DALI bus not commisioned");
        set_state(DALI_BUS_NOT_COMMISIONED_STATE);
    }
    else if (response == DALI_ERR_NO_RESPONSE_ON_BUS)
    {
        ESP_LOGE(TAG, "No response on DALI bus");
        set_state(NO_RESPONSE_ON_DALI_BUS);
    }
    else if (response == DALI_ERR_BUS_CORRUPTED)
    {
        ESP_LOGE(TAG, "DALI bus corrupted");
        set_state(DALI_BUS_CORRUPTED_STATE);
    }
    else
    {
        ESP_LOGE(TAG, "Unknown error");
    }
}

// void run(void *parameter)
// {
//     while (true)
//     {
//         State_t current_state = get_state();
//         switch (current_state)
//         {
//         case SYSTEM_RUNNING_STATE:
//             printf("System OK\n");
//             break;

//         default:
//             break;
//         }

//         Controle_gear_values_t controle_gear_1 = fetch_controle_gear_data(0x00);
//         vTaskDelay(1000 / portTICK_PERIOD_MS);
//         Controle_gear_values_t controle_gear_2 = fetch_controle_gear_data(0x01);
//         printObject(controle_gear_1);
//         printObject(controle_gear_2);

//         // uint8_t driversOnBus = commission_bus();
//         // printf("Drivers on bus: %d\n", driversOnBus);

//         printf("Turn light off...\n");
//         send_DALI_Tx(0xFE00);
//         vTaskDelay(1000 / portTICK_PERIOD_MS);
//         printf("incrementer: %d\n", incrementer);
//         printf("incrementer2: %d\n", incrementer2);
//         printf("Turn light on...\n");
//         send_DALI_Tx(0xFEFE);
//         vTaskDelay(1000 / portTICK_PERIOD_MS);
//         printf("incrementer: %d\n", incrementer);
//         printf("incrementer2: %d\n", incrementer2);

//         printf("Blinking lamp 0\n");
//         send_DALI_Tx(0x0000);
//         vTaskDelay(1000 / portTICK_PERIOD_MS);
//         send_DALI_Tx(0x00FE);
//         vTaskDelay(1000 / portTICK_PERIOD_MS);
//         printf("Blinking lamp 1\n");
//         send_DALI_Tx(0x0200);
//         vTaskDelay(1000 / portTICK_PERIOD_MS);
//         send_DALI_Tx(0x02FE);
//         vTaskDelay(1000 / portTICK_PERIOD_MS);
//     }
// }

// void app_main(void)
// {
//     init_state_manager();
//     xTaskCreate(state_task, "state_task", 512, NULL, 5, NULL);
//     init_nvs_handler();
//     init_wifi_provisioning(); // Error handling is already handled in the init function
//     set_state(DALI_COMMUNICATION_INIT_STATE);
//     init_DALI_communication();
//     set_state(ANALYZE_DALI_BUS_STATE);
//     DALI_Status check = check_drivers_commissioned();

//     // uint8_t masterValue = 210;
//     // nvs_write_uint8("GigaTest", masterValue);

//     // printf("Value read: %d\n", nvs_read_uint8("GigaTest"));

//     if (check == DALI_OK)
//     {
//         set_state(SYSTEM_RUNNING_STATE);
//         printf("All drivers commissioned\n");
//     }
//     else if (check == DALI_ERR_BUS_NOT_COMMISIONED)
//     {
//         set_state(DALI_BUS_NOT_COMMISIONED_STATE);
//         printf("Bus not commissioned\n");
//     }
//     else if (check == DALI_ERR_NO_RESPONSE_ON_BUS)
//     {
//         set_state(NO_RESPONSE_ON_DALI_BUS);
//         printf("No response on the bus\n");
//     }
//     else if (check == DALI_ERR_BUS_CORRUPTED)
//     {
//         set_state(DALI_BUS_CORRUPTED_STATE);
//         printf("Uncommissioned driver\n");
//     }
//     else
//     {
//         printf("Unknown error: %d\n", check);
//     }

//     xTaskCreate(tcp_server_task, "tcp_server", 4096, NULL, 5, NULL);
//     mDNS_init();
//     xTaskCreatePinnedToCore(run, "task two", 2048, NULL, 2, NULL, 0);

//     /* Initialize NVS partition */
// }
