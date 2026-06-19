#include <avr/io.h>
#include <avr/interrupt.h>
#include "../include/inventory_db.h"
#include "../include/uart_driver.h"
#include "../include/interface.h"

int main(void) {
// 1. Inisialisasi hardware dan baudrate serial
uart_init(9600);
initDB();
// 2. Bersihkan buffer antarmuka dan tampilkan menu
interface_init();

// 3. Execution Cycle Loop
while (1) {
    interface_loop();
}

return 0;
}