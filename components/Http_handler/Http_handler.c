#include <stdio.h>
#include "Http_handler.h"
#include "esp_http_client.h"

// esp_http_client_config_t config = {
//     .url = "http://example.com/post",
//     .method = HTTP_METHOD_POST,
//     .event_handler = _http_event_handler,
// };

Http_status post_control_gear_status(Controle_gear *controle_gear)
{
    printf("Posting control gear status\n");
    return HTTP_OK;
}