#include "flash_mem.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <stdio.h>


static nvs_handle_t my_handle;


static int flash_inited = 0;


#define SSID_KEY "nombrewifi"
#define PASSWORD_KEY "wificontraseña"

void flash_mem_init(){

    ESP_ERROR_CHECK(nvs_flash_init());


    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);

     if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return;
    }
    else
    {
        printf("Inicializado correctamente\n");
        flash_inited = 1;
    }


}


void flash_mem_save(char* id,char* pass){


    if(flash_inited == 0)flash_mem_init();


      // Guardar SSID
    esp_err_t err = nvs_set_str(my_handle, SSID_KEY, id);
    if (err != ESP_OK)
    {
        printf("Failed to save SSID!\n");
    }
    else
    {
        printf("guardado correctamente");
    }

    // Guardar Contraseña
    err = nvs_set_str(my_handle, PASSWORD_KEY, pass);
    if (err != ESP_OK)
    {
        printf("Failed to save Password!\n");
    }

    // Confirmar cambios
    err = nvs_commit(my_handle);
    if (err != ESP_OK)
    {
        printf("Failed to commit NVS updates!\n");
    }
    else
    {
        printf("guardado correctamente2");
    }

    nvs_close(my_handle);
    flash_inited = 0;


}


esp_err_t flash_mem_get(char* id, char* pass){

    if(flash_inited == 0)flash_mem_init();

     esp_err_t err = nvs_open("storage", NVS_READONLY, &my_handle);
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return err;
    }


    // Leer SSID
    size_t ssid_size =0;
    // Primero con parametro id en NULL, se lee el size del string
    err = nvs_get_str(my_handle, SSID_KEY, NULL, &ssid_size);
    // Luego se lee el string
    err = nvs_get_str(my_handle, SSID_KEY, id, &ssid_size);


    

    if (err != ESP_OK)
    {
        printf("Failed to read SSID!\n");
        return err;
    }

    // Leer Contraseña
    size_t pass_size = 0;
    
    err = nvs_get_str(my_handle, PASSWORD_KEY, NULL, &pass_size);
    err = nvs_get_str(my_handle, PASSWORD_KEY, pass, &pass_size);
    if (err != ESP_OK)
    {
        printf("Failed to read Password!\n");
        return err;
    }

    printf("NVS leída correctamente SSID:%s  password:%s", id, pass);

    nvs_close(my_handle);
    flash_inited = 0;
    return err;

}



int flash_memory_clear_credentials(){

     if(flash_inited == 0)flash_mem_init();

     esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return err;
    }


     nvs_erase_all(&my_handle);
    err = nvs_erase_key(&my_handle,SSID_KEY );


    err = nvs_erase_key(&my_handle,PASSWORD_KEY );
    
     // Confirmar cambios
    err = nvs_commit(my_handle);


    nvs_close(my_handle);
    return err;
}

void flash_mem_deinit(){


}