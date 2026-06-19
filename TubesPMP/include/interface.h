#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// Panjang Max Sebuah Perintah
#define CMD_BUFFER_SIZE 80

// Zero-RAM helper function to print translation strings directly from Flash
void print_lut_string(const char* const table[], uint8_t index); 

// Fungsi Inti Interface
void interface_init(void);
void interface_loop(void);
void parse_and_execute(char* cmd_str);

#endif // END INTERFACE_H