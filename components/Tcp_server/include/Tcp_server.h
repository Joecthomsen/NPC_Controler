#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <stdbool.h>

#define LISTEN_PORT 3333

void tcp_server_task(void *pvParameters);
void send_tcp_message(char *message); // Notify the TCP server that a DALI transmission error has occured

extern uint8_t selected_driver;

#endif /* TCP_SERVER_H */