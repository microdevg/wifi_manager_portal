#include <stdio.h>


#include <stdio.h>
#include "api_config.h"




void main_aplication(void){


    printf("This is my main aplication\n");
}



void error(void){
    printf("Error in api_config, reset\n");
}

void app_main(void)
{
    api_config(main_aplication,error);
}
