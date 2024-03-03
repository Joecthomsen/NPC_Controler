#pragma once

#include "esp_err.h"
#include <stdbool.h>

#define NVS_DRIVER_1 "nvs_driver_one"
#define NVS_DRIVER_2 "nvs_driver_two"
#define NVS_DRIVER_3 "nvs_driver_thr"
#define NVS_DRIVER_4 "nvs_driver_fou"
#define NVS_DRIVER_5 "nvs_driver_fiv"
#define NVS_DRIVER_6 "nvs_driver_six"
#define NVS_DRIVER_7 "nvs_driver_sev"
#define NVS_DRIVER_8 "nvs_driver_eig"
#define NVS_DRIVER_9 "nvs_driver_nin"

#define NVS_DRIVER_10 "nvs_driver_ten"
#define NVS_DRIVER_11 "nvs_driver_elev"
#define NVS_DRIVER_12 "nvs_driver_twe"
#define NVS_DRIVER_13 "nvs_driver_thir"
#define NVS_DRIVER_14 "nvs_driver_fou"
#define NVS_DRIVER_15 "nvs_driver_fif"
#define NVS_DRIVER_16 "nvs_driver_sixt"
#define NVS_DRIVER_17 "nvs_driver_seve"
#define NVS_DRIVER_18 "nvs_driver_eigh"
#define NVS_DRIVER_19 "nvs_driver_nine"

#define NVS_DRIVER_20 "nvs_driver_twev"
#define NVS_DRIVER_21 "nvs_driver_tweone"
#define NVS_DRIVER_22 "nvs_driver_twetwo"
#define NVS_DRIVER_23 "nvs_driver_twethree"
#define NVS_DRIVER_24 "nvs_driver_twefour"
#define NVS_DRIVER_25 "nvs_driver_twefive"
#define NVS_DRIVER_26 "nvs_driver_twe6"
#define NVS_DRIVER_27 "nvs_driver_twe7"
#define NVS_DRIVER_28 "nvs_driver_twe8"
#define NVS_DRIVER_29 "nvs_driver_twe9"

#define NVS_DRIVER_30 "nvs_driver_thirty"
#define NVS_DRIVER_31 "nvs_driver_thirt1"
#define NVS_DRIVER_32 "nvs_driver_thirt2"
#define NVS_DRIVER_33 "nvs_driver_thirt3"
#define NVS_DRIVER_34 "nvs_driver_thirt4"
#define NVS_DRIVER_35 "nvs_driver_thirt5"
#define NVS_DRIVER_36 "nvs_driver_thirt6"
#define NVS_DRIVER_37 "nvs_driver_thirt7"
#define NVS_DRIVER_38 "nvs_driver_thirt8"
#define NVS_DRIVER_39 "nvs_driver_thirt9"

#define NVS_DRIVER_40 "nvs_driver_forty"
#define NVS_DRIVER_41 "nvs_driver_fort1"
#define NVS_DRIVER_42 "nvs_driver_fort2"
#define NVS_DRIVER_43 "nvs_driver_fort3"
#define NVS_DRIVER_44 "nvs_driver_fort4"
#define NVS_DRIVER_45 "nvs_driver_fort5"
#define NVS_DRIVER_46 "nvs_driver_fort6"
#define NVS_DRIVER_47 "nvs_driver_fort7"
#define NVS_DRIVER_48 "nvs_driver_fort8"
#define NVS_DRIVER_49 "nvs_driver_fort9"

#define NVS_DRIVER_50 "nvs_driver_fifty"
#define NVS_DRIVER_51 "nvs_driver_fift1"
#define NVS_DRIVER_52 "nvs_driver_fift2"
#define NVS_DRIVER_53 "nvs_driver_fift3"
#define NVS_DRIVER_54 "nvs_driver_fift4"
#define NVS_DRIVER_55 "nvs_driver_fift5"
#define NVS_DRIVER_56 "nvs_driver_fift6"
#define NVS_DRIVER_57 "nvs_driver_fift7"
#define NVS_DRIVER_58 "nvs_driver_fift8"
#define NVS_DRIVER_59 "nvs_driver_fift9"

#define NVS_DRIVER_60 "nvs_driver_sixty"
#define NVS_DRIVER_61 "nvs_driver_sixt1"
#define NVS_DRIVER_62 "nvs_driver_sixt2"
#define NVS_DRIVER_63 "nvs_driver_sixt3"
#define NVS_DRIVER_64 "nvs_driver_sixt4"

void init_nvs_handler(void);

// General purpose NVS fucntions - all key/value are stored in "stored" namespace
void nvs_write_uint8(const char *key, uint8_t value);
void nvs_write_uint64(const char *key, uint64_t value);
uint8_t nvs_read_uint8(const char *key);
uint64_t nvs_read_uint64(const char *key);
void nvs_delete_key_value_pair(char *namespace, const char *key);

// Special functions with desinated namespaces
void nvs_write_manufactoring_id(const char *key, uint64_t value);
uint64_t nvs_read_manufactoring_id(const char *key);
char *nvs_read_all_manufactoring_ids(void);
bool nvs_synchronize(uint8_t *short_addresses, uint8_t short_addresses_count, uint64_t *manufactoring_ids);
bool nvs_set_token(char *token, size_t token_len);
bool nvs_get_token(char *token);
bool nvs_set_email(char *email, size_t email_len);
bool nvs_get_email(char *email);

bool nvs_set_string(char *namespace, const char *key, char *value);
bool nvs_get_string(char *namespace, const char *key, char *return_value);

bool authenticated(void);

// char *get_manufacturing_id(uint8_t short_address);
// bool manufactoring_id_stored(const char *key);
