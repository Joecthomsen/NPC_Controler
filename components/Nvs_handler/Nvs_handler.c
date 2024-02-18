#include <stdio.h>
#include "Nvs_handler.h"
#include "esp_err.h"
#include <nvs_flash.h>
#include "nvs.h"
#include "esp_log.h"

void init_nvs_handler()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        /* NVS partition was truncated
         * and needs to be erased */
        ESP_ERROR_CHECK(nvs_flash_erase());

        /* Retry nvs_flash_init */
        ESP_ERROR_CHECK(nvs_flash_init());
    }
}

void nvs_write_uint8(const char *key, uint8_t value)
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_write_uint8", "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        err = nvs_set_u8(my_handle, key, value);
        if (err != ESP_OK)
        {
            ESP_LOGE("nvs_write_uint8", "Error (%s) writing!\n", esp_err_to_name(err));
        }
        err = nvs_commit(my_handle);
        if (err != ESP_OK)
        {
            ESP_LOGE("nvs_write_uint8", "Error (%s) commit!\n", esp_err_to_name(err));
        }
        nvs_close(my_handle);
    }
}

uint8_t nvs_read_uint8(const char *key)
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_read_uint8", "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    uint8_t value;
    err = nvs_get_u8(my_handle, key, &value);
    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_read_uint8", "Error (%s) reading!\n", esp_err_to_name(err));
    }
    nvs_close(my_handle);
    return value;
}

// bool nvs_value_exists(const char *key)
// {
//     nvs_handle_t handle;
//     int32_t value;
//     esp_err_t err = nvs_get_i32(handle, key, &value);

//     if (err == ESP_OK)
//     {
//         return true;
//     }
//     else if (err == ESP_ERR_NVS_NOT_FOUND)
//     {
//         return false;
//     }
//     else
//     {
//         return false;
//     }
// }