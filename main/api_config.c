#include "api_config.h"



#define ERROR           1

void api_config(esp_callback_t main_aplication, esp_callback_t error){

    if(ERROR) return error();
    return main_aplication();
}
