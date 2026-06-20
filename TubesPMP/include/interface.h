/*
Tugas Besar PMP K02 Kelompok 01
Nama file/function: interface.h
Deskripsi file/function: File header yang berisi deklarasi makro dan fungsi-fungsi untuk antarmuka pengguna (user interface) dan pemrosesan perintah.
*/

#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// Panjang Max Sebuah Perintah
#define CMD_BUFFER_SIZE 80

/*
Nama file/function: print_lut_string
Deskripsi file/function: Zero-RAM helper function untuk mencetak string terjemahan secara langsung dari memori Flash menggunakan indeks.
*/
void print_lut_string(const char* const table[], uint8_t index); 

/*
Nama file/function: interface_init
Deskripsi file/function: Fungsi inti untuk melakukan inisialisasi awal pada sistem antarmuka (interface) sebelum digunakan.
*/
void interface_init(void);

/*
Nama file/function: interface_loop
Deskripsi file/function: Fungsi inti yang berjalan dalam loop utama untuk terus memantau dan menangani input dari sistem antarmuka.
*/
void interface_loop(void);

/*
Nama file/function: parse_and_execute
Deskripsi file/function: Fungsi untuk membaca, memecah (parsing), dan mengeksekusi string perintah (command) yang diterima dari pengguna.
*/
void parse_and_execute(char* cmd_str);

#endif // END INTERFACE_H
