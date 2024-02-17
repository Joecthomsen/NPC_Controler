#include <stdio.h>
#include "State_manager.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

State_Machine state_machine = STATE_NOT_WIFI_PROVISIONED;
bool state_manager_inited = false;

void state_task(void *parameter)
{
    while (true)
    {
        gpio_set_level(STATE_MANAGER_GPIO_PIN_RED, LOW);
        gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, HIGH);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, LOW);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void set_state(State_Machine new_state)
{
    if (state_manager_inited == false)
    {
        ESP_LOGE("State_manager", "State manager not initialized");
    }
    state_machine = new_state;
}

State_Machine get_state()
{
    return state_machine;
}

void init_state_manager()
{
    // // Configure the GPIO pin GREEN
    // gpio_config_t io_config_state_manager_green = {
    //     .mode = STATE_MANAGER_PIN_DIRECTION,
    //     .intr_type = STATE_MANAGER_INTERRUPT_TYPE,
    //     .pin_bit_mask = (1ULL << STATE_MANAGER_GPIO_PIN_GREEN),
    //     .pull_down_en = STATE_MANAGER_PIN_PULLDOWN,
    //     .pull_up_en = STATE_MANAGER_PIN_PULLUP,
    // };
    // // Configure the GPIO pin YELLOW
    // gpio_config_t io_config_state_manager_yellow = {
    //     .mode = STATE_MANAGER_PIN_DIRECTION,
    //     .intr_type = STATE_MANAGER_INTERRUPT_TYPE,
    //     .pin_bit_mask = (1ULL << STATE_MANAGER_GPIO_PIN_YELLOW),
    //     .pull_down_en = STATE_MANAGER_PIN_PULLDOWN,
    //     .pull_up_en = STATE_MANAGER_PIN_PULLUP,
    // };
    // Configure the GPIO pin BLUE
    gpio_config_t io_config_state_manager_blue = {
        .mode = STATE_MANAGER_PIN_DIRECTION,
        .intr_type = STATE_MANAGER_INTERRUPT_TYPE,
        .pin_bit_mask = (1ULL << STATE_MANAGER_GPIO_PIN_BLUE),
        .pull_down_en = STATE_MANAGER_PIN_PULLDOWN,
        .pull_up_en = STATE_MANAGER_PIN_PULLUP,
    };
    // Configure the GPIO pin RED
    gpio_config_t io_config_state_manager_red = {
        .mode = STATE_MANAGER_PIN_DIRECTION,
        .intr_type = STATE_MANAGER_INTERRUPT_TYPE,
        .pin_bit_mask = (1ULL << STATE_MANAGER_GPIO_PIN_RED),
        .pull_down_en = STATE_MANAGER_PIN_PULLDOWN,
        .pull_up_en = STATE_MANAGER_PIN_PULLUP,
    };

    gpio_reset_pin(STATE_MANAGER_GPIO_PIN_BLUE);
    gpio_reset_pin(STATE_MANAGER_GPIO_PIN_RED);

    gpio_config(&io_config_state_manager_blue);
    gpio_config(&io_config_state_manager_red);

    gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, HIGH);
    gpio_set_level(STATE_MANAGER_GPIO_PIN_RED, HIGH);

    state_manager_inited = true;
}
