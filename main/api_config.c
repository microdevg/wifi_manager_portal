#include "api_config.h"
#include "wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_credentials.h"











void api_config(esp_callback_t main_aplication, esp_callback_t error){


    char id[32];
    char pass[64];

    wifi_init_ap(id,pass);
    
    vTaskDelay(50000/portTICK_PERIOD_MS);
    
    wifi_connect(id,pass,main_aplication,NULL,error);



}
