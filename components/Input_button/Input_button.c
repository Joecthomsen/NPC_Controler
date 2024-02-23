#include <stdio.h>
#include "Input_button.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

void input_button_isr_handler(void *arg);
void (*input_button_isr_handler_ptr)(void *) = input_button_isr_handler;
gpio_isr_handle_t *handler_gpio_isr;
gptimer_handle_t timer_input_button; // Init the Tx timer

static const char *TAG = "GPIO Input button";

void init_button(void)
{
    esp_err_t err;
    gpio_config_t io_conf;
    // disable interrupt
    io_conf.intr_type = INPUT_BUTTON_INTERRUPT_TYPE;
    // set as input mode
    io_conf.mode = INPUT_BUTTON_PIN_DIRECTION;
    // bit mask of the pin
    io_conf.pin_bit_mask = (1ULL << INPUT_BUTTON_GPIO_PIN);
    // disable pull-down mode
    io_conf.pull_down_en = INPUT_BUTTON_PIN_PULLDOWN;
    // disable pull-up mode
    io_conf.pull_up_en = INPUT_BUTTON_PIN_PULLUP;
    // configure GPIO with the given settings
    err = gpio_config(&io_conf);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to configure GPIO input pin");
    }
    else
    {
        ESP_LOGI(TAG, "GPIO input pin configured successfully");
    }

    err = gpio_isr_register(input_button_isr_handler_ptr, NULL, 0, &handler_gpio_isr);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to register GPIO button interrupt");
    }
    else
    {
        ESP_LOGI(TAG, "GPIO interrupt registered successfully");
        gpio_intr_enable(INPUT_BUTTON_GPIO_PIN);
    }

    // Configure the GP Timer
    gptimer_config_t gptimer_config_input_button =
        {
            .clk_src = CLOCK_SOURCE_INPUT_BUTTON,
            .direction = COUNTER_DIRECTION_INPUT_BUTTON,
            .resolution_hz = TIMER_FREQUENZ_INPUT_BUTTON,
            .intr_priority = TIMER_INTERRUPT_PRIORITY_INPUT_BUTTON,
        };
    err = gptimer_new_timer(&gptimer_config_input_button, &timer_input_button); // Create the new Tx timer
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to configure the Tx timer");
    }
    else
        ESP_LOGI(TAG, "Tx timer configured successfully");
}

void input_button_isr_handler(void *arg)
{
    ESP_LOGI(TAG, "GPIO button interrupt triggered");
    vTaskDelay(100 / portTICK_PERIOD_MS);
}