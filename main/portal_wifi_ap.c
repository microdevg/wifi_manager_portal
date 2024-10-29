#include "wifi.h"

#include "esp_wifi.h"

#include "nvs_flash.h"

#include <string.h>


#include "dns_server.h"

#include <stdio.h>
#include "esp_http_server.h"
#include "flash_mem.h"
#include "page.h"
#include "portal_wifi_ap.h"



#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif



static int waiting = 1;


// Handle HTTP GET for the root page (Captive Portal)
static esp_err_t root_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html_page, strlen(html_page));
    return ESP_OK;
}

static esp_err_t connect_post_handler(httpd_req_t *req)
{
    char id[32];
    char pass[64];
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

        sscanf(buf, "ssid=%31[^&]&password=%63s", id, pass); // Parse credentials
    }
    //printf( "Received SSID: %s, Password: %s", _id, _pass);
    httpd_resp_set_type(req, "text/html");
    httpd_resp_sendstr(req, confirmation_page);
    flash_mem_init();
    flash_mem_save(id,pass);
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







int *  portal_wifi_ap(){
 
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
    return &waiting;
}