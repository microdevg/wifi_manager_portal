#ifndef API_CONFIG_H
#define API_CONFIG_H





#include "base.h"


/**
* @brief Start the configuration application
*
* @param main_aplication function that is called and used to call the main application
* @param error function that is called when an error occurs.
 */
void api_config(esp_callback_t main_aplication, esp_callback_t error);


#endif