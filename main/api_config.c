#include "api_config.h"
#include "wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_credentials.h"
#define ERROR          0







void* _error = NULL;


int enable = 0;

void ok(void)
{
    printf("ok\n");
}
void error_reintentar(void){
    printf("Reintentar con otras credenciales\n");
   enable = 1;
}



void api_config(esp_callback_t main_aplication, esp_callback_t error){
    _error = error;
    wifi_connect("BADNAME","NOTPASS",main_aplication,NULL,error_reintentar);

    printf("sali del primer wifi conect\n");    
    while(!enable){
        vTaskDelay(100);
    }
    enable= 0;
    wifi_connect("BADNAME","NOTPASS",main_aplication,NULL,error_reintentar);

    printf("sali del primer wifi conect\n");    
    while(!enable){
        vTaskDelay(100);
    }
    printf("try again\n");
    wifi_connect(WIFI_CREDENTIALS_ID,WIFI_CREDENTIALS_PASS,main_aplication,NULL,error);



  //  if(ERROR) return error();
  //  return main_aplication();

}
