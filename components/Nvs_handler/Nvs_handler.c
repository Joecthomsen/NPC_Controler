#include <stdio.h>
#include "Nvs_handler.h"
#include "esp_err.h"
#include <nvs_flash.h>
#include "nvs.h"
#include "esp_log.h"

/**
 * @brief Initialize the NVS flash storage partition
 *
 * This initializes the default NVS partition defined in the ESP-IDF.
 * It will first try to mount the partition. If that fails due to
 * corrupt flash or first boot, it will erase the partition and recreate it.
 */
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

/**
 * @brief Write a uint8_t value to NVS storage
 *
 * This opens an NVS handle, writes the value under the given key,
 * commits and closes the handle.
 *
 * @param key Null-terminated string key to store value under
 * @param value The uint8_t value to store
 */
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

/**
 * @brief Read a uint8_t value from NVS storage
 *
 * This opens an NVS handle, reads the value under the given key,
 * and closes the handle.
 *
 * @param key Null-terminated string key to read value from
 * @return uint8_t The value stored under the key
 */
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