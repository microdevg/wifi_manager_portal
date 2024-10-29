#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#include "esp_system.h"
#include <string.h>


#include "api_config.h"
#include "wifi.h"
#include "flash_mem.h"
#include "portal_wifi_ap.h"





void clear_credential(){
    printf("Credenciales invalidad, borrar e iniciar ap\n");
    flash_mem_save("","");
    esp_restart();
}

void reconection(){
    printf("trying to reconnected\n");
}


void api_config(esp_callback_t main_aplication, esp_callback_t error){

    char id[32];
    char pass[64];
    printf("Verifico si tengo credenciales validas\n");
    flash_mem_init();
    flash_mem_get( id, pass);
    printf("datos leidos: %s %s\n",id,pass);

    if(strlen(id) == 0 ){
        printf("Credenciales invalidad, inicio interfaz\n");
        int* waiting = portal_wifi_ap();
        while(*waiting){
            vTaskDelay(500/portTICK_PERIOD_MS);
            printf(".\n");
        }
        printf("Tengo las credenciales, reinciio el sistema\n");
        esp_restart();
    }
    else
    {    // Si tengo las credenciales
        printf("Conectandome a WiFi :%s %s\n", id,pass);
        wifi_connect(id,pass,main_aplication,reconection,clear_credential);
    }
}
