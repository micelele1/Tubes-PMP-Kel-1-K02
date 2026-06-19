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