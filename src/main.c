#include <avr/io.h>
#include <avr/interrupt.h>
#include "../include/inventory_db.h"
#include "../include/uart_driver.h"
#include "../include/interface.h"

int main(void) {
    // 1. Initialize core system hardware
    uart_init(9600);
    initDB();
    
    // 2. Clear out line tracking and fire boot menu greetings
    interface_init();

    // 3. Execution Cycle Loop
    while (1) {
        // Keeps parsing and assembling text strings as long as bytes roll down the pin
        interface_loop();
    }

    return 0;
}