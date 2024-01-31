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
//#include "DALI_transmit.h"
#include "driver/gptimer.h"
#include "esp_log.h"


#define TIMER_FREQUENZ 1000000

gptimer_handle_t gptimer = NULL;   //Init the timer
gptimer_config_t gptimer_config;    //Init timer config struct
gptimer_alarm_config_t gptimer_alarm_config;    //Init alarm config struct

static const char * TAG = "MAIN_APP";
esp_err_t err;

bool toggle = false;


static bool IRAM_ATTR onTimer(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx){
    BaseType_t high_task_awoken = pdFALSE;
    toggle =!toggle;

    return (high_task_awoken == pdTRUE);
}

void taskOne(void *parameter){
    while (true)
    {
        printf("Starting timer...\n");
        err = gptimer_start(gptimer);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to start the timer");
        }
        else
            ESP_LOGI(TAG, "Timer started successfully");
        vTaskDelay(15000/portTICK_PERIOD_MS);
        vTaskDelete(NULL);
    }
        
}

void taskTwo(void *parameter){
    while (true)
    {
        //sendDALI_TX(0xFE00);
        uint64_t raw_count;
        err = gptimer_get_raw_count(gptimer, &raw_count);
        printf("counter %llu\n", raw_count);
        printf("toggle %d\n", toggle);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        
    }
}



void app_main(void)
{
    //init_DALI_transmit();

    gptimer_config.clk_src = GPTIMER_CLK_SRC_DEFAULT;   //Set clock source to default   
    gptimer_config.direction = GPTIMER_COUNT_UP;        //Set counting direction to UP   
    gptimer_config.resolution_hz = TIMER_FREQUENZ;      //Set timer frequenz (to 1MHz)                                                                
    gptimer_config.intr_priority = 1;                              
                                                        
    gptimer_alarm_config.alarm_count = 10000000;//TIMER_FREQUENZ/BAUD_RATE;  //Set the alarm trigger point
    gptimer_alarm_config.reload_count = 0; //Reload value upon alarm trigger
    gptimer_alarm_config.flags.auto_reload_on_alarm = 1;         

    err = gptimer_new_timer(&gptimer_config, &gptimer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure the timer");
    }
    else
        ESP_LOGI(TAG, "Timer configured successfully");

    err = gptimer_set_alarm_action(gptimer, &gptimer_alarm_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set alarm action");
    }
    else
        ESP_LOGI(TAG, "Alarm action set successfully");

    gptimer_event_callbacks_t cbs = 
    {
        .on_alarm = onTimer, // register user callback
    };

    err = gptimer_register_event_callbacks(gptimer, &cbs, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register event callback for timer");
    }
    else
        ESP_LOGI(TAG, "Timer event callback registered successfully");

    err = gptimer_enable(gptimer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable the timer in DALI init function");
    }
    else{
        ESP_LOGI(TAG, "Timer enabled in DALI_transmit_init() function");
    }


    xTaskCreatePinnedToCore(taskOne, "task one", 2048, NULL, 2, NULL, 0);
    
    xTaskCreatePinnedToCore(taskTwo, "task two", 2048, NULL, 2, NULL, 0);




        
/*             printf("Hello, world, once again!\n");
            vTaskDelay(2000 / portTICK_PERIOD_MS); // sleep for 2 seconds
            sendDALI_TX(off);
            vTaskDelay(2000 / portTICK_PERIOD_MS); // sleep for 2 seconds */

        
}
