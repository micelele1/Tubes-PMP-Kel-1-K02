#include "../include/interface.h"
#include "../include/uart_driver.h"
#include "../include/inventory_db.h"
#include <avr/pgmspace.h> // WAJIB DITAMBAHKAN UNTUK PSTR

// Internal line buffer states
static char cmd_buffer[CMD_BUFFER_SIZE];
static uint8_t cmd_idx = 0;

void interface_init(void) {
    cmd_idx = 0;
    cmd_buffer[0] = '\0';
    uart_print_P(PSTR("\n=== INVENTARIS TERMINAL READY ===\n"));
    uart_print_P(PSTR("Format: MENU atau CMD;PARAM1;PARAM2;...\n>> "));
}

void interface_loop(void) {
    uint8_t bytes_waiting = 0;
    uart_available(&bytes_waiting);

    while (bytes_waiting > 0) {
        char c = '\0';
        uart_getchar(&c); 

        // 1. HANDLE BACKSPACE (ASCII 127 or '\b') FIRST
        if (c == 127 || c == '\b') {
            if (cmd_idx > 0) {
                cmd_idx--;
                cmd_buffer[cmd_idx] = '\0';
                // Send the exact terminal escape sequence to physically erase the character:
                uart_print_P(PSTR("\b \b")); 
            }
        } 
        // 2. HANDLE ENTER KEY (Line Termination)
        else if (c == '\r' || c == '\n') {
            if (cmd_idx > 0) { 
                cmd_buffer[cmd_idx] = '\0'; 
                uart_print_P(PSTR("\n")); 
                
                parse_and_execute(cmd_buffer); 
                
                // Reset buffer for the next round
                cmd_idx = 0;
                cmd_buffer[0] = '\0';
                uart_print_P(PSTR(">> "));
            }
        } 
        // 3. HANDLE REGULAR PRINTABLE CHARACTERS ONLY
        else {
            if (c >= 32 && c <= 126) { 
                if (cmd_idx < (CMD_BUFFER_SIZE - 1)) {
                    uart_putchar(c); 
                    cmd_buffer[cmd_idx++] = c;
                }
            }
        }
        
        uart_available(&bytes_waiting);
    }
}

void parse_and_execute(char* cmd_str) {
    char* token = strtok(cmd_str, ";");
    if (token == NULL) return;

    if (strcmp(token, "MENU") == 0 || strcmp(token, "HELP") == 0) {
        uart_print_P(PSTR("--- FITUR COMMAND TERMINAL ---\n"));
        uart_print_P(PSTR("1. MENU / HELP\n"));
        uart_print_P(PSTR("2. INSERTS;ID;NAMA;KAT;JML;LOKASI;STAT;OWN;PIC\n"));
        uart_print_P(PSTR("3. SEARCH;ID\n"));
        uart_print_P(PSTR("4. DELETE;ID\n"));
        return;
    }

    if (strcmp(token, "INSERTS") == 0) {
        char* p_id       = strtok(NULL, ";");
        char* p_nama     = strtok(NULL, ";");
        char* p_kategori = strtok(NULL, ";");
        char* p_jumlah   = strtok(NULL, ";");
        char* p_lokasi   = strtok(NULL, ";");
        char* p_status   = strtok(NULL, ";");
        char* p_pemilik  = strtok(NULL, ";");
        char* p_PIC      = strtok(NULL, ";");

        if (!p_id || !p_nama || !p_kategori || !p_jumlah || !p_lokasi || !p_status || !p_pemilik || !p_PIC) {
            uart_print_P(PSTR("Error: Parameter INSERT tidak lengkap!\n"));
            return;
        }

        uint16_t id = (uint16_t)atoi(p_id);
        uint8_t kat = (uint8_t)atoi(p_kategori);
        uint8_t jml = (uint8_t)atoi(p_jumlah);
        uint8_t stat = (uint8_t)atoi(p_status);
        uint8_t own = (uint8_t)atoi(p_pemilik);

        insertItem(id, p_nama, kat, jml, p_lokasi, stat, own, p_PIC);
        // HAPUS CETAKAN SUKSES DARI SINI
        return;
    }

    if (strcmp(token, "SEARCH") == 0) {
        char* p_id = strtok(NULL, ";");
        if (!p_id) {
            uart_print_P(PSTR("Error: ID harus diisi!\n"));
            return;
        }

        uint16_t target_id = (uint16_t)atoi(p_id);
        Inventaris* match = NULL;
        searchItem(target_id, &match);

        if (match == NULL) {
            uart_print_P(PSTR("Error: Item dengan ID tersebut tidak ditemukan.\n"));
        } else {
            uart_print_P(PSTR("-> Item Ditemukan: \n"));
            uart_print_P(PSTR("   Nama    : ")); uart_print(match->nama);     uart_print_P(PSTR("\n"));
            uart_print_P(PSTR("   Jumlah  : ")); uart_print_num(match->jumlah); uart_print_P(PSTR("\n"));
            uart_print_P(PSTR("   Lokasi  : ")); uart_print(match->lokasi);   uart_print_P(PSTR("\n"));
        }
        return;
    }

    if (strcmp(token, "DELETE") == 0) {
        char* p_id = strtok(NULL, ";");
        if (!p_id) {
            uart_print_P(PSTR("Error: ID harus diisi!\n"));
            return;
        }

        uint16_t target_id = (uint16_t)atoi(p_id);
        deleteItem(target_id);
        // HAPUS CETAKAN SUKSES DARI SINI
        return;
    }

    // --- COMMAND 5: DUMP DATABASE ---
if (strcmp(token, "DUMP") == 0) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        Inventaris* curr = hashTable[i];
        while (curr != NULL) {
            uart_print_P(PSTR("DATA;"));
            uart_print_num(curr->id); uart_print_P(PSTR(";"));
            uart_print(curr->nama); uart_print_P(PSTR(";"));
            uart_print_num(curr->kategori); uart_print_P(PSTR(";"));
            uart_print_num(curr->jumlah); uart_print_P(PSTR(";"));
            uart_print(curr->lokasi); uart_print_P(PSTR(";"));
            uart_print_num(curr->status); uart_print_P(PSTR(";"));
            uart_print_num(curr->pemilik); uart_print_P(PSTR(";"));
            uart_print(curr->PIC); uart_print_P(PSTR("\n"));
            curr = curr->next;
        }
    }
    uart_print_P(PSTR("DUMP_DONE\n"));
    return;
}

    uart_print_P(PSTR("Error: Command tidak dikenali. Ketik 'MENU' untuk bantuan.\n"));
}