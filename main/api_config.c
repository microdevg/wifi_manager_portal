#include "api_config.h"
#include "wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_credentials.h"











void api_config(esp_callback_t main_aplication, esp_callback_t error){

    wifi_connect(WIFI_CREDENTIALS_ID,WIFI_CREDENTIALS_PASS,main_aplication,NULL,error);



}
