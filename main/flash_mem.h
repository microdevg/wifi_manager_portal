#ifndef FLASH_MEM_H
#define FLASH_MEM_H



void flash_mem_init();


void flash_mem_save(char* id,char* pass);


void flash_mem_get(char* id, char* pass);



void flash_mem_deinit();
#endif