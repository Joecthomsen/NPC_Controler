#include <stdio.h>
#include "Input_button.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <wifi_provisioning/manager.h>

#include "esp_log.h"
#include "Nvs_handler.h"

static const char *TAG = "INPUT BUTTON";

static TimerHandle_t hold_timer = NULL;

void reset_task(void *pvParameters)
{

    wifi_prov_mgr_reset_provisioning();
    // nvs_flash_erase();
    char key[] = "authentication";
    char refresh_token[] = "refresh_token";
    char access_token[] = "access_token";
    nvs_delete_key_value_pair(key, refresh_token);
    nvs_delete_key_value_pair(key, access_token);
    esp_restart();
    vTaskDelete(NULL);
}

void button_press_callback()
{
    // Start the timer when the button is pressed
    hold_timer = xTimerCreate("HoldTimer", BUTTON_HOLD_DURATION, pdFALSE, (void *)0, reset_task);
    xTimerStart(hold_timer, 0);
}

void button_release_callback()
{
    // If the button is released before the timer expires, stop the timer
    if (hold_timer != NULL)
    {
        xTimerStop(hold_timer, 0);
        xTimerDelete(hold_timer, 0);
        hold_timer = NULL;
    }
}

static void gpio_input_button_isr_handler(void *arg)
{
    // BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (gpio_get_level(GPIO_NUM_1) == 1)
    {
        button_press_callback();
    }
    else
    {
        button_release_callback();
    }

    // // Inform the task manager that a higher priority task has been woken up
    // vTaskNotifyGiveFromISR(NULL, &xHigherPriorityTaskWoken);
    // // Switch context if necessary
    // if (xHigherPriorityTaskWoken == pdTRUE)
    // {
    //     portYIELD_FROM_ISR();
    // }

    // xTaskCreate(reset_task, "reset_task", 4098, NULL, 9, NULL);
}

void init_input_button()
{

    esp_err_t err;

    // Configure the GPIO pin RX
    gpio_config_t input_button = {
        .mode = INPUT_BUTTON_PIN_DIRECTION,
        .intr_type = INPUT_BUTTON_INTERRUPT_TYPE,
        .pin_bit_mask = (1ULL << GPIO_NUM_1),
        .pull_down_en = INPUT_BUTTON_PIN_PULLUP,
        .pull_up_en = INPUT_BUTTON_PIN_PULLUP,

    };

    err = gpio_config(&input_button); // Init the GPIO
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to configure GPIO for input button");
    }

    err = gpio_install_isr_service(4);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to install input button interrupt service");
    }
    gpio_isr_handler_add(GPIO_NUM_1, gpio_input_button_isr_handler, (void *)GPIO_NUM_1);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to add GPIO ISR handler for input button");
    }
    else
    {
        ESP_LOGI(TAG, "GPIO ISR handler for input button added successfully");
    }
}