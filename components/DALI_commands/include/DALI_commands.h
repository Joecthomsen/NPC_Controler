#pragma once

#define INITIALIZE 0xC101FE     //Send twice
#define RANDOMIZE 0xC10200      //Send twice
#define COMPARE 0xC10300      
#define WITHDRAW 0xC10400
#define PROGRAM_SHORT_ADDRESS (address) (0xC10800 + (address))
#define VERIFY_SHORT_ADDRESS (address) (0xC10900 + (address))
#define DAPC_ON 0xFEFE
#define DAPC_OFF 0xFE00