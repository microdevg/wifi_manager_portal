#ifndef _WIFI_H
#define _WIFI_H
#include  "base.h"








/**
 * @brief Inicio conexión Wifi en STATION MODE por default.
 * 
 * @param WIFI_ID Nombre de la red WiFi
 * @param PASS  Contraseña de la red
 * @param cb_conn Función callback se llama cuando el dispositivo se conecto a la red
 * @param cb_desconn  Función callback se llama cuando el dispositivo se desconecto de la red.
 * @param cb_failed Función callback se llama cuando fallaron los intentos de conectarse a la red.
 * @return esp_err_t 
 */
 esp_err_t wifi_connect(const char* WIFI_ID, const char * PASS,
                        esp_callback_t cb_conn, 
                        esp_callback_t cb_desconn,
                        esp_callback_t cb_failed);

/**
 * @brief Libera los recursos asignados al WiFi.
 * 
 * @return esp_err_t 
 */
esp_err_t wifi_desconnect(void);

#endif