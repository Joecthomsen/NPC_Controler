#include <stdio.h>
#include "Http_handler.h"
#include "esp_http_client.h"
#include "Nvs_handler.h"
#include "esp_log.h"

#define JSON_BUFFER_SIZE 2048 // Adjust this based on your expected JSON size

// esp_err_t send_post_request(const char *url, const char *post_data);
void create_json_object(const char **keys, const char **values, int num_pairs, char *json_buffer);
esp_err_t client_event_post_handler(esp_http_client_event_t *evt);

char *get_access_token(void)
{
    char *access_token = malloc(sizeof(char) * 512);
    char refresh_token[512];
    nvs_get_string("authentication", "refresh_token", refresh_token);

    const char *key[] = {"refreshToken"};
    const char *value[] = {refresh_token};
    char json_buffer[1024] = {0};
    create_json_object(key, value, 1, json_buffer);

    ESP_LOGI("HTTP_HANDLER", "json_buffer: %s", json_buffer);

    esp_http_client_config_t config = {
        .url = "http://65.108.92.248/auth/refreshToken_device", //"http://httpbin.org/post",
        .method = HTTP_METHOD_POST,
        //.buffer_size = strlen(json_buffer) + 1, // Add 1 for null terminator
        .cert_pem = NULL,
        .auth_type = HTTP_AUTH_TYPE_NONE,
        .event_handler = client_event_post_handler};

    esp_http_client_handle_t client = esp_http_client_init(&config); // client_event_post_handler;

    esp_http_client_set_post_field(client, json_buffer, strlen(json_buffer));
    esp_http_client_set_header(client, "Content-Type", "application/json");
    // esp_http_client_set_header(client, "Accept", "application/json");
    esp_err_t err;

    int status_code;
    do
    {
        err = esp_http_client_perform(client);
        status_code = esp_http_client_get_status_code(client);
    } while (status_code == -1);

    // Check status code
    if (status_code != 201)
    {
        ESP_LOGE("http_client", "Error %d", status_code);
        return NULL;
    }

    // Read response data
    // char *access_token = malloc(512);
    int content_length = esp_http_client_get_content_length(client);
    int data_read = esp_http_client_read(client, access_token, content_length);

    if (err == ESP_OK)
    {
        printf("HTTP POST Status = %d, content_length = %lld\n",
               esp_http_client_get_status_code(client),
               esp_http_client_get_content_length(client));
    }
    else
    {
        printf("Error perform http request %s", esp_err_to_name(err));
        return 2;
    }

    return access_token;
}

// Helper function to append key-value pair to JSON string
void append_json_pair(char *json, const char *key, const char *value)
{
    strcat(json, "\"");
    strcat(json, key);
    strcat(json, "\":\"");
    strcat(json, value);
    strcat(json, "\",");
}

// Function to create a JSON object from key-value pairs
void create_json_object(const char **keys, const char **values, int num_pairs, char *json_buffer)
{
    strcpy(json_buffer, "{");

    for (int i = 0; i < num_pairs; i++)
    {
        append_json_pair(json_buffer, keys[i], values[i]);
    }

    // Replace the last comma with a closing bracket
    json_buffer[strlen(json_buffer) - 1] = '}';
}

esp_err_t client_event_post_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA, len: %d data: %s\n", evt->data_len, (char *)evt->data);
        break;

    default:
        break;
    }
    return ESP_OK;
}

// Function to send HTTP POST request with serialized JSON string
Http_status post_json_data(const char **keys, const char **values, int num_pairs)
{
    char json_buffer[JSON_BUFFER_SIZE] = {0};
    // Create the JSON object
    create_json_object(keys, values, num_pairs, json_buffer);

    // Now you can use json_buffer in your HTTP POST request
    printf("JSON string: %s\n", json_buffer);

    // Replace the following lines with your HTTP POST request logic using esp_http_client
    // Example:
    esp_http_client_config_t config = {
        .url = "http://65.108.92.248/controleGear/new_data_instance", //"http://httpbin.org/post",
        .method = HTTP_METHOD_POST,
        //.buffer_size = strlen(json_buffer) + 1, // Add 1 for null terminator
        .cert_pem = NULL,
        .event_handler = client_event_post_handler};

    esp_http_client_handle_t client = esp_http_client_init(&config); // client_event_post_handler;

    esp_http_client_set_post_field(client, json_buffer, strlen(json_buffer));
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        printf("HTTP POST Status = %d, content_length = %lld\n",
               esp_http_client_get_status_code(client),
               esp_http_client_get_content_length(client));
    }
    else
    {
        printf("Error perform http request %s", esp_err_to_name(err));
        return 2;
    }

    // esp_http_client_handle_t client = esp_http_client_init(&config);
    // esp_err_t err = esp_http_client_perform(client);
    // esp_http_client_cleanup(client);

    return HTTP_OK;
}

Http_status post_controle_gear_data(const Controle_gear_values_t *controle_gear)
{
    char *access_token = get_access_token();
    // Define keys and values arrays
    const char *keys[] = {
        "manufacturer_id",
        "operating_time",
        "start_counter",
        "external_supply_voltage",
        "external_supply_voltage_frequency",
        "power_factor",
        "overall_faliure_condition",
        "external_supply_undervoltage",
        "external_supply_undervoltage_counter",
        "external_supply_overvoltage",
        "external_supply_overvoltage_counter",
        "output_power_limitation",
        "output_power_limitation_counter",
        "thermal_derating",
        "thermal_derating_counter",
        "thermal_shutdown",
        "thermal_shutdown_counter",
        "temperature",
        "output_current_percent",
        "light_source_start_counter_resettable",
        "light_source_start_counter",
        "light_source_on_time_resettable",
        "light_source_on_time",
        "light_source_voltage",
        "light_source_current",
        "light_source_overall_faliure_condition",
        "light_source_overall_faliure_condition_counter",
        "light_source_short_circuit",
        "light_source_short_circuit_counter",
        "light_source_open_circuit",
        "light_source_open_circuit_counter",
        "light_source_thermal_derating",
        "light_source_thermal_derating_counter",
        "light_source_thermal_shutdown",
        "light_source_thermal_shutdown_counter",
        "light_source_temperature",
        "rated_median_usefull_life_of_luminare",
        "internal_controle_gear_reference_temperature",
        "rated_median_usefull_light_source_starts"};

    // Allocate memory for the values array
    const char **values = (const char **)malloc(sizeof(const char *) * (sizeof(keys) / sizeof(keys[0])));
    if (values == NULL)
    {
        printf("Error: Failed to allocate memory for values array\n");
        return 0; // Or any appropriate error status
    }

    // Convert structure fields to strings and store in values array
    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
    {
        char *value_buffer = (char *)malloc(20 * sizeof(char)); // Adjust the buffer size as needed
        if (value_buffer == NULL)
        {
            printf("Error: Failed to allocate memory for value %zu\n", i);
            // Free previously allocated memory
            for (size_t j = 0; j < i; j++)
            {
                free((void *)values[j]);
            }
            free(values);
            return 0; // Or any appropriate error status
        }

        // Convert structure field to string
        switch (i)
        {
        case 0:
            sprintf(value_buffer, "%llu", controle_gear->manufacturer_id);
            break;
        case 1:
            sprintf(value_buffer, "%lu", controle_gear->operating_time);
            break;
        case 2:
            sprintf(value_buffer, "%lu", controle_gear->start_counter);
            break;
        case 3:
            sprintf(value_buffer, "%u", controle_gear->external_supply_voltage);
            break;
        case 4:
            sprintf(value_buffer, "%u", controle_gear->external_supply_voltage_frequency);
            break;
        case 5:
            sprintf(value_buffer, "%u", controle_gear->power_factor);
            break;
        case 6:
            sprintf(value_buffer, "%u", controle_gear->overall_faliure_condition);
            break;
        case 7:
            sprintf(value_buffer, "%u", controle_gear->external_supply_undervoltage);
            break;
        case 8:
            sprintf(value_buffer, "%u", controle_gear->external_supply_undervoltage_counter);
            break;
        case 9:
            sprintf(value_buffer, "%u", controle_gear->external_supply_overvoltage);
            break;
        case 10:
            sprintf(value_buffer, "%u", controle_gear->external_supply_overvoltage_counter);
            break;
        case 11:
            sprintf(value_buffer, "%u", controle_gear->output_power_limitation);
            break;
        case 12:
            sprintf(value_buffer, "%u", controle_gear->output_power_limitation_counter);
            break;
        case 13:
            sprintf(value_buffer, "%u", controle_gear->thermal_derating);
            break;
        case 14:
            sprintf(value_buffer, "%u", controle_gear->thermal_derating_counter);
            break;
        case 15:
            sprintf(value_buffer, "%u", controle_gear->thermal_shutdown);
            break;
        case 16:
            sprintf(value_buffer, "%u", controle_gear->thermal_shutdown_counter);
            break;
        case 17:
            sprintf(value_buffer, "%u", controle_gear->temperature);
            break;
        case 18:
            sprintf(value_buffer, "%u", controle_gear->output_current_percent);
            break;
        case 19:
            sprintf(value_buffer, "%lu", controle_gear->light_source_start_counter_resettable);
            break;
        case 20:
            sprintf(value_buffer, "%lu", controle_gear->light_source_start_counter);
            break;
        case 21:
            sprintf(value_buffer, "%lu", controle_gear->light_source_on_time_resettable);
            break;
        case 22:
            sprintf(value_buffer, "%lu", controle_gear->light_source_on_time);
            break;
        case 23:
            sprintf(value_buffer, "%u", controle_gear->light_source_voltage);
            break;
        case 24:
            sprintf(value_buffer, "%u", controle_gear->light_source_current);
            break;
        case 25:
            sprintf(value_buffer, "%u", controle_gear->light_source_overall_faliure_condition);
            break;
        case 26:
            sprintf(value_buffer, "%u", controle_gear->light_source_overall_faliure_condition_counter);
            break;
        case 27:
            sprintf(value_buffer, "%u", controle_gear->light_source_short_circuit);
            break;
        case 28:
            sprintf(value_buffer, "%u", controle_gear->light_source_short_circuit_counter);
            break;
        case 29:
            sprintf(value_buffer, "%u", controle_gear->light_source_open_circuit);
            break;
        case 30:
            sprintf(value_buffer, "%u", controle_gear->light_source_open_circuit_counter);
            break;
        case 31:
            sprintf(value_buffer, "%u", controle_gear->light_source_thermal_derating);
            break;
        case 32:
            sprintf(value_buffer, "%u", controle_gear->light_source_thermal_derating_counter);
            break;
        case 33:
            sprintf(value_buffer, "%u", controle_gear->light_source_thermal_shutdown);
            break;
        case 34:
            sprintf(value_buffer, "%u", controle_gear->light_source_thermal_shutdown_counter);
            break;
        case 35:
            sprintf(value_buffer, "%u", controle_gear->light_source_temperature);
            break;
        case 36:
            sprintf(value_buffer, "%u", controle_gear->rated_median_usefull_life_of_luminare);
            break;
        case 37:
            sprintf(value_buffer, "%u", controle_gear->internal_controle_gear_reference_temperature);
            break;
        case 38:
            sprintf(value_buffer, "%u", controle_gear->rated_median_usefull_light_source_starts);
            break;
        }

        // Assign value buffer to values array
        values[i] = value_buffer;
    }

    // Call post_json_data function with keys, values, and number of pairs
    Http_status status = post_json_data(keys, values, sizeof(keys) / sizeof(keys[0]));

    // Free dynamically allocated memory
    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
    {
        free((void *)values[i]);
    }
    free(values);

    return status;
}
