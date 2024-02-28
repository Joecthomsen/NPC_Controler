#include <stdio.h>
#include "Nvs_handler.h"
#include "esp_err.h"
#include <nvs_flash.h>
#include "nvs.h"
#include "esp_log.h"
#include "DALI_memory_bank_handler.h"
#include "constants.h"
#include "string.h"

#define TAG "NVS_HANDLER"

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

    // for (int i = 0; i < 10; i++)
    // {
    //     char test[10];                 // Define a char array to store the key
    //     sprintf(test, "%d", i);        // Convert integer i to a string
    //     nvs_write_uint64(test, i * 4); // Use the string representation of i as the key
    // }
    // for (int i = 0; i < 10; i++)
    // {
    //     char test[10];
    //     sprintf(test, "%d", i);
    //     uint64_t value = nvs_read_uint64(test);

    //     printf("get test value %d: %02llx\n", i, value);
    // }
}

bool nvs_set_string(char *namespace, const char *key, char *value)
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open(namespace, NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_synchronize", "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return false;
    }
    // Write token to NVS
    err = nvs_set_str(my_handle, key, value);
    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_set_token", "Error (%s) storing token in NVS", esp_err_to_name(err));
        nvs_close(my_handle);
        return false;
    }

    // Commit changes to NVS
    err = nvs_commit(my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_set_token", "Error (%s) committing changes to NVS", esp_err_to_name(err));
        nvs_close(my_handle);
        return false;
    }

    // Close NVS handle
    nvs_close(my_handle);

    return true;
}

bool nvs_get_string(char *namespace, const char *key, char *return_value)
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open(namespace, NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_synchronize", "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return false;
    }

    size_t len = 512;               // Assuming maximum token length
    char *buffer = malloc(len + 1); // +1 for null terminator

    if (buffer == NULL)
    {
        ESP_LOGE("nvs_synchronize", "Failed to allocate memory for token buffer!\n");
        nvs_close(my_handle);
        return false;
    }

    // Write token to NVS
    err = nvs_get_str(my_handle, key, return_value, &len);
    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_set_token", "Error (%s) storing token in NVS", esp_err_to_name(err));
        nvs_close(my_handle);
        return false;
    }
    return true;
}

bool nvs_synchronize(uint8_t *short_addresses, uint8_t short_addresses_count)
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("manu_id", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_synchronize", "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return false;
    }

    // Check if there are any manufacturing IDs stored in NVS
    size_t count = 0;
    err = nvs_get_used_entry_count(my_handle, &count);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error getting count of manufacturing IDs in NVS: %s", esp_err_to_name(err));
        nvs_close(my_handle);
        return false;
    }

    // If no IDs are stored, synchronize by storing the ones from the bus
    if (count == 0)
    {
        ESP_LOGI(TAG, "No manufacturing IDs found in NVS. Synchronizing with IDs from the bus.");
        for (int i = 0; i < short_addresses_count; i++)
        {
            // uint64_t manu_id = read_ma(short_addresses[i]);
            char key[10]; // Assuming maximum 10 characters for numeric key representation
            sprintf(key, "%d", i);
            uint64_t manu_id = read_manufactor_id(short_addresses[i]);
            err = nvs_set_u64(my_handle, key, manu_id);
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "Error storing manufacturing ID at index %d: %s", i, esp_err_to_name(err));
                nvs_close(my_handle);
                return false;
            }
        }
        // Commit changes
        nvs_commit(my_handle);
        return true;
    }

    else if (count != short_addresses_count)
    {
        ESP_LOGI(TAG, "Manufacturing IDs count in NVS does not match the count of IDs on the bus. Synchronizing needed.");
        nvs_close(my_handle);
        return false;
    }

    // Proceed with comparison if needed
    else
    {
        // Flag to track if synchronization is needed
        bool sync_needed = false;
        char key[10]; // Assuming maximum 10 characters for numeric key representation

        for (int i = 0; i < short_addresses_count; i++)
        {
            sprintf(key, "%d", i);
            uint64_t value;
            err = nvs_get_u64(my_handle, key, &value);
            if (err == ESP_ERR_NVS_NOT_FOUND)
            {
                // Value not found, needs synchronization
                sync_needed = true;
                ESP_LOGI(TAG, "Manufacturing ID at index %d not found in NVS. Synchronization needed.", i);
                // If you want to store this ID here, you can do so:
                // err = nvs_set_u64(my_handle, key, <your_value>);
            }
            else if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "Error reading manufacturing ID at index %d: %s", i, esp_err_to_name(err));
                // Handle error
            }
            else
            {

                // Value found, compare with bus
                for (size_t j = 0; j < short_addresses_count; j++)
                {
                    if (value == read_manufactor_id(short_addresses[j]))
                    {
                        ESP_LOGI(TAG, "Manufacturing ID at index %d matches ID on the bus.", i);
                        sync_needed = false;
                        break;
                    }
                }

                // IDs match, continue checking next ID
            }
        }
        nvs_close(my_handle);
        if (sync_needed)
        {
            // If synchronization is needed, you can perform additional actions here
            // such as storing new IDs to NVS
        }
    }

    // Close NVS handle
    nvs_close(my_handle);

    // Return true if all checks pass

    return true;
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

void nvs_write_uint64(const char *key, uint64_t value)
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_write_uint64", "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        err = nvs_set_u64(my_handle, key, value);
        if (err != ESP_OK)
        {
            ESP_LOGE("nvs_write_uint64", "Error (%s) writing!\n", esp_err_to_name(err));
        }
        err = nvs_commit(my_handle);
        if (err != ESP_OK)
        {
            ESP_LOGE("nvs_write_uint64", "Error (%s) commit!\n", esp_err_to_name(err));
        }
        nvs_close(my_handle);
    }
}

uint64_t nvs_read_uint64(const char *key)
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_read_uint64", "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    uint64_t value;
    err = nvs_get_u64(my_handle, key, &value);
    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_read_uint64", "Error (%s) reading!\n", esp_err_to_name(err));
    }
    nvs_close(my_handle);
    return value;
}

void nvs_delete_key_value_pair(const char *key)
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_delete_key_value_pair", "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        err = nvs_erase_key(my_handle, key);
        if (err != ESP_OK)
        {
            ESP_LOGE("nvs_delete_key_value_pair", "Error (%s) erasing!\n", esp_err_to_name(err));
        }
        err = nvs_commit(my_handle);
        if (err != ESP_OK)
        {
            ESP_LOGE("nvs_delete_value", "Error (%s) commit!\n", esp_err_to_name(err));
        }
        nvs_close(my_handle);
    }
}

void nvs_write_manufactoring_id(const char *key, uint64_t value)
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("manu_id", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_write_manufactoring_id", "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        err = nvs_set_u64(my_handle, key, value);
        if (err != ESP_OK)
        {
            ESP_LOGE("nvs_write_manufactoring_id", "Error (%s) writing!\n", esp_err_to_name(err));
        }
        err = nvs_commit(my_handle);
        if (err != ESP_OK)
        {
            ESP_LOGE("nvs_write_manufactoring_id", "Error (%s) commit!\n", esp_err_to_name(err));
        }
        nvs_close(my_handle);
    }
}

uint64_t nvs_read_manufactoring_id(const char *key)
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("manu_id", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_read_manufactoring_id", "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    uint64_t value;
    err = nvs_get_u64(my_handle, key, &value);
    if (err != ESP_OK)
    {
        ESP_LOGE("nvs_read_manufactoring_id", "Error (%s) reading!\n", esp_err_to_name(err));
    }
    nvs_close(my_handle);
    return value;
}

char *nvs_read_all_manufactoring_ids()
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("manu_id", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return NULL;
    }

    // Create a buffer for storing CSV data
    size_t csv_buffer_size = 1024; // Initial buffer size, adjust as needed
    char *csv_buffer = malloc(csv_buffer_size);
    if (!csv_buffer)
    {
        ESP_LOGE(TAG, "Memory allocation failed!");
        nvs_close(my_handle);
        return NULL;
    }
    csv_buffer[0] = '\0'; // Ensure buffer is initially empty

    // Read all manufacturing IDs
    uint64_t value;
    char key[32];                                          // Adjust the size as needed
    for (int i = 0; i < short_addresses_on_bus_count; i++) // Adjust MAX_MANUFACTURING_IDS as needed
    {
        snprintf(key, sizeof(key), "%d", i);
        value = nvs_read_manufactoring_id(key);
        if (value != 0) // Assuming 0 is not a valid manufacturing ID
        {
            // Append the ID to the CSV buffer
            int bytes_written = snprintf(csv_buffer + strlen(csv_buffer), csv_buffer_size - strlen(csv_buffer), "%llu,", value);
            if (bytes_written < 0 || bytes_written >= csv_buffer_size - strlen(csv_buffer))
            {
                // Error handling for buffer overflow
                ESP_LOGE(TAG, "CSV buffer overflow!");
                free(csv_buffer);
                nvs_close(my_handle);
                return NULL;
            }
        }
    }

    // Close NVS handle
    nvs_close(my_handle);

    // Remove trailing comma
    if (strlen(csv_buffer) > 0)
    {
        csv_buffer[strlen(csv_buffer) - 1] = '\0';
    }

    return csv_buffer;
}

bool authenticated(void)
{
    bool returnValue = false;
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("authentication", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE("authenticated", "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        size_t token_len = 0;
        err = nvs_get_str(my_handle, "refresh_token", NULL, &token_len);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
        {
            ESP_LOGE("authenticated", "Error (%s) reading refresh token!\n", esp_err_to_name(err));
        }
        else
        {
            returnValue = (err == ESP_OK); // If refresh token exists, return true
        }
        nvs_close(my_handle);
    }
    return returnValue;
}