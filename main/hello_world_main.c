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
#include "constants.h"
#include "Input_button.h"
#include "Http_handler.h"
#include "device.h"

void process_DALI_response(DALI_Status response);

Device_t devices_on_bus[64];

static const char *TAG = "app_main";

uint8_t devices_on_bus_count = 0;
uint8_t uncommissioned_devices_on_bus_count = 0;
address24_t uncommissioned_devices_on_bus_addresses[64];

uint8_t error_counter = 0;
bool await_user_action = false;
EventGroupHandle_t tcpEventGroup;

uint8_t selected_driver = 0;
char popID[10];

char error_message[128];
bool error_message_send = false;

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
    tcpEventGroup = xEventGroupCreate();
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

            // TODO Remove for production
            // char *access_token_namespace = "authentication";
            // const char *key = "refresh_token";
            // nvs_delete_key_value_pair(access_token_namespace, key);
            set_state(INIT_POP_STATE);
            break;

        case INIT_POP_STATE:
            strcpy(popID, "abcd1234");
            set_state(AWAIT_WIFI_PROVISIONING_STATE);
            break;

        case AWAIT_WIFI_PROVISIONING_STATE:
            init_wifi_provisioning(popID);
            ESP_LOGI(TAG, "Wifi provisioning initialized");
            set_state(DALI_COMMUNICATION_INIT_STATE);
            break;

        case DALI_COMMUNICATION_INIT_STATE:
            init_DALI_communication();
            ESP_LOGI(TAG, "DALI communication initialized");
            set_state(TCP_SERVER_INIT_STATE);
            break;

        case TCP_SERVER_INIT_STATE:
            xTaskCreate(tcp_server_task, "tcp_server", 4096, NULL, 5, NULL);
            ESP_LOGI(TAG, "TCP server initialized");
            set_state(MDNS_INIT_STATE);
            break;

        case MDNS_INIT_STATE:
            mDNS_init(popID);
            ESP_LOGI(TAG, "mDNS initialized");
            set_state(ANALYZE_DALI_BUS_STATE);
            break;

        case ANALYZE_DALI_BUS_STATE:
            ESP_LOGI(TAG, "Analyzing DALI bus");
            DALI_Status check = check_drivers_commissioned(devices_on_bus, &devices_on_bus_count, &uncommissioned_devices_on_bus_count, uncommissioned_devices_on_bus_addresses);
            ESP_LOGI(TAG, "Short addresses on bus: %u", devices_on_bus_count);
            ESP_LOGI(TAG, "Uncommissioned devices on bus: %u", uncommissioned_devices_on_bus_count);
            for (uint8_t i = 0; i < uncommissioned_devices_on_bus_count; i++)
            {
                ESP_LOGI(TAG, "Uncommissioned device on bus: %lu", uncommissioned_devices_on_bus_addresses[i]);
            }
            process_DALI_response(check); // Assert if OK or some ERROR and set state.
            break;

        case DALI_COMMISION_BUS_STATE: // Ensure that this state is working
            ESP_LOGI(TAG, "Commissioning DALI bus");
            devices_on_bus_count = commission_bus();
            for (size_t i = 0; i < devices_on_bus_count; i++)
            {
                devices_on_bus[i].short_address = i; // short_addresses_on_bus[i] = i;
            }
            set_state(ANALYZE_DALI_BUS_STATE);
            break;

        case DALI_COMMUNICATION_OK_STATE:
            set_state(SYNCRONIZE_NVS_STATE);
            break;

        case SYNCRONIZE_NVS_STATE:
            ESP_LOGI(TAG, "Synchronizing NVS");
            nvs_synchronize(devices_on_bus, devices_on_bus_count);
            for (size_t i = 0; i < devices_on_bus_count; i++)
            {
                ESP_LOGI(TAG, "struct short address %d manufactoringID: %llu", devices_on_bus[i].short_address, devices_on_bus[i].manufactoring_id);
            }
            set_state(AUTHENTICATION_STATE);
            break;

        case AUTHENTICATION_STATE:
            ESP_LOGI(TAG, "System authentication");
            if (authenticated(devices_on_bus, devices_on_bus_count))
            {
                ESP_LOGI(TAG, "Refresh token is stored in NVS \nFetching access token");
                refresh_token();
                set_state(SYNCRONIZE_BACKEND_STATE);
                break;
            }
            else
            {
                set_state(NOT_AUTHENTICATED_STATE);
                vTaskDelay(3000 / portTICK_PERIOD_MS);
            }
            break;

        case SYNCRONIZE_BACKEND_STATE:
            ESP_LOGI(TAG, "Synchronizing backend");
            uint64_t manufactoring_ids_fetched[64];

            size_t number_of_manufactoring_ids_fetched = 0;
            uint64_t *res = getControleGearsRemote(&number_of_manufactoring_ids_fetched);

            bool new_controle_gear_found; // = false;

            printf("number of id fetched: %d\n", number_of_manufactoring_ids_fetched);
            for (size_t i = 0; i < number_of_manufactoring_ids_fetched; i++)
            {
                printf("Manufactoring id fetched: %llu\n", res[i]);
            }

            // printf("Number of manufactoring ids fetched: %u\n", number_of_manufactoring_ids_fetched);
            for (size_t i = 0; i < devices_on_bus_count; i++)
            {
                new_controle_gear_found = true;
                for (size_t j = 0; j < number_of_manufactoring_ids_fetched; j++)
                {
                    if (devices_on_bus[i].manufactoring_id == res[j])
                    {
                        new_controle_gear_found = false;
                        vTaskDelay(2000 / portTICK_PERIOD_MS);
                        break;
                    }
                }
                if (new_controle_gear_found)
                {
                    ESP_LOGI(TAG, "New controle gear found: %llu", devices_on_bus[i].manufactoring_id);
                    add_controle_gear_to_db(devices_on_bus[i].manufactoring_id);
                    vTaskDelay(3000 / portTICK_PERIOD_MS);
                }
                else
                {
                    ESP_LOGI(TAG, "Controle gear already in database: %llu", devices_on_bus[i].manufactoring_id);
                }
            }
            ESP_LOGI(TAG, "Synchronizing backend done");
            set_state(SYSTEM_RUNNING_STATE);
            break;

        case SYSTEM_RUNNING_STATE:
            ESP_LOGI(TAG, "System running");
            for (size_t i = 0; i < devices_on_bus_count; i++)
            {
                Controle_gear_values_t controle_gear = fetch_controle_gear_data(devices_on_bus[i].short_address);
                post_controle_gear_data(&controle_gear);
            }
            const EventBits_t tcpEventBits = xEventGroupWaitBits(tcpEventGroup, TCP_EVENT_BIT, pdTRUE, pdFALSE, ONE_HOUR);
            break;

            // ****************************   Error states    ****************************

        case NO_WIFI_STATE: // This state is already handled in the Wifi_provisioning.c
            ESP_LOGE(TAG, "No wifi");
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            break;

        case DALI_BUS_CORRUPTED_STATE:
            ESP_LOGE(TAG, "DALI bus corrupted");
            devices_on_bus_count = 0;
            uncommissioned_devices_on_bus_count = 0;
            if (error_counter < 3)
            {
                error_counter++;
                ESP_LOGI(TAG, "Trying to re-analyze DALI bus");
                set_state(ANALYZE_DALI_BUS_STATE);
                vTaskDelay(3000 / portTICK_PERIOD_MS);
            }
            else
            {
                // sprintf(error_message, "{\"message\":\"DALI bus corrupted\", \"popID\":\"%s\"}", popID);
                // send_tcp_message(error_message);
                error_counter = 0;
            }
            break;

        case DALI_BUS_NOT_COMMISIONED_STATE:
            ESP_LOGE(TAG, "DALI bus not commissioned");
            if (await_user_action)
            {
                // sprintf(error_message, "{\"message\":\"DALI bus not commissioned\", \"popID\":\"%s\"}", popID);
                // send_tcp_message(error_message);
                await_user_action = true;
            }
            break;

        case NO_RESPONSE_ON_DALI_BUS:
            ESP_LOGE(TAG, "No response on DALI bus");
            if (error_counter < 3)
            {
                ESP_LOGI(TAG, "Trying to re-analyze DALI bus");
                set_state(ANALYZE_DALI_BUS_STATE);
                error_counter++;
            }
            else if (!error_message_send)
            {
                // sprintf(error_message, "{\"message\":\"No response on DALI bus\", \"popID\":\"%s\"}", popID);
                // send_tcp_message(error_message);
                error_counter = 0;
            }
            else
            {
                error_counter = 0;
            }
            vTaskDelay(3000 / portTICK_PERIOD_MS);
            break;

        case NOT_AUTHENTICATED_STATE:
            ESP_LOGE(TAG, "System not authenticated");
            if (authenticated(devices_on_bus, devices_on_bus_count))
            {
                ESP_LOGI(TAG, "Refresh token is stored in NVS \nFetching access token");
                refresh_token();
                set_state(SYNCRONIZE_BACKEND_STATE);
            }
            vTaskDelay(3000 / portTICK_PERIOD_MS);
            break;

            // ****************************   Other states    ****************************

        case BLINK_LAMP_STATE:
            ESP_LOGI(TAG, "Blinking lamp");
            uint16_t cmd_turn_on = 0xFEFE; // Turn all lamps on //(selected_driver << 9) | 0xFE;
            uint16_t cmd_turn_off = selected_driver << 9;
            send_DALI_Tx(cmd_turn_off);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            send_DALI_Tx(cmd_turn_on);
            vTaskDelay(500 / portTICK_PERIOD_MS);
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
