#include <stdio.h>
#include "Tcp_server.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "State_manager.h"
#include "API_controler.h"
#include "constants.h"
#include "DALI_communication.h"
#include "Nvs_handler.h"
#include "constants.h"

#define TAG "TCP_SERVER"

char response[128];
char transmit[128];
int global_socket = -1;

typedef enum
{
    EVENT_NONE,
    EVENT_GET_STATUS,
    EVENT_COMMISION_DALI_BUS,
} tcp_event_type_t;

bool transmit_error_flag = false;
static tcp_event_type_t current_event = EVENT_NONE;

// TCP prototypes
void message_handler(char *rx_buffer, int len, int socket);

// API prototype - move in the future
char *get_controler_state(void);
void blink_lamp(uint64_t manuID);

void tcp_server_task(void *pvParameters)
{
    char rx_buffer[512];
    int addr_family;
    int ip_protocol;

    while (1)
    {
        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(LISTEN_PORT);
        ip_protocol = IPPROTO_IP;

        int listen_sock = socket(AF_INET, SOCK_STREAM, ip_protocol);
        if (listen_sock < 0)
        {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0)
        {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
            close(listen_sock);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        err = listen(listen_sock, 1);
        if (err != 0)
        {
            ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
            close(listen_sock);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        ESP_LOGI(TAG, "Socket listening");

        struct sockaddr_in source_addr;
        uint addr_len = sizeof(source_addr);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);

        ESP_LOGI(TAG, "Socket accepted");

        if (transmit_error_flag)
        {
            int msg_len = strlen(transmit);

            send(sock, transmit, msg_len, 0);
            transmit_error_flag = false;
        }

        global_socket = sock;
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            close(listen_sock);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        // Receive the data
        while (1)
        {
            int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            if (len < 0)
            {
                ESP_LOGE(TAG, "recv failed: errno %d", errno);
                break;
            }
            else if (len == 0)
            {
                ESP_LOGI(TAG, "Connection closed");
                break;
            }
            else
            {
                rx_buffer[len] = 0;
                ESP_LOGI(TAG, "Received %d bytes: %s", len, rx_buffer);
                // test_me();
                message_handler(rx_buffer, len, sock);
                // send(sock, rx_buffer, len, 0);
            }
        }

        if (sock != -1)
        {
            shutdown(sock, 0);
            close(sock);
            global_socket = -1;
        }
        close(listen_sock);
    }
    vTaskDelete(NULL);
}

void message_handler(char *rx_buffer, int len, int socket)
{
    // Trim leading and trailing whitespace
    char *end = rx_buffer + len - 1;
    while (end > rx_buffer && isspace((unsigned char)*end))
        end--;
    end[1] = '\0';

    char *start = rx_buffer;
    while (*start && isspace((unsigned char)*start))
        start++;

    ESP_LOGI(TAG, "Received message from messages handler: %s", rx_buffer);

    if (strcmp(rx_buffer, "GET_STATE") == 0)
    {
        ESP_LOGI(TAG, "Received GET_STATE message");
        char *current_state = get_controler_state();
        uint8_t current_state_len = strlen(current_state);
        send(socket, current_state, current_state_len, 0);
    }
    else if (strcmp(rx_buffer, "COMMISION_DALI_BUS") == 0)
    {
        ESP_LOGI(TAG, "Received COMMISION_DALI_BUS message");
        set_state(DALI_COMMISION_BUS_STATE);
        xEventGroupSetBits(tcpEventGroup, TCP_EVENT_BIT);
    }
    else if (strncmp(rx_buffer, "BLINK_LAMP", strlen("BLINK_LAMP")) == 0)
    {
        // Extract the short address argument
        char *argument = rx_buffer + strlen("BLINK_LAMP");
        uint64_t manuID = strtoull(argument, NULL, 10); // Convert the argument to a uint64_t

        // Valid short address, proceed with blinking the lamp
        ESP_LOGI(TAG, "Received BLINK_LAMP message for manifactoring ID: %llu", manuID);
        xEventGroupSetBits(tcpEventGroup, TCP_EVENT_BIT);
        blink_lamp(manuID);
    }
    else if (strcmp(rx_buffer, "STOP_BLINK") == 0)
    {
        ESP_LOGI(TAG, "Received STOP_BLINK message");
        State_t last_state = get_last_state();
        // while (last_state == BLINK_LAMP_STATE)
        // {
        //     last_state = get_last_state();
        // }

        set_state(last_state);
    }
    else if (strcmp(rx_buffer, "GET_MANUFACTORING_ID_ON_BUS") == 0)
    {
        char *manu_id = nvs_read_all_manufactoring_ids();
        uint16_t manu_id_len = strlen(manu_id);
        sprintf(response, "{\"status\":\"ManufactoringID\", \"ids\":\"%s\"}", manu_id);
        send(socket, response, strlen(response), 0);

        // send(socket, manu_id, manu_id_len, 0);
    }

    else if (strncmp(rx_buffer, "SET_REFRESH_TOKEN", strlen("SET_REFRESH_TOKEN")) == 0)
    {
        // Skip "SET_REFRESH_TOKEN" command to get the token
        char *token_start = rx_buffer + strlen("SET_REFRESH_TOKEN") + 1; // Plus one for the whitespace after the command

        // Find the end of the token
        char *token_end = strchr(token_start, '\0'); // Token ends at the null terminator

        if (token_end != NULL)
        {
            // Token found
            *token_end = '\0'; // Null-terminate the token
            char *refresh_token = token_start;

            // Calculate the length of the token
            uint16_t token_len = token_end - token_start;

            // Ensure the token length is within bounds
            if (token_len > 0)
            {
                // Call nvs_set_token function with the token and its length
                bool result = nvs_set_string("authentication", "refresh_token", refresh_token);
                ESP_LOGI(TAG, "Refresh Token: %s", refresh_token);

                // Check if setting the token was successful
                if (result)
                {
                    // If successful, send a success response
                    sprintf(response, "{\"status\":\"Refresh token set successfully\", \"popID\":\"%s\"}", popID);
                    send(socket, response, strlen(response), 0);
                }
                else
                {
                    // If unsuccessful, send an error response
                    sprintf(response, "{\"status\":\"Failed to set token\", \"popID\":\"%s\"}", popID);
                    send(socket, response, strlen(response), 0);
                }
            }
            else
            {
                // Handle case where the token is empty
                sprintf(response, "{\"status\":\"Empty token provided\", \"popID\":\"%s\"}", popID);
                send(socket, response, strlen(response), 0);
            }
        }
        else
        {
            // Handle case where no token is found
            sprintf(response, "{\"status\":\"No token provided\", \"popID\":\"%s\"}", popID);
            send(socket, response, strlen(response), 0);
        }
    }

    // else if (strncmp(rx_buffer, "SET_REFRESH_TOKEN", strlen("SET_REFRESH_TOKEN")) == 0)
    // {
    //     // Skip "SET_TOKEN" command to get the token
    //     char *token_start = rx_buffer + strlen("SET_REFRESH_TOKEN") + 1; // Plus one for the whitespace after the command

    //     // Find the end of the token
    //     char *token_end = strchr(token_start, '\0');

    //     // Calculate the length of the token
    //     uint16_t token_len = token_end - token_start;

    //     // Ensure the token length is within bounds
    //     if (token_len > 0)
    //     {
    //         // Call nvs_set_token function with the token and its length
    //         bool result = nvs_set_string("authentication", "token", token_start); // nvs_set_token(token_start, token_len);

    //         // Check if setting the token was successful
    //         if (result)
    //         {
    //             // If successful, send a success response
    //             const char *success_msg = "Token set successfully";
    //             send(socket, success_msg, strlen(success_msg), 0);
    //         }
    //         else
    //         {
    //             // If unsuccessful, send an error response
    //             const char *error_msg = "Failed to set token";
    //             send(socket, error_msg, strlen(error_msg), 0);
    //         }
    //     }
    //     else
    //     {
    //         // Handle case where no token is found
    //         // Perhaps send an appropriate response or log a warning
    //     }
    // }

    else if (strcmp(rx_buffer, "GET_REFRESH_TOKEN") == 0)
    {
        char token[512];
        // nvs_get_token(token);
        nvs_get_string("authentication", "token", token);
        uint16_t token_len = strlen(token);
        send(socket, token, token_len, 0);
    }
    else if (strncmp(rx_buffer, "SET_EMAIL", strlen("SET_EMAIL")) == 0)
    {
        // Skip "SET_EMAIL" command to get the email
        char *email_start = rx_buffer + strlen("SET_EMAIL") + 1; // Plus one for the whitespace after the command
        // Find the end of the email
        char *email_end = strchr(email_start, '\0');
        // Calculate the length of the email
        uint16_t email_len = email_end - email_start;
        // Ensure the email length is within bounds
        if (email_len > 0)
        {
            // Call nvs_set_email function with the email and its length
            bool result = nvs_set_string("authentication", "email", email_start); // nvs_set_email(email_start, email_len);
            // Check if setting the email was successful
            if (result)
            {
                // If successful, send a success response
                const char *success_msg = "Email set successfully";
                send(socket, success_msg, strlen(success_msg), 0);
            }
            else
            {
                // If unsuccessful, send an error response
                const char *error_msg = "Failed to set email";
                send(socket, error_msg, strlen(error_msg), 0);
            }
        }
        else
        {
            // Handle case where no email is found
            // Perhaps send an appropriate response or log a warning
        }
    }
    else if (strcmp(rx_buffer, "GET_EMAIL") == 0)
    {
        char email[512];
        // nvs_get_email(email);
        nvs_get_string("authentication", "email", email);
        uint16_t email_len = strlen(email);
        send(socket, email, email_len, 0);
    }
    else if (strncmp(rx_buffer, "SET_REFRESH_TOKEN", strlen("SET_REFRESH_TOKEN")) == 0)
    {
        // Skip "SET_REFRESH_TOKEN" command to get the refresh token
        char *refresh_token_start = rx_buffer + strlen("SET_REFRESH_TOKEN") + 1; // Plus one for the whitespace after the command
        // Find the end of the refresh token
        char *refresh_token_end = strchr(refresh_token_start, '\0');
        // Calculate the length of the refresh token
        uint16_t refresh_token_len = refresh_token_end - refresh_token_start;
        // Ensure the refresh token length is within bounds
        if (refresh_token_len > 0)
        {
            // Call nvs_set_refresh_token function with the refresh token and its length
            bool result = nvs_set_string("authentication", "refresh_token", refresh_token_start); // nvs_set_refresh_token(refresh_token_start, refresh_token_len);
            // Check if setting the refresh token was successful
            if (result)
            {
                // If successful, send a success response
                // const char *success_msg = "Refresh token set successfully in NVS";
                sprintf(response, "{\"status\":\"Refresh token set successfully in NVS\", \"popID\":\"%s\"}", popID);
                send(socket, response, strlen(response), 0);
            }
            else
            {
                const char *error_msg = "Failed to set refresh token in NVS";
                send(socket, error_msg, strlen(error_msg), 0);
            }
        }
        else
        {
            // Handle case where no refresh token is found
            // Perhaps send an appropriate response or log a warning
        }
    }
    else if (strcmp(rx_buffer, "GET_REFRESH_TOKEN") == 0)
    {
        char refresh_token[512];
        // nvs_get_refresh_token(refresh_token);
        nvs_get_string("authentication", "refresh_token", refresh_token);
        uint16_t refresh_token_len = strlen(refresh_token);
        send(socket, refresh_token, refresh_token_len, 0);
    }
    else if (strcmp(rx_buffer, "ERROR_MESSAGES_READ") == 0)
    {
        error_message_send = true;
    }
}

void send_tcp_message(char *message)
{
    if (global_socket == -1)
    {
        ESP_LOGE(TAG, "Socket not connected from SEND_TCP_MSG");
        transmit_error_flag = true;
        strcpy(transmit, message);
    }
    else
    {
        ESP_LOGI(TAG, "Sending TCP message: %s", message);
        send(global_socket, message, strlen(message), 0);
    }
}

//******************************** API functions implemented here until i get the linker to work - should move to API.c. *******************************

char *get_controler_state(void)
{
    State_t current_state = get_state();
    switch (current_state)
    {
    case NVS_INIT_STATE:
        sprintf(response, "{\"status\":\"NVS initialization state\", \"popID\":\"%s\"}", popID);
        break;
    case AWAIT_WIFI_PROVISIONING_STATE:
        sprintf(response, "{\"status\":\"Awaiting WiFi provisioning\", \"popID\":\"%s\"}", popID);
        break;
    case WIFI_PROVISIONING_STATE:
        sprintf(response, "{\"status\":\"WiFi provisioning state\", \"popID\":\"%s\"}", popID);
        break;
    case WIFI_CONNECTED_STATE:
        sprintf(response, "{\"status\":\"WiFi connected state\", \"popID\":\"%s\"}", popID);
        break;
    case DALI_COMMUNICATION_INIT_STATE:
        sprintf(response, "{\"status\":\"DALI communication initialization state\", \"popID\":\"%s\"}", popID);
        break;
    case DALI_COMMUNICATION_OK_STATE:
        sprintf(response, "{\"status\":\"DALI communication OK state\", \"popID\":\"%s\"}", popID);
        break;
    case ANALYZE_DALI_BUS_STATE:
        sprintf(response, "{\"status\":\"Analyzing DALI bus state\", \"popID\":\"%s\"}", popID);
        break;
    case TCP_SERVER_INIT_STATE:
        sprintf(response, "{\"status\":\"TCP server initialization state\", \"popID\":\"%s\"}", popID);
        break;
    case MDNS_INIT_STATE:
        sprintf(response, "{\"status\":\"mDNS initialization state\", \"popID\":\"%s\"}", popID);
        break;
    case AUTHENTICATION_STATE:
        sprintf(response, "{\"status\":\"Authentication state\", \"popID\":\"%s\"}", popID);
        break;
    case NOT_AUTHENTICATED_STATE:
        sprintf(response, "{\"status\":\"Not authenticated state\", \"popID\":\"%s\"}", popID);
        break;
    case SYSTEM_RUNNING_STATE:
        sprintf(response, "{\"status\":\"System is running\", \"popID\":\"%s\"}", popID);
        break;
    case NO_WIFI_STATE:
        sprintf(response, "{\"status\":\"No WiFi connection\", \"popID\":\"%s\"}", popID);
        break;
    case DALI_BUS_CORRUPTED_STATE:
        sprintf(response, "{\"status\":\"DALI bus corrupted state\", \"popID\":\"%s\"}", popID);
        break;
    case DALI_BUS_NOT_COMMISIONED_STATE:
        sprintf(response, "{\"status\":\"DALI bus not commissioned state\", \"popID\":\"%s\"}", popID);
        break;
    case NO_RESPONSE_ON_DALI_BUS:
        sprintf(response, "{\"status\":\"No response on DALI bus\", \"popID\":\"%s\"}", popID);
        break;
    default:
        sprintf(response, "{\"status\":\"Unknown state or perhaps a programmer added a state without adding it here! Dammit programmer!\"}");
        break;
    }
    return response;
}

void blink_lamp(uint64_t manuID)
{
    selected_driver = manuID;
    set_state(BLINK_LAMP_STATE);
}

void turn_lamp_on(uint8_t short_address)
{
}
void turn_lamp_off(uint8_t short_address)
{
}
