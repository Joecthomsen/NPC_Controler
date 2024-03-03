#pragma once

#include "DALI_diagnostics_and_maintenance.h"

#define URL "http://localhost:8080/controleGear/new_data_instance"

typedef enum Http_status
{
    HTTP_OK = 200,
    HTTP_CREATED = 201,
    HTTP_BAD_REQUEST = 400,
    HTTP_UNAUTHORIZED = 401,
    HTTP_FORBIDDEN = 403,
    HTTP_NOT_FOUND = 404,
    HTTP_INTERNAL_SERVER_ERROR = 500,
    HTTP_NOT_IMPLEMENTED = 501
} Http_status;

typedef enum HTTP_REQUEST_TYPE
{
    HTTP_NON_REQUEST,
    HTTP_ACCESS_TOKEN_REQUEST,
} HTTP_REQUEST_TYPE;

// Http_status post_control_gear_status(Controle_gear_values_t *controle_gear);
Http_status post_json_data(const char **keys, const char **values, int num_pairs);
Http_status post_controle_gear_data(const Controle_gear_values_t *controle_gear);
Http_status authenticate_token(const char *token);
