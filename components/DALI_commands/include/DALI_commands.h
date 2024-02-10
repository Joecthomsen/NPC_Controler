#pragma once

#define INITIALIZE_ALL_DEVICE 0xA500   // Send twice
#define GENERATE_RANDOM_ADDRESS 0xA700 // Send twice
#define COMPARE 0xA900
#define WITHDRAW 0xAB00
#define SEARCH_ADDRESS_H 0xB100
#define SEARCH_ADDRESS_M 0xB300
#define SEARCH_ADDRESS_L 0xB500
#define PROGRAM_SHORT_ADDRESS 0xB700 // plus address
#define VERIFY_SHORT_ADDRESS 0xB900  // plus address
#define TERMINATE 0xA100
#define DAPC_ON 0xFEFE
#define DAPC_OFF 0xFE00