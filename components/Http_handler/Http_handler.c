#include <stdio.h>
#include "Http_handler.h"
#include "esp_http_client.h"
#include "Nvs_handler.h"
#include "esp_log.h"
#include "constants.h"
#include "State_manager.h"

#define JSON_BUFFER_SIZE 2048 // Adjust this based on your expected JSON size

char new_access_token[256];

bool response_received = false;
char control_gears[MAX_CONTROLE_GEARS][10];
size_t control_gears_count = 0;

uint8_t current_device_short_address = 0;

// esp_err_t send_post_request(const char *url, const char *post_data);
void create_json_object(const char **keys, const char **values, int num_pairs, char *json_buffer);
esp_err_t http_event_handler(esp_http_client_event_t *evt);
char *get_access_token(uint8_t short_address);

uint64_t global_elements[64]; // = NULL;
size_t global_num_elements = 0;

HTTP_REQUEST_TYPE HTTP_REQUEST = HTTP_NON_REQUEST;

char *get_access_token(uint8_t short_address)
{
    char *access_token = NULL; // Initialize access_token to NULL

    // ESP_LOGI("Short address", "%d", short_address);

    // Convert short_address to a string representation
    char short_addr_key[4]; // Assuming the maximum length of the string representation is 3 characters (plus 1 for the null terminator)
    snprintf(short_addr_key, sizeof(short_addr_key), "%hhu", short_address);

    // Your existing code to obtain the refresh token
    char refresh_token[512];
    bool error = nvs_get_string("authentication", "refresh_token", refresh_token);

    if (!error)
    {
        ESP_LOGI("HTTP_HANDLER", "Error getting refresh token from NVS");
        return NULL;
    }

    // Create JSON object containing the refresh token
    const char *key[] = {"refreshToken"};
    const char *value[] = {refresh_token};
    char json_buffer[1024] = {0};
    create_json_object(key, value, 1, json_buffer);

    // Log JSON buffer for debugging
    ESP_LOGI("HTTP_HANDLER", "json_buffer: %s", json_buffer);

    // Configure HTTP client for the request
    esp_http_client_config_t config = {
        .url = "http://95.217.159.233/auth/controller/refresh_token",
        .method = HTTP_METHOD_POST,
        .cert_pem = NULL,
        .auth_type = HTTP_AUTH_TYPE_NONE,
        .buffer_size = 1024,
        .event_handler = http_event_handler};

    // Initialize HTTP client
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Set JSON buffer as POST field
    esp_http_client_set_post_field(client, json_buffer, strlen(json_buffer));
    esp_http_client_set_header(client, "Content-Type", "application/json");

    // Perform HTTP POST request
    esp_err_t err = ESP_FAIL;
    HTTP_REQUEST = HTTP_ACCESS_TOKEN_REQUEST;
    err = esp_http_client_perform(client);

    // Cleanup HTTP client
    esp_http_client_cleanup(client);

    return access_token; // Return access token (may be NULL if request failed)
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

// Function to parse JSON response and extract controleGears array
uint64_t *parseJSONResponse(const char *response, int *num_elements)
{
    // Find the start position of the "controleGears" array
    const char *start_pos = strstr(response, "\"controleGears\":[");
    if (start_pos == NULL)
    {
        // "controleGears" array not found
        *num_elements = 0;
        return NULL;
    }

    start_pos += strlen("\"controleGears\":[");

    // Allocate memory for the array of uint64_t values
    uint64_t *elements = (uint64_t *)malloc(sizeof(uint64_t) * 64);
    if (elements == NULL)
    {
        // Memory allocation failed
        *num_elements = 0;
        return NULL;
    }

    // Extract uint64_t values from the "controleGears" array
    int index = 0;
    const char *ptr = start_pos;
    while (*ptr != ']' && index < 64)
    {
        // Skip leading whitespaces
        while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r')
            ptr++;

        // Expecting a double quote for the start of a value
        if (*ptr == '"')
        {
            ptr++; // Move past the opening double quote

            // Find the end of the uint64_t value
            const char *value_start = ptr;
            while (*ptr != '"' && *ptr != '\0')
                ptr++;

            // Check if we found the end of the value
            if (*ptr == '"')
            {
                // Extract the value string
                char value_str[32]; // Assuming no uint64 will exceed 32 characters
                int value_length = ptr - value_start;
                if (value_length < sizeof(value_str))
                {
                    strncpy(value_str, value_start, value_length);
                    value_str[value_length] = '\0';
                    // Convert the string to uint64_t and store it in the array
                    elements[index++] = strtoull(value_str, NULL, 10);
                }
            }
        }

        // Move to the next character
        ptr++;
    }

    *num_elements = index;
    return elements;
}

esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:

        char data[512];
        memcpy(data, evt->data, evt->data_len);
        data[evt->data_len] = '\0';
        ESP_LOGI("HTTP_EVENT_ON_DATA", " len: %d data: %s\n", evt->data_len, data);

        switch (HTTP_REQUEST)
        {
        case HTTP_NON_REQUEST:
            break;

        case HTTP_REFRESH_TOKEN_REQUEST:
            // Search for "accessToken" field manually
            const char *data = (char *)evt->data;
            const char *token_start = strstr(data, "\"accessToken\":\"");
            if (token_start != NULL)
            {
                token_start += strlen("\"accessToken\":\"");
                const char *token_end = strchr(token_start, '"');
                if (token_end != NULL)
                {
                    int token_len = token_end - token_start;
                    char access_token[token_len + 1];
                    strncpy(access_token, token_start, token_len);
                    access_token[token_len] = '\0';
                    printf("Access Token: %s\n", access_token);
                    strcpy(new_access_token, access_token);
                    bool result = nvs_set_string("authentication", "access_token", access_token);
                    if (!result)
                    {
                        ESP_LOGE("HTTP_HANDLER", "Error re-setting access token in NVS");
                    }
                    // Do whatever you want with the access token here
                }
                else
                {
                    printf("Access Token not found or invalid format.\n");
                }
            }

            // Search for "refreshToken" field manually
            const char *data2 = (char *)evt->data;
            const char *token_start2 = strstr(data2, "\"refreshToken\":\"");
            if (token_start2 != NULL)
            {
                token_start2 += strlen("\"refreshToken\":\"");
                const char *token_end2 = strchr(token_start2, '"');
                if (token_end2 != NULL)
                {
                    int token_len2 = token_end2 - token_start2;
                    char refresh_token[token_len2 + 1];
                    strncpy(refresh_token, token_start2, token_len2);
                    refresh_token[token_len2] = '\0';
                    printf("Refresh Token: %s\n", refresh_token);
                    char key[4];
                    snprintf(key, sizeof(key), "%hhu", current_device_short_address);
                    bool result = nvs_set_string("authentication", "refresh_token", refresh_token);
                    if (!result)
                    {
                        ESP_LOGE("HTTP_HANDLER", "Error re-setting refresh token in NVS");
                        nvs_delete_key_value_pair("authentication", "refresh_token");
                        nvs_delete_key_value_pair("authentication", "access_token");
                        set_state(NOT_AUTHENTICATED_STATE);
                    }
                    // Do whatever you want with the refresh token here
                }
                else
                {
                    nvs_delete_key_value_pair("authentication", "refresh_token");
                    nvs_delete_key_value_pair("authentication", "access_token");
                    set_state(NOT_AUTHENTICATED_STATE);
                    ESP_LOGE("HTTP_HANDLER", "Error re-setting refresh token in NVS");
                }
            }

            else
            {
                nvs_delete_key_value_pair("authentication", "refresh_token");
                nvs_delete_key_value_pair("authentication", "access_token");
                set_state(NOT_AUTHENTICATED_STATE);
                ESP_LOGE("HTTP_HANDLER", "Error re-setting refresh token in NVS");
            }
            HTTP_REQUEST = HTTP_NON_REQUEST;
            break;

        case HTTP_GET_CONTROL_GEARS_REQUEST:
            const char *response = (char *)evt->data;
            printf("Data: %s\n", response);

            int num_elements;
            uint64_t *elements = parseJSONResponse(response, &num_elements);
            global_num_elements = num_elements;

            printf("Number of elements: %d\n", num_elements);
            for (int i = 0; i < num_elements; i++)
            {
                printf("Element %d: %llu\n", i, elements[i]);
                global_elements[i] = elements[i];
            }

            // Allocate memory for global_elements
            // global_elements = (uint64_t *)malloc(num_elements * sizeof(uint64_t));
            // if (global_elements != NULL)
            // {
            //     // Copy elements to global_elements
            //     memcpy(global_elements, elements, num_elements * sizeof(uint64_t));
            //     global_num_elements = num_elements;
            // }
            // else
            // {
            //     // Handle memory allocation failure
            //     global_num_elements = 0;
            // }

            // Free memory allocated for elements
            free(elements);

            break;

        default:
            break;
        }

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
        .url = "http://95.217.159.233/controller/new_data_instance", //"http://httpbin.org/post",
        .method = HTTP_METHOD_POST,
        //.buffer_size = strlen(json_buffer) + 1, // Add 1 for null terminator
        .cert_pem = NULL,
        .event_handler = http_event_handler};

    esp_http_client_handle_t client = esp_http_client_init(&config); // http_event_handler;

    esp_http_client_set_post_field(client, json_buffer, strlen(json_buffer));
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "token", new_access_token);
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
    current_device_short_address = controle_gear->short_address;
    // char *access_token = get_access_token(controle_gear->short_address); // Will put the access token in new_access_token when http event is triggered
    refresh_token();
    ESP_LOGI("HTTP_CLIENT", "Access token fetched: %s", new_access_token);

    // if (access_token != NULL)
    // {
    //     ESP_LOGI("HTTP_CLIENT", "Access token fetched: %s", access_token);
    //     free(access_token); // Free the memory allocated for access_token
    // }
    // else
    // {
    //     ESP_LOGE("HTTP_CLIENT", "Failed to fetch access token");
    // }
    // Define keys and values arrays
    const char *keys[] = {
        "manufactoringID",
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

    // char email[100];
    // nvs_get_string("authentication", "email", email);
    // // Copy the email to a dynamically allocated buffer
    // char *email_value_buffer = (char *)malloc((strlen(email) + 1) * sizeof(char)); // Allocate memory for email
    // if (email_value_buffer == NULL)
    // {
    //     printf("Error: Failed to allocate memory for email value buffer\n");
    //     free(values); // Free previously allocated memory
    //     return 0;     // Or any appropriate error status
    // }
    // strcpy(email_value_buffer, email); // Copy email to the buffer

    // Assign the email value buffer to the values array
    // values[sizeof(keys) / sizeof(keys[0]) - 1] = email_value_buffer;
    // ESP_LOGI("HTTP_CLIENT", "Email fetched: %s", email);

    // nvs_get_email(email);

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
    // return 0;
}

// esp_err_t http_event_handler_fetch_controle_gear(esp_http_client_event_t *evt)
// {
//     char data[512];
//     memcpy(data, evt->data, evt->data_len);
//     data[evt->data_len] = '\0';
//     ESP_LOGI("HTTP_EVENT_ON_DATA (Fetch Controle Gears):", " len: %d data: %s\n", evt->data_len, data);
//     char *reponse = (char *)evt->data;
//     printf("Data: %s\n", reponse);
//     // if (data != NULL)
//     // {
//     //     parseJSONResponse(data);
//     // }
//     // else
//     // {
//     //     ESP_LOGE("HTTP_HANDLER", "Error: No data received from server");
//     // }
//     HTTP_REQUEST = HTTP_NON_REQUEST;
//     response_received = true;
//     return ESP_OK;
// }

uint64_t *getControleGearsRemote(size_t *numGears)
{
    // *manufactoring_id_fetched_i = manufactoring_id_fetched;
    // numGears_i = numGears;
    //*numGears = sizeof(controleGears) / sizeof(controleGears[0]);

    char url[100];
    char access_token[512] = {0};
    nvs_get_string("authentication", "access_token", access_token);
    snprintf(url, sizeof(url), "http://95.217.159.233/controller/controle_gears/%s", popID);

    // Configure HTTP client for the request
    esp_http_client_config_t config = {
        .url = url, //"http://95.217.159.233/controller/controle_gears/abcd1234",
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        .auth_type = HTTP_AUTH_TYPE_NONE,
        .buffer_size = 1024,
        .event_handler = http_event_handler};
    // Initialize HTTP client
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "token", access_token);

    // Perform HTTP POST request
    esp_err_t err = ESP_FAIL;
    HTTP_REQUEST = HTTP_GET_CONTROL_GEARS_REQUEST;
    err = esp_http_client_perform(client);

    *numGears = global_num_elements;

    // // Cleanup HTTP client
    // while (!response_received)
    //     ;
    // response_received = false;
    esp_http_client_cleanup(client);
    return &global_elements;
}

void refresh_token()
{
    // Reinitialize tokens
    // char access_token[512] = {0};
    char refresh_token[512] = {0};
    nvs_get_string("authentication", "refresh_token", refresh_token);

    // Configure HTTP client for the request
    esp_http_client_config_t config = {
        .url = "http://95.217.159.233/auth/controller/refresh_token",
        .method = HTTP_METHOD_POST,
        .cert_pem = NULL,
        .auth_type = HTTP_AUTH_TYPE_NONE,
        .buffer_size = 1024,
        .event_handler = http_event_handler};
    // Concatenate the popID to the URL
    // Initialize HTTP client

    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Create JSON object containing the refresh token
    const char *key[] = {"refreshToken"};
    const char *value[] = {refresh_token};
    char json_buffer[1024] = {0};
    create_json_object(key, value, 1, json_buffer);

    esp_http_client_set_post_field(client, json_buffer, strlen(json_buffer));
    esp_http_client_set_header(client, "Content-Type", "application/json");

    HTTP_REQUEST = HTTP_REFRESH_TOKEN_REQUEST;
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
    }
    // Cleanup HTTP client
    esp_http_client_cleanup(client);
}

void add_controle_gear_to_db(uint64_t manufactoring_id)
{

    char access_token[512] = {0};
    nvs_get_string("authentication", "access_token", access_token);
    esp_http_client_config_t config = {
        .url = "http://95.217.159.233/controller/add_control_gear",
        .method = HTTP_METHOD_POST,
        .cert_pem = NULL,
        .auth_type = HTTP_AUTH_TYPE_NONE,
        .buffer_size = 1024,
        .event_handler = http_event_handler};

    esp_http_client_handle_t client = esp_http_client_init(&config);

    char manufactoring_id_str[21]; // 21 characters are enough to hold the maximum uint64_t value
    snprintf(manufactoring_id_str, sizeof(manufactoring_id_str), "%llu", (unsigned long long)manufactoring_id);

    // Create JSON object containing the refresh token
    const char *key[] = {"manufactoringID", "popID"};
    const char *value[] = {manufactoring_id_str, popID};
    char json_buffer[1024] = {0};
    create_json_object(key, value, 2, json_buffer);

    esp_http_client_set_post_field(client, json_buffer, strlen(json_buffer));
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "token", access_token);

    HTTP_REQUEST = HTTP_NON_REQUEST; // TODO Just temporary. Implement a better solution

    esp_err_t err = esp_http_client_perform(client);
}