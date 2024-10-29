#ifndef BASE_H
#define BASE_H



#include "esp_err.h"
#include <stdbool.h>



#define NOT_CALLBACK        (NULL)

typedef void (*esp_callback_t)(void);


#define CHECK_RUN_F(f)            if(f)f()     // si f es distinta de NULL, llamar

#endif