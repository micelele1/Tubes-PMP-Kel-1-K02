#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdint.h>

// Parameter Ring Buffer RX UART 
#define UART_RX_BUFFER_SIZE 64

// Fungsi Primitif Driver
void uart_init(uint32_t baudrate);
void uart_putchar(char c);

// Fungsi Utilitas String
void uart_print(const char* str);
void uart_print_P(const char* str); // TAMBAHAN PENTING
void uart_print_num(uint16_t num);

// Fungsi Utilitas Pembacaaan
void uart_available(uint8_t* available);
void uart_getchar(char* c);
void uart_flush(void);

#endif // UART_DRIVER_H