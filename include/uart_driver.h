#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>

// Parameter Ring Buffer RX UART 
#define UART_RX_BUFFER_SIZE 64

// Fungsi Primitif Driver
void uart_init(uint32_t baudrate);          // Inisialisasi Register USART dan Interrupt
void uart_putchar(char c);                  // Kirim Satu Karakter ke Hardware Tx Pin

// Fungsi Utilitas String
void uart_print(const char* str);           // Kirim String Null-Terminated ke Hardware TX Pin
void uart_print_num(uint16_t num);          // Kirim Angka Unsigned 16-bit sebagai String ke Hardware TX Pin
void uart_print_p(const char* strp);        // Kirim String Dari Progmem Ke Hardware TX

// Fungsi Utilitas Pembacaaan
void uart_available(uint8_t* available);    // Cek Jumlah Karakter yang Tersedia di Buffer RX
void uart_getchar(char* c);                 // Baca Satu Karakter dari Buffer RX (Blocking)
void uart_flush(void);                      // Bersihkan Buffer RX

#endif // UART_DRIVER_H