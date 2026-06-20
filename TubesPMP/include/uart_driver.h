/*
Tugas Besar PMP K02 Kelompok 01
Nama file/function: uart_driver.h
Deskripsi file/function: File header yang berisi deklarasi fungsi dan makro untuk antarmuka komunikasi serial UART pada mikrokontroler AVR.
*/

#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdint.h>

// Parameter Ring Buffer RX UART 
#define UART_RX_BUFFER_SIZE 64

// Fungsi Primitif Driver

/*
Nama file/function: uart_init
Deskripsi file/function: Fungsi untuk menginisialisasi modul UART dengan baudrate yang ditentukan.
*/
void uart_init(uint32_t baudrate);

/*
Nama file/function: uart_putchar
Deskripsi file/function: Fungsi untuk mengirimkan satu karakter (byte) melalui jalur transmisi UART.
*/
void uart_putchar(char c);

// Fungsi Utilitas String

/*
Nama file/function: uart_print
Deskripsi file/function: Fungsi untuk mengirimkan string (array of char) dari memori SRAM melalui UART.
*/
void uart_print(const char* str);

/*
Nama file/function: uart_print_P
Deskripsi file/function: Fungsi untuk mengirimkan string yang tersimpan di memori Flash (PROGMEM) melalui UART untuk menghemat penggunaan RAM.
*/
void uart_print_P(const char* str); // TAMBAHAN PENTING

/*
Nama file/function: uart_print_num
Deskripsi file/function: Fungsi untuk mengirimkan bilangan bulat (integer) dalam bentuk teks (string) melalui UART.
*/
void uart_print_num(uint16_t num);

// Fungsi Utilitas Pembacaaan

/*
Nama file/function: uart_available
Deskripsi file/function: Fungsi untuk mengecek ketersediaan data atau karakter yang belum dibaca pada ring buffer RX UART.
*/
void uart_available(uint8_t* available);

/*
Nama file/function: uart_getchar
Deskripsi file/function: Fungsi untuk mengambil dan membaca satu karakter dari ring buffer RX UART.
*/
void uart_getchar(char* c);

/*
Nama file/function: uart_flush
Deskripsi file/function: Fungsi untuk membersihkan atau mengosongkan seluruh isi sisa data yang ada di ring buffer RX UART.
*/
void uart_flush(void);

#endif // UART_DRIVER_H
