#include <stdio.h>
#include "Http_handler.h"
#include "esp_http_client.h"

// esp_http_client_config_t config = {
//     .url = "http://example.com/post",
//     .method = HTTP_METHOD_POST,
//     .event_handler = _http_event_handler,
// };

esp_err_t send_post_request(const char *url, const char *post_data);

Http_status post_control_gear_status(Controle_gear_values_t *controle_gear)
{
    return HTTP_OK;
}

// Event handler function to handle HTTP events
esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        printf("HTTP_EVENT_ERROR\n");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        printf("HTTP_EVENT_ON_CONNECTED\n");
        break;
    case HTTP_EVENT_HEADER_SENT:
        printf("HTTP_EVENT_HEADER_SENT\n");
        break;
    case HTTP_EVENT_ON_HEADER:
        printf("HTTP_EVENT_ON_HEADER, key=%s, value=%s\n", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA, len=%d\n", evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        printf("HTTP_EVENT_ON_FINISH\n");
        break;
    case HTTP_EVENT_DISCONNECTED:
        printf("HTTP_EVENT_DISCONNECTED\n");
        break;
    default:
        break;
    }
    return ESP_OK;
}

// Function to send HTTP POST request with a body
esp_err_t send_post_request(const char *url, const char *post_data)
{
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .event_handler = _http_event_handler,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL)
    {
        printf("Failed to initialize HTTP client\n");
        return ESP_FAIL;
    }

    // Set HTTP request body
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    // Perform HTTP request
    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK)
    {
        printf("HTTP POST request failed: %s\n", esp_err_to_name(err));
    }

    // Clean up
    esp_http_client_cleanup(client);

    return err;
}
