/*
Tugas Besar PMP K02 Kelompok 01
Nama file/function: main.c
Deskripsi file/function: File utama yang mengintegrasikan inisialisasi modul UART, database inventaris, dan antarmuka pengguna, serta menjalankan loop utama program mikrokontroler.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../include/inventory_db.h"
#include "../include/uart_driver.h"
#include "../include/interface.h"

int main(void) {
    uart_init(9600);
    initDB();
    interface_init();

    while (1) {
        interface_loop();
    }

    return 0;
}
