| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 | Linux |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | ----- |

# DALI Diagnostic Wifi Gateway

The purpose of this project is to provide a simple way to diagnose and monitor DALI lighting systems over the internet.
By connecting the controller to a DALI bus with control gears attached, the controller will commission the bus if needed and send diagnostic data to a back-end ([GitHub link](https://github.com/Joecthomsen/NPC_http_server))

## How to use the project

Follow detailed instructions provided specifically for this example.

Select the instructions depending on Espressif chip installed on your development board:

- [ESP32 Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/index.html)
- [ESP32-S2 Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/index.html)

## Controller folder contents

The project **Controller** contains one source file in C language [main.c](main/main.c). The file is located in folder [main](main).

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt` files that provide set of directives and instructions describing the project's source files and targets (executable, library, or both).

Below is a file tree of remaining files in the project folder.

```
.
├── build
├── CMakeLists.txt
├── components
│   ├── API
│   ├── API_controler
│   ├── components
│   ├── DALI_addressing
│   ├── DALI_commands
│   ├── DALI_communication
│   ├── DALI_diagnostics_and_maintenance
│   ├── DALI_memory_bank_handler
│   ├── device
│   ├── Http_handler
│   ├── Input_button
│   ├── JSON_handler
│   ├── mDNS_handler
│   ├── Nvs_handler
│   ├── State_manager
│   ├── Tcp_server
│   └── Wifi_provisioning
├── dependencies.lock
├── main
│   ├── CMakeLists.txt
│   ├── constants.h
│   ├── idf_component.yml
│   └── main.c
├── managed_components
│   ├── espressif__mdns
│   └── espressif__qrcode
├── README.md   The file you are currently reading
├── sdkconfig
├── sdkconfig.ci
└── sdkconfig.old
```

For more information on structure and contents of ESP-IDF projects, please refer to Section [Build System](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html) of the ESP-IDF Programming Guide.
