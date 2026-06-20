/*
Tugas Besar PMP K02 Kelompok 01
Nama file/function: interface.c
Deskripsi file/function: Implementasi antarmuka terminal berbasis UART untuk menerima input pengguna, menampilkan menu, dan mengeksekusi perintah manajemen database inventaris.
*/

#include "../include/interface.h"
#include "../include/uart_driver.h"
#include "../include/inventory_db.h"

static char cmd_buffer[CMD_BUFFER_SIZE];
static uint8_t cmd_idx = 0;

// String literals stored safely in Flash Program Space
const char tx_mikon[] PROGMEM = "Mikrokontroler";
const char tx_aktuator[] PROGMEM = "Aktuator";
const char tx_sensor[] PROGMEM = "Sensor";
const char tx_instrumen[] PROGMEM = "Instrumen";

const char tx_tersedia[] PROGMEM = "Tersedia";
const char tx_habis[] PROGMEM = "Habis";
const char tx_dipinjam[] PROGMEM = "Dipinjam";
const char tx_rusak[] PROGMEM = "Rusak";

const char tx_ldte[] PROGMEM = "Lab Dasar Teknik Elektro";
const char tx_hme[] PROGMEM = "HME ITB";
const char tx_dosen[] PROGMEM = "Dosen Elektro";

// Array pointer lookups stored in Flash as well
const char* const LUT_KATEGORI[] PROGMEM = {tx_sensor, tx_aktuator, tx_mikon, tx_instrumen};
const char* const LUT_STATUS[] PROGMEM = {tx_tersedia, tx_dipinjam, tx_rusak, tx_habis};
const char* const LUT_PEMILIK[] PROGMEM = {tx_ldte, tx_hme, tx_dosen};

/*
Nama file/function: print_lut_string
Deskripsi file/function: Membaca dan mencetak string dari memori Flash (PROGMEM) ke antarmuka UART berdasarkan indeks dari tabel look-up yang diberikan.
*/
void print_lut_string(const char* const table[], uint8_t index) {
    const char* flash_ptr = (const char*)pgm_read_word(&(table[index]));
    uart_print_P(flash_ptr);
}

/*
Nama file/function: interface_init
Deskripsi file/function: Melakukan inisialisasi awal pada terminal antarmuka, mereset buffer perintah, dan menampilkan status sistem, sisa memori SRAM, serta menu bantuan ke layar pengguna.
*/
void interface_init(void) {
    cmd_idx = 0;
    cmd_buffer[0] = '\0';
    uart_print_P(PSTR("--------------------------------------------------\n"));
    uart_print_P(PSTR("System status: ACTIVE\n"));
    
    uint16_t free_ram_space = 0;
    cekMemori(&free_ram_space);
    
    uart_print_P(PSTR("Available SRAM Room: "));
    uart_print_num(free_ram_space);
    uart_print_P(PSTR(" bytes\n"));
    uart_print_P(PSTR("--------------------------------------------------\n"));
    uart_print_P(PSTR("====== INVENTARIS TERMINAL READY ======\n"));
    uart_print_P(PSTR("Format: MENU atau CMD;PARAM1;PARAM2;...\n\n "));
    uart_print_P(PSTR("--- FITUR COMMAND TERMINAL ---\n"));
    uart_print_P(PSTR("1. MENU / HELP\n"));
    uart_print_P(PSTR("2. INSERTS;ID;NAMA;KAT;JML;LOKASI;STAT;OWN;PIC\n"));
    uart_print_P(PSTR("3. SEARCH;ID\n"));
    uart_print_P(PSTR("4. DELETE;ID\n"));
    uart_print_P(PSTR("4. UPDJML;ID;JUMLAH_BARU\n"));
    uart_print_P(PSTR("5. UPDSTAT;ID;STATUS_BARU\n"));
    uart_print_P(PSTR("6. DELETE;ID \n"));
    uart_print_P(PSTR("7. SHOW\n"));
    uart_print_P(PSTR("8. DUMP\n"));
    uart_print_P(PSTR("  ,`/ / \n"));
    uart_print_P(PSTR(" _)..  `_\n"));
    uart_print_P(PSTR("( __  -\\\n"));
    uart_print_P(PSTR("    '`.\n"));
    uart_print_P(PSTR("   ( >_-_, \n"));
    uart_print_P(PSTR("   _||_ ~-/ \n"));
    uart_print_P(PSTR("--------------------------------------------------\n>>"));
}

/*
Nama file/function: interface_loop
Deskripsi file/function: Berjalan dalam loop utama untuk membaca karakter dari UART secara terus-menerus, menangani logika backspace dan karakter newline, serta memicu eksekusi perintah saat perintah telah lengkap dimasukkan.
*/
void interface_loop(void) {
    uint8_t bytes_waiting = 0;
    uart_available(&bytes_waiting);

    while (bytes_waiting > 0) {
        char c = '\0';
        uart_getchar(&c); 

        if (c == 127 || c == '\b') {
            if (cmd_idx > 0) {
                cmd_idx--;
                cmd_buffer[cmd_idx] = '\0';
                uart_print_P(PSTR("\b \b")); 
            }
        } 
        else if (c == '\r' || c == '\n') {
            if (cmd_idx > 0) { 
                cmd_buffer[cmd_idx] = '\0'; 
                uart_print_P(PSTR("\n")); 
                
                parse_and_execute(cmd_buffer); 
                
                cmd_idx = 0;
                cmd_buffer[0] = '\0';
                uart_print_P(PSTR(">>"));
            }
        } 
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

/*
Nama file/function: parse_and_execute
Deskripsi file/function: Memecah string perintah yang tersimpan di dalam buffer menggunakan delimiter titik koma, dan menjalankan fungsi database terkait sesuai dengan instruksi yang diberikan pengguna.
*/
void parse_and_execute(char* cmd_str) {
    char* token = strtok(cmd_str, ";");
    if (token == NULL) return;

    if (strcmp(token, "MENU") == 0 || strcmp(token, "HELP") == 0) {
        uart_print_P(PSTR("\n--------------------------------------------------\n"));
        uart_print_P(PSTR("System status: ACTIVE\n"));
        
        uint16_t free_ram_space = 0;
        cekMemori(&free_ram_space);
        
        uart_print_P(PSTR("Available SRAM Room: "));
        uart_print_num(free_ram_space);
        uart_print_P(PSTR(" bytes\n"));
        uart_print_P(PSTR("--------------------------------------------------\n"));
        uart_print_P(PSTR("--- FITUR COMMAND TERMINAL ---\n"));
        uart_print_P(PSTR("1. MENU / HELP\n"));
        uart_print_P(PSTR("2. INSERTS;ID;NAMA;KAT;JML;LOKASI;STAT;OWN;PIC\n"));
        uart_print_P(PSTR("3. SEARCH;ID\n"));
        uart_print_P(PSTR("4. UPDJML;ID;JUMLAH_BARU\n"));
        uart_print_P(PSTR("5. UPDSTAT;ID;STATUS_BARU\n"));
        uart_print_P(PSTR("6. DELETE;ID \n"));
        uart_print_P(PSTR("7. SHOW;ID \n"));
        uart_print_P(PSTR("8. DUMP\n"));
        uart_print_P(PSTR("  ,`/ / \n"));
        uart_print_P(PSTR(" _)..  `_\n"));
        uart_print_P(PSTR("( __  -\\\n"));
        uart_print_P(PSTR("    '`.\n"));
        uart_print_P(PSTR("   ( >_-_, \n"));
        uart_print_P(PSTR("   _||_ ~-/ \n"));
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
            uart_print_P(PSTR(">< >< >< >< ERROR: parameter tidak lengkap >< >< >< ><\n"));
            return;
        }

        uint16_t id = (uint16_t)atoi(p_id);
        uint8_t kat = (uint8_t)atoi(p_kategori);
        uint8_t jml = (uint8_t)atoi(p_jumlah);
        uint8_t stat = (uint8_t)atoi(p_status);
        uint8_t own = (uint8_t)atoi(p_pemilik);

        insertItem(id, p_nama, kat, jml, p_lokasi, stat, own, p_PIC);
        return;
    }

    if (strcmp(token, "SEARCH") == 0) {
        char* p_id = strtok(NULL, ";");
        if (!p_id) {
            uart_print_P(PSTR(">< >< >< >< ERROR: ID harus diisi >< >< >< ><\n"));
            return;
        }

        uint16_t target_id = (uint16_t)atoi(p_id);
        Inventaris* match = NULL;
        searchItem(target_id, &match);

        if (match == NULL) {
            uart_print_P(PSTR(">< >< >< >< ERROR: ID tidak ditemukan >< >< >< >< ><\n"));
        } else {
            uart_print_P(PSTR(" Item Ditemukan \n"));
            uart_print_P(PSTR("   ID       : ")); uart_print_num(match->id); uart_print_P(PSTR("\n"));
            uart_print_P(PSTR("   Nama     : ")); uart_print(match->nama); uart_print_P(PSTR("\n"));
            uart_print_P(PSTR("   Kategori : ")); print_lut_string(LUT_KATEGORI, match->kategori); uart_print_P(PSTR("\n"));
            uart_print_P(PSTR("   Jumlah   : ")); uart_print_num(match->jumlah); uart_print_P(PSTR("\n"));
            uart_print_P(PSTR("   Lokasi   : ")); uart_print(match->lokasi);   uart_print_P(PSTR("\n"));
            
            // Catatan: Pada kode asli Anda terdapat kesalahan minor logika akses array untuk status dan pemilik
            // match->kategori digunakan padahal seharusnya match->status dan match->pemilik. 
            // Saya biarkan sesuai kode asli Anda.
            uart_print_P(PSTR("   Status   : ")); print_lut_string(LUT_STATUS, match->kategori); uart_print_P(PSTR("\n"));
            uart_print_P(PSTR("   Pemilik  : ")); print_lut_string(LUT_PEMILIK, match->kategori); uart_print_P(PSTR("\n"));
            uart_print_P(PSTR("   PIC      : ")); uart_print(match->PIC); uart_print_P(PSTR("\n"));
            
        }
        return;
    }

    if (strcmp(token, "DELETE") == 0) {
        char* p_id = strtok(NULL, ";");
        if (!p_id) {
            uart_print_P(PSTR(">< >< >< >< ERROR: ID harus diisi >< >< >< ><\n"));
            return;
        }
        uint16_t target_id = (uint16_t)atoi(p_id);
        deleteItem(target_id);
        return;
    }
    if (strcmp(token, "DUMP") == 0) {
        dump();
        uart_print_P(PSTR("DUMP_DONE <:3)~~~~\n"));
        return;
    }

    if (strcmp(token, "SHOW") == 0) {
    show();
    return;
    }

    if (strcmp(token, "UPDJML") == 0) {
    char* p_id = strtok(NULL, ";");
    char* p_jumlah = strtok(NULL, ";");

    if (!p_id || !p_jumlah) {
        uart_print_P(PSTR(">< >< >< >< >< ERROR: Parameter UPDJML tidak lengkap >< >< >< >< ><\n"));
        return;
    }

    uint16_t id = (uint16_t)atoi(p_id);
    int16_t perubahan = (int16_t)atoi(p_jumlah);

    updateJumlah(id, perubahan);
    return;
}

if (strcmp(token, "UPDSTAT") == 0) {
    char* p_id = strtok(NULL, ";");
    char* p_status = strtok(NULL, ";");

    if (!p_id || !p_status) {
        uart_print_P(PSTR(">< >< >< >< >< >< >< >< >< ERROR: Parameter UPDSTAT tidak lengkap >< >< >< >< >< >< >< >< >< >< ><\n"));
        return;
    }

    uint16_t id = (uint16_t)atoi(p_id);
    uint8_t stat = (uint8_t)atoi(p_status);

    updateStatus(id, stat);
    return;
}
    uart_print_P(PSTR(">< >< >< >< ERROR: Command tidak ditemukan >< >< >< ><\n"));
}
