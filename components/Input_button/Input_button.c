#include <stdio.h>
#include "Input_button.h"
#include "freertos/FreeRTOS.h"

#include "esp_log.h"

static const char *TAG = "INPUT BUTTON";

void reset_task(void *pvParameters)
{

    // wifi_prov_mgr_reset_provisioning();

    esp_restart();
    vTaskDelete(NULL);
}

static void gpio_isr_handler_test(void *arg)
{
    xTaskCreate(reset_task, "reset_task", 4098, NULL, 9, NULL);
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
    gpio_isr_handler_add(GPIO_NUM_1, gpio_isr_handler_test, (void *)GPIO_NUM_1);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to add GPIO ISR handler for input button");
    }
    else
    {
        ESP_LOGI(TAG, "GPIO ISR handler for input button added successfully");
    }
}