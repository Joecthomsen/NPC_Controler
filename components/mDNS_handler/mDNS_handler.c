#include <stdio.h>
#include "mDNS_handler.h"
#include "../../managed_components/espressif__mdns/include/mdns.h"
#include "../../main/constants.h"

void mDNS_init(const char *popID)
{
    // printf("mDNS_init %s\n", pop);
    esp_err_t err = mdns_init();
    if (err)
    {
        printf("MDNS Init failed: %d\n", err);
        return;
    }
    else
    {
        printf("MDNS Init succeeded\n");
    }

    // Set hostname (append popID to the hostname)
    char hostname[30];                                             // Define a buffer to hold the concatenated hostname
    snprintf(hostname, sizeof(hostname), "NPC_Connect_%s", popID); // Concatenate popID with the hostname

    // set hostname
    mdns_hostname_set(hostname);

    // Set instance name (use hostname as the service name)
    mdns_instance_name_set(hostname);
    // add services
    mdns_service_add(NULL, "_tcp", "_tcp", 3333, NULL, 0);
}
