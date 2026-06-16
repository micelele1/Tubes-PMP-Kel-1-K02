#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "../include/inventory_db.h"

// Define CPU Clock Speed and desired Baud Rate for the register math
#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR ((F_CPU / (16UL * BAUD)) - 1)

extern unsigned char *__brkval;
// 1. Hardcore primitive: Send a single character directly out of the physical Tx pin
void raw_uart_putchar(char c) {
    // Wait for the hardware transmit buffer register (UDRE0) to empty out
    while (!(UCSR0A & (1 << UDRE0)));
    // Shove the character into the I/O data register to be broadcasted over USB
    UDR0 = c;
}

// 2. The Bridge Function: Adapts our hardware putchar to the stdio.h system requirements
int uart_putchar_printf(char c, FILE *stream) {
    // Standard terminal files interpret '\n' as just a line feed. 
    // We add a Carriage Return '\r' automatically so it lines up correctly on your laptop monitor.
    if (c == '\n') {
        raw_uart_putchar('\r');
    }
    raw_uart_putchar(c);
    return 0;
}

// 3. Create a custom standard I/O stream using the AVR toolchain macro
// This links our bridge function to stdout (Standard Output)
static FILE uart_stdout = FDEV_SETUP_STREAM(uart_putchar_printf, NULL, _FDEV_SETUP_WRITE);

// 4. Initialize the actual physical registers of the microcontroller
void init_uart_hardware(unsigned int ubrr) {
    // Load calculated baud rate boundaries into high/low tracking registers
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    
    // Enable the transmitter hardware circuitry (TXEN0)
    UCSR0B = (1 << TXEN0);
    
    // Set serial transmission style: 8 Data Bits, 1 Stop Bit
    UCSR0C = (3 << UCSZ00);
    
    // REDIRECT: Redirect standard output (stdout) to flow straight into our stream link
    stdout = &uart_stdout;
}

// int main(void) {
//     // Initialize baremetal Serial hardware line at 9600 Baud Rate
//     init_uart_hardware(MYUBRR);
    
//     printf("\n=== ARDUINO UNO DB DIAGNOSTIC RUN ===\n");
    
//     // Step 1: Initialize Database
//     initDB();
//     printf("[INIT] Database initialized.\n");
    
//     // Establish clear baseline tracking variables at the top
//     uint16_t initial_mem = 0;
//     uint16_t post_insert_mem = 0;
//     uint16_t current_mem = 0; // FIXED: Temporary tracking needle

//     cekMemori(&initial_mem);
//     printf("[MEM] Initial Free SRAM: %u bytes\n", initial_mem);
    
//     // --- TEST 1: Insertion ---
//     printf("\n[TEST 1] Inserting first item (ID: 1)...\n");
//     insertItem(1, "ESP32", K_MIKON, 10, "R1A", S_TERSEDIA, P_LDTE, "TNK");
    
//     cekMemori(&post_insert_mem);
//     printf("[MEM] Free SRAM after insertion: %u bytes (%u bytes used)\n", 
//            post_insert_mem, (initial_mem - post_insert_mem));
    
//     // FIXED: Check if the heap failed to move entirely
//     if (initial_mem == post_insert_mem) {
//         printf("[FAIL] Allocation failed! No memory claimed.\n");
//         while(1);
//     }
//     printf("[PASS] Memory consumption holds true.\n");

//     // --- TEST 2: Search/Retrieval ---
//     printf("\n[TEST 2] Executing search for ID 1...\n");
//     Inventaris* search_result = NULL;
//     searchItem(1, &search_result);
    
//     if (search_result == NULL || search_result->id != 1) {
//         printf("[FAIL] Item retrieval broken!\n");
//         while(1);
//     }
//     printf("[PASS] Item found! Name: %s, Qty: %u, Location: %s\n", 
//            search_result->nama, search_result->jumlah, search_result->lokasi);

//     // --- TEST 3: Duplicate Rejection ---
//     printf("\n[TEST 3] Attempting to force duplicate ID 1...\n");
    
//     // Take a snapshot of the actual Heap boundary address before insertion
//     unsigned char* heap_before = __brkval;
    
//     insertItem(1, "SG920", K_AKTUATOR, 5, "R1B", S_TERSEDIA, P_HME, "ABC");
    
//     // Verification: If duplicate protection works, the Heap pointer must not have moved at all!
//     if (__brkval != heap_before) {
//         printf("[FAIL] Duplicate allowed to leak onto the Heap!\n");
//         while(1);
//     }
//     printf("[PASS] Duplicate blocked successfully. Heap boundary preserved.\n");

//     // --- TEST 4: Updates ---
//     printf("\n[TEST 4] Updating item quantities...\n");
//     updateJumlah(1, 45);
//     updateStatus(1, S_DIPINJAM);
    
//     searchItem(1, &search_result);
//     if (search_result->jumlah != 45 || search_result->status != S_DIPINJAM) {
//         printf("[FAIL] Heap mutation failed!\n");
//         while(1);
//     }
//     printf("[PASS] Fields altered on the heap successfully.\n");

//     // --- TEST 5: Deletion & Cleanup ---
//     printf("\n[TEST 5] Deleting ID 1 and inspecting cleanup...\n");
//     deleteItem(1);
    
//     searchItem(1, &search_result);
//     if (search_result != NULL) {
//         printf("[FAIL] Node was not orphaned safely!\n");
//         while(1);
//     }
    
//     // Check our dynamic available RAM gap again
//     cekMemori(&current_mem);
//     printf("[MEM] Free SRAM after full deletion: %u bytes (Initial baseline was: %u)\n", current_mem, initial_mem);
    
//     // VERIFICATION: Because the stack shifts slightly due to nested printf calls, 
//     // allow a tiny 4-byte tolerance. As long as current_mem is within a few bytes 
//     // of initial_mem, the heap space was successfully reclaimed!
//     if (current_mem < (initial_mem - 4)) {
//         printf("[FAIL] Memory leak caught! Free list failed to reclaim space.\n");
//         while(1);
//     }
//     printf("[PASS] Heap memory successfully reclaimed and recycled!\n");

//     printf("\n=== ALL TESTS SUCCESSFUL! BACKEND AIRTIGHT ===\n");
    
//     // Light up Pin 13 LED solidly on complete pass
//     DDRB |= (1 << DDB5);
//     PORTB |= (1 << PB5);

//     while(1) {
//         // Keep core loop alive
//     }
//     return 0;
// }