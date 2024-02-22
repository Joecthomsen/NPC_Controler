#include <stdio.h>
#include "mDNS_handler.h"
#include "../../managed_components/espressif__mdns/include/mdns.h"

void mDNS_init(void)
{
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

    // set hostname
    mdns_hostname_set("NPC_Connect");
    // set default instance
    mdns_instance_name_set("ESP32C3 TCP Server");
    // add services
    mdns_service_add(NULL, "_tcp", "_tcp", 3333, NULL, 0);
}
