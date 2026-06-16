#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../include/inventory_db.h"
#include "../include/uart_driver.h"

// int main(void) {
//     // 1. Initialize the Baremetal UART Driver at 9600 Baud
//     // This executes our register setup and enables the physical Rx/Tx circuitry
//     uart_init(9600);
    
//     // 2. Initialize our Inventory Database Layer
//     initDB();
    
//     // 3. Clear your terminal screen and print a welcoming header
//     // Using our lightweight uart_print instead of the heavy printf!
//     uart_print("\n\e[1;36m=== BAREMETAL UART & DATABASE INTEGRATION TEST ===\e[0m\n");
    // uart_print("System status: ACTIVE\n");
    
    // // 4. Check our available SRAM room using our custom utility
    // uint16_t free_ram_space = 0;
    // cekMemori(&free_ram_space);
    
    // uart_print("Available SRAM Room: ");
    // uart_print_num(free_ram_space);
    // uart_print(" bytes\n");
    // uart_print("--------------------------------------------------\n");
//     uart_print("Type anything into your terminal. The Arduino will catch it\n");
//     uart_print("via Asynchronous Interrupts and echo it back live!\n\n");

//     // Variables to manage the incoming live stream characters
//     uint8_t waiting_bytes = 0;
//     char received_character = '\0';

//     // 5. The Infinite Execution Loop
//     while (1) {
//         // Poll our custom driver to see if the interrupt has placed bytes in the ring buffer
//         uart_available(&waiting_bytes);
        
//         if (waiting_bytes > 0) {
//             // A byte is waiting! Extricate it safely out of the Ring Buffer
//             uart_getchar(&received_character);
            
//             // Visual confirmation: Echo the bracketed character straight back to the user
//             uart_print("[Echo: ");
//             uart_putchar(received_character);
//             uart_print("] ");
//         }
        
//         // The CPU stays completely free to run other tasks here without slowing down the serial data!
//         _delay_ms(1); 
//     }

//     return 0;
// }