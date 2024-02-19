#include "DALI_diagnostics_and_maintenance.h"

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

Http_status post_control_gear_status(Controle_gear_values_t *controle_gear);