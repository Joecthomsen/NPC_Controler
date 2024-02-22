// #include <stdio.h>
// #include "API.h"
// #include "DALI_addressing.h"
// #include "State_manager.h"

// char response[RESPONSE_BUFFER_SIZE] = "{\"status\":\"Error fetching state\"}";

// char *get_all_controle_gear_as_json_string(void)
// {
//     return NULL;
// }
// void store_controle_gear(Controle_gear_t controle_gear)
// {
// }

// void test_me()
// {
//     return;
//     // ESP_LOGI("API", "Testing me");
// }

// char *get_controler_state(void)
// {
//     State_t current_state = get_state();
//     switch (current_state)
//     {
//     case NVS_INIT_STATE:
//         sprintf(response, "{\"status\":\"NVS initialization state\"}");
//         break;
//     case AWAIT_WIFI_PROVISIONING_STATE:
//         sprintf(response, "{\"status\":\"Awaiting WiFi provisioning\"}");
//         break;
//     case WIFI_PROVISIONING_STATE:
//         sprintf(response, "{\"status\":\"WiFi provisioning state\"}");
//         break;
//     case WIFI_CONNECTED_STATE:
//         sprintf(response, "{\"status\":\"WiFi connected state\"}");
//         break;
//     case DALI_COMMUNICATION_INIT_STATE:
//         sprintf(response, "{\"status\":\"DALI communication initialization state\"}");
//         break;
//     case DALI_COMMUNICATION_OK_STATE:
//         sprintf(response, "{\"status\":\"DALI communication OK state\"}");
//         break;
//     case ANALYZE_DALI_BUS_STATE:
//         sprintf(response, "{\"status\":\"Analyzing DALI bus state\"}");
//         break;
//     case TCP_SERVER_INIT_STATE:
//         sprintf(response, "{\"status\":\"TCP server initialization state\"}");
//         break;
//     case MDNS_INIT_STATE:
//         sprintf(response, "{\"status\":\"mDNS initialization state\"}");
//         break;
//     case SYSTEM_RUNNING_STATE:
//         sprintf(response, "{\"status\":\"System is running\"}");
//         break;
//     case NO_WIFI_STATE:
//         sprintf(response, "{\"status\":\"No WiFi connection\"}");
//         break;
//     case DALI_BUS_CORRUPTED_STATE:
//         sprintf(response, "{\"status\":\"DALI bus corrupted state\"}");
//         break;
//     case DALI_BUS_NOT_COMMISIONED_STATE:
//         sprintf(response, "{\"status\":\"DALI bus not commissioned state\"}");
//         break;
//     case NO_RESPONSE_ON_DALI_BUS:
//         sprintf(response, "{\"status\":\"No response on DALI bus\"}");
//         break;
//     default:
//         sprintf(response, "{\"status\":\"Unknown state\"}");
//         break;
//     }
//     return response;
// }
