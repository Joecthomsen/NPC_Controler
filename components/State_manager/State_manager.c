#include <stdio.h>
#include "State_manager.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

int state_machine = NVS_INIT_STATE;
bool state_manager_inited = false;

void state_task(void *parameter)
{
    while (true)
    {
        switch (state_machine)
        {
        case NVS_INIT_STATE:
            gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, HIGH);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, LOW);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            break;

        case WIFI_PROVISIONING_STATE:
            gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, HIGH);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, LOW);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            break;

        case AWAIT_WIFI_PROVISIONING_STATE:
            gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, HIGH);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, LOW);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            break;

        case DALI_COMMUNICATION_INIT_STATE:
            gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, HIGH);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_YELLOW, HIGH);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_YELLOW, LOW);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            break;

        case DALI_COMMUNICATION_OK_STATE:
            gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, HIGH);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_GREEN, HIGH);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            break;

        case ANALYZE_DALI_BUS_STATE:
            gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, HIGH);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_YELLOW, HIGH);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_YELLOW, LOW);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            break;

        case TCP_SERVER_INIT_STATE:
            gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, HIGH);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_GREEN, HIGH);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            break;

        case MDNS_INIT_STATE:
            gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, HIGH);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_GREEN, HIGH);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            break;

        case DALI_BUS_NOT_COMMISIONED_STATE:
            gpio_set_level(STATE_MANAGER_GPIO_PIN_YELLOW, HIGH);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_RED, LOW);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_GREEN, LOW);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, HIGH);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            break;

        case DALI_BUS_CORRUPTED_STATE:
            gpio_set_level(STATE_MANAGER_GPIO_PIN_YELLOW, HIGH);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_RED, HIGH);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_GREEN, LOW);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, HIGH);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_YELLOW, LOW);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_RED, HIGH);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_GREEN, LOW);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, HIGH);
            break;

        case NO_RESPONSE_ON_DALI_BUS:
            gpio_set_level(STATE_MANAGER_GPIO_PIN_YELLOW, HIGH);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_RED, HIGH);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_GREEN, LOW);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, HIGH);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            break;

        case SYSTEM_RUNNING_STATE:
            gpio_set_level(STATE_MANAGER_GPIO_PIN_GREEN, HIGH);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, LOW);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_YELLOW, LOW);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_RED, LOW);
            vTaskDelay(50 / portTICK_PERIOD_MS);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_GREEN, LOW);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, LOW);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_YELLOW, LOW);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_RED, LOW);
            vTaskDelay(2950 / portTICK_PERIOD_MS);
            break;

        case NO_WIFI_STATE:
            gpio_set_level(STATE_MANAGER_GPIO_PIN_RED, HIGH);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, HIGH);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            break;

        case WIFI_CONNECTED_STATE:
            gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, HIGH);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_YELLOW, LOW);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_RED, LOW);
            gpio_set_level(STATE_MANAGER_GPIO_PIN_GREEN, LOW);
            vTaskDelay(500 / portTICK_PERIOD_MS);

        default:
            ESP_LOGE("State_manager", "Unknown state error");
            gpio_set_level(STATE_MANAGER_GPIO_PIN_RED, HIGH);
            vTaskDelay(500 / portTICK_PERIOD_MS);
            break;
        }
    }
}

void set_state(int new_state)
{
    if (state_manager_inited == false)
    {
        ESP_LOGE("State_manager", "State manager not initialized");
    }
    state_machine = new_state;
}

State_t get_state()
{
    return state_machine;
}

void init_state_manager()
{
    // Configure the GPIO pin GREEN
    gpio_config_t io_config_state_manager_green = {
        .mode = STATE_MANAGER_PIN_DIRECTION,
        .intr_type = STATE_MANAGER_INTERRUPT_TYPE,
        .pin_bit_mask = (1ULL << STATE_MANAGER_GPIO_PIN_GREEN),
        .pull_down_en = STATE_MANAGER_PIN_PULLDOWN,
        .pull_up_en = STATE_MANAGER_PIN_PULLUP,
    };
    // Configure the GPIO pin YELLOW
    gpio_config_t io_config_state_manager_yellow = {
        .mode = STATE_MANAGER_PIN_DIRECTION,
        .intr_type = STATE_MANAGER_INTERRUPT_TYPE,
        .pin_bit_mask = (1ULL << STATE_MANAGER_GPIO_PIN_YELLOW),
        .pull_down_en = STATE_MANAGER_PIN_PULLDOWN,
        .pull_up_en = STATE_MANAGER_PIN_PULLUP,
    };
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
    gpio_reset_pin(STATE_MANAGER_GPIO_PIN_YELLOW);
    gpio_reset_pin(STATE_MANAGER_GPIO_PIN_GREEN);

    gpio_config(&io_config_state_manager_blue);
    gpio_config(&io_config_state_manager_red);
    gpio_config(&io_config_state_manager_yellow);
    gpio_config(&io_config_state_manager_green);

    gpio_set_level(STATE_MANAGER_GPIO_PIN_BLUE, LOW);
    gpio_set_level(STATE_MANAGER_GPIO_PIN_RED, LOW);
    gpio_set_level(STATE_MANAGER_GPIO_PIN_YELLOW, LOW);
    gpio_set_level(STATE_MANAGER_GPIO_PIN_GREEN, LOW);

    state_manager_inited = true;
}
