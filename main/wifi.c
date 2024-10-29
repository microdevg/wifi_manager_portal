#include "wifi.h"

#include "wifi.h"
#include "esp_wifi.h"

#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include <string.h>


#include "dns_server.h"

#include <stdio.h>
#include "esp_http_server.h"
#include "flash_mem.h"
#include "page.h"
#define RCONN_TRY_MAX                   10



#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif



esp_callback_t __callback_connection = NOT_CALLBACK;
esp_callback_t __callback_disconnection = NOT_CALLBACK;
esp_callback_t __callback_failed = NOT_CALLBACK;




typedef struct {
    bool wifi_connected ;
    bool wifi_got_ip ;
    uint8_t try_counter;
}wifi_state_t;




static wifi_state_t _state ={
    .wifi_connected = false,
    .wifi_got_ip = false,
    .try_counter = 0,
};




static int loop_inited = 0;
static int waiting = 1;

#define ESP32_AP_NAME       "QMAX-CONFIG"







static char* _id;
static char* _pass;

//-----------------------------------------------------------CRD PROVIDE-----------------------------
// Handle HTTP GET for the root page (Captive Portal)
static esp_err_t root_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html_page, strlen(html_page));
    return ESP_OK;
}

static esp_err_t connect_post_handler(httpd_req_t *req)
{
    char buf[100];
    int ret, remaining = req->content_len;
    while (remaining > 0)
    {
        ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)));
        if (ret <= 0)
        {
            return ESP_FAIL;
        }
        remaining -= ret;
        buf[ret] = '\0';

        sscanf(buf, "ssid=%31[^&]&password=%63s", _id, _pass); // Parse credentials
    }

    //printf( "Received SSID: %s, Password: %s", _id, _pass);

    httpd_resp_set_type(req, "text/html");
    httpd_resp_sendstr(req, "Credenciales recibidas, conectado a la red");

    flash_mem_init();
    flash_mem_save(_id,_pass);
    printf("Credenciales guardadas\n");
    waiting = 0;

    return ESP_OK;
}




static const httpd_uri_t root = {.uri = "/",.method = HTTP_GET,.handler = root_get_handler};

static const httpd_uri_t connect = {.uri = "/c", .method = HTTP_POST, .handler = connect_post_handler};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    if (httpd_start(&server, &config) == ESP_OK)
    {
        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &root));
        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &connect));
    }
    else printf("Error al iniciar servidor\n");
    return server;
}



int *  wifi_init_ap(char* wifi_id, char*wifi_pass){
    _id = wifi_id;
    _pass = wifi_pass;
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid =ESP32_AP_NAME,
            .ssid_len = strlen(ESP32_AP_NAME),
            .password = "", //"12345678",
            .max_connection = 4,
            .authmode = WIFI_AUTH_OPEN},   //  WIFI_AUTH_WPA_WPA2_PSK
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());


    start_webserver();

    dns_server_config_t dns_config = DNS_SERVER_CONFIG_SINGLE("*", "WIFI_AP_DEF");
    start_dns_server(&dns_config);
    loop_inited = 1;

    return &waiting;
}





static void event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)
{
    if( event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) esp_wifi_connect();
    else if( event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED)
            {   if(_state.try_counter < RCONN_TRY_MAX){
                    _state.wifi_connected = false;
                    printf("[Wifi desconectado, intentando reconexion:%u / %u]\n",_state.try_counter,RCONN_TRY_MAX);
                    _state.try_counter ++;
                    esp_wifi_connect();}
                else{
                    printf("[Fallaron los intentos de reconexion]\n");
                    _state.try_counter = 0;
                    CHECK_RUN_F(__callback_failed);}
                return;
            }
   
    if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP){
        printf("evento obtuve ip\n");
        _state.wifi_got_ip = true;
        CHECK_RUN_F( __callback_connection);}

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED){
        printf("evento se conecto a la red wifi\n");
        _state.try_counter = 0;
        _state.wifi_connected = true;
        return;}
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED){
        printf("evento me desconecte de la red\n");
        _state.wifi_connected = false;
        CHECK_RUN_F(__callback_disconnection);
        return;}
}









 esp_err_t wifi_connect(const char* WIFI_ID, const char * PASS,
                        esp_callback_t cb_conn , 
                        esp_callback_t cb_disconn,
                        esp_callback_t cb_failed ){
    
    // Si ya esta conectado, no conecto nuevamente
    if(_state.wifi_connected) return ESP_OK;

    // Asigno los callback recibidos por parámetro o los por defecto
    __callback_connection = cb_conn ;
    __callback_disconnection = cb_disconn ;
    __callback_failed = cb_failed;

     esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(err = nvs_flash_erase());
      err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err = esp_netif_init());

    // Configure el netif
    if(!loop_inited){
 
    ESP_ERROR_CHECK(err = esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
     // Configuración por defecto.
    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    // Esta es la primera funcion que debe llamarse para el manejo de WiFi
    ESP_ERROR_CHECK(err = esp_wifi_init(&config));
    
    esp_event_handler_instance_t instance_any_id;
    ESP_ERROR_CHECK(err = esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
                           
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    }
  



    wifi_config_t wifi_config = {0};
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    strncpy((char*)wifi_config.sta.ssid, (char*)WIFI_ID,strlen(WIFI_ID));
    strncpy((char*)wifi_config.sta.password, (char*)PASS,strlen(PASS));

    

    ESP_ERROR_CHECK(err = esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(err = esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(err = esp_wifi_start());
    loop_inited = 1;

    printf( "wifi_init_softap finished. SSID:%s password:%s \n",WIFI_ID, PASS);

    return err;
}





esp_err_t wifi_desconnect(void){
    esp_err_t err = ESP_OK;
    printf("Desconectando wifi, liberando recursos\n");
    // Si ya estoy desconectado, ignoro.
    if( _state.wifi_connected == false) return ESP_OK;
    _state.try_counter = 0;
    _state.wifi_connected = false;
    _state.wifi_got_ip = false;
    ESP_ERROR_CHECK(err = esp_wifi_stop());
    ESP_ERROR_CHECK(err = esp_wifi_disconnect());
    ESP_ERROR_CHECK(err = esp_wifi_deinit());
    return err;
}