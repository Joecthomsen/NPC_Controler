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

#include "esp_event.h"
#include "esp_wifi.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

#define TAG "TCP_SERVER"

#define LISTEN_PORT 3333

static void tcp_server_task(void *pvParameters)
{
    char rx_buffer[128];
    int addr_family;
    int ip_protocol;

    while (1)
    {
        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(LISTEN_PORT);
        ip_protocol = IPPROTO_IP;

        int listen_sock = socket(AF_INET, SOCK_STREAM, ip_protocol);
        if (listen_sock < 0)
        {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0)
        {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
            close(listen_sock);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        err = listen(listen_sock, 1);
        if (err != 0)
        {
            ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
            close(listen_sock);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        ESP_LOGI(TAG, "Socket listening");

        struct sockaddr_in source_addr;
        uint addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            close(listen_sock);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        // Receive the data
        while (1)
        {
            int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            if (len < 0)
            {
                ESP_LOGE(TAG, "recv failed: errno %d", errno);
                break;
            }
            else if (len == 0)
            {
                ESP_LOGI(TAG, "Connection closed");
                break;
            }
            else
            {
                rx_buffer[len] = 0;
                ESP_LOGI(TAG, "Received %d bytes: %s", len, rx_buffer);
                send(sock, rx_buffer, len, 0);
            }
        }

        if (sock != -1)
        {
            shutdown(sock, 0);
            close(sock);
        }
        close(listen_sock);
    }
    vTaskDelete(NULL);
}

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
        DALI_Status check = check_drivers_commissioned();

        if (check == DALI_OK)
        {
            printf("All drivers commissioned\n");
        }
        else if (check == DALI_ERR_NO_DRIVERS)
        {
            printf("No drivers on the bus\n");
        }
        else if (check == DALI_ERR_UNCOMMISSIONED_DRIVER)
        {
            printf("Uncommissioned driver\n");
        }
        else
        {
            printf("Unknown error: %d\n", check);
        }

        Controle_gear controle_gear_1 = fetch_controle_gear_data(0x00);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        Controle_gear controle_gear_2 = fetch_controle_gear_data(0x01);
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

void init_wifi_provisioning_task(void *parameter)
{
    while (true)
    {
        vTaskDelay(50 / portTICK_PERIOD_MS);
        init_wifi_provisioning();
        vTaskDelete(NULL);
    }
}

void app_main(void)
{

    init_wifi_provisioning();
    init_DALI_communication();

    xTaskCreate(tcp_server_task, "tcp_server", 4096, NULL, 5, NULL);

    xTaskCreatePinnedToCore(taskOne, "task one", 2048, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(taskTwo, "task two", 2048, NULL, 2, NULL, 0);

    /* Initialize NVS partition */
}
