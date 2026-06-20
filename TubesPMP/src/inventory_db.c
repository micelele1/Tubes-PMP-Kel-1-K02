/*
Tugas Besar PMP K02 Kelompok 01
Nama file/function: inventory_db.c
Deskripsi file/function: File implementasi fungsi-fungsi manajemen database inventaris berbasis hash table, termasuk operasi CRUD dan pemantauan kapasitas memori SRAM.
*/

#include "../include/inventory_db.h"
#include "../include/uart_driver.h"

Inventaris *hashTable[HASH_TABLE_SIZE];

extern unsigned int __data_start;
extern unsigned int __bss_end;
extern unsigned char *__brkval;

/*
Nama file/function: cekMemori
Deskripsi file/function: Fungsi untuk menghitung dan mengecek sisa ruang memori SRAM (antara heap dan stack) yang masih tersedia pada mikrokontroler.
*/
void cekMemori(uint16_t* memoriFree){
    char top_of_stack;
    if(__brkval == 0){
        *memoriFree = (uint16_t)(&top_of_stack) - (uint16_t)(&__bss_end);
    }
    else{
        *memoriFree = (uint16_t)(&top_of_stack) - (uint16_t)(__brkval);
    }
}

/*
Nama file/function: initNode
Deskripsi file/function: Fungsi untuk mengosongkan atau memberikan nilai default awal pada sebuah node inventaris.
*/
void initNode(Inventaris* node){
    node->id = 0;
    node->nama[0] = '\0';
    node->kategori = 0;
    node->jumlah = 0;
    node->lokasi[0] = '\0';
    node->status = 0;
    node->pemilik = 0;
    node->PIC[0] = '\0';
    node->next = NULL;
}

/*
Nama file/function: initDB
Deskripsi file/function: Fungsi untuk menginisialisasi database dengan mengatur semua pointer dalam array hash table menjadi NULL.
*/
void initDB(void){
    for (int i = 0; i < HASH_TABLE_SIZE; i++){
        hashTable[i] = NULL;
    }
}

/*
Nama file/function: hashFunction
Deskripsi file/function: Fungsi hash menggunakan operasi modulo untuk menentukan indeks penempatan data berdasarkan ID barang.
*/
void hashFunction(uint16_t id, uint8_t* index){
    *index = (uint8_t)(id % HASH_TABLE_SIZE);
}

/*
Nama file/function: insertItem
Deskripsi file/function: Fungsi untuk mengalokasikan memori dan menambahkan data barang baru ke dalam hash table, dilengkapi proteksi duplikasi ID dan pengecekan batas aman memori SRAM.
*/
void insertItem(uint16_t id, const char* nama, uint8_t kategori, uint8_t jumlah,
                const char* lokasi, uint8_t status, uint8_t pemilik, const char* PIC)
{
    uint8_t index;
    uint16_t memoriFree;
    hashFunction(id, &index);

    Inventaris* tesIdx = NULL;
    searchItem(id, &tesIdx);

    if(tesIdx != NULL){
        uart_print_P(PSTR(">< >< >< >< >< ERROR: : Item dengan ID yang sama sudah ada! >< >< >< >< \n"));
        return;
    }

    cekMemori(&memoriFree);

    if (memoriFree < 350 + sizeof(Inventaris)){
        uart_print_P(PSTR("ERROR: : memori sudah penuh!\n"));
        return;
    }
    else if (memoriFree < 500 + sizeof(Inventaris)){
        uart_print_P(PSTR("Peringatan!! Memori mulai menipis.\n"));
    }

    Inventaris* newNode = (Inventaris*)malloc(sizeof(Inventaris));

    if (newNode == NULL){
        uart_print_P(PSTR(">< >< >< >< ERROR: : alokasi memori gagal >< >< >< >< ><\n"));
        return;
    }

    newNode->id = id;
    newNode->kategori = kategori;
    newNode->jumlah = jumlah;
    newNode->status = status;
    newNode->pemilik = pemilik;

    strncpy(newNode->nama, nama, sizeof(newNode->nama) - 1);
    newNode->nama[sizeof(newNode->nama)-1] = '\0';
    strncpy(newNode->lokasi, lokasi, sizeof(newNode->lokasi) - 1);
    newNode->lokasi[sizeof(newNode->lokasi)-1] = '\0';
    strncpy(newNode->PIC, PIC, sizeof(newNode->PIC) - 1);
    newNode->PIC[sizeof(newNode->PIC)-1] = '\0';

    newNode->next = hashTable[index];
    hashTable[index] = newNode;

    uart_print_P(PSTR("Input berhasil diproses\n"));
}

/*
Nama file/function: deleteItem
Deskripsi file/function: Fungsi untuk mencari dan menghapus node barang dari linked list dalam hash table berdasarkan ID, serta membebaskan (free) memori yang digunakan node tersebut.
*/
void deleteItem(uint16_t id){
    uint8_t index;
    hashFunction(id, &index);

    Inventaris* current = hashTable[index];
    Inventaris* prev = NULL;

    while (current != NULL){
        if (current->id == id){
            if(prev == NULL){
                hashTable[index] = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            uart_print_P(PSTR("-----------8<---Item berhasil dihapus----------\n"));
            return;
        }
        prev = current;
        current = current->next;
    }

    uart_print_P(PSTR(">< >< >< >< >< ERROR: : Item tidak ditemukan, gagal menghapus. >< >< >< >< \n"));
}

/*
Nama file/function: searchItem
Deskripsi file/function: Fungsi untuk mencari item spesifik di dalam hash table menggunakan ID dan mengembalikan pointernya melalui parameter referensi (result).
*/
void searchItem(uint16_t id, Inventaris** result){
    uint8_t index;
    hashFunction(id, &index);

    Inventaris* current = hashTable[index];
    *result = NULL;

    while (current != NULL){
        if (current->id == id){
            *result = current;
            return;
        }
        current = current->next;
    }
}

/*
Nama file/function: updateJumlah
Deskripsi file/function: Fungsi untuk menambah atau mengurangi stok barang berdasarkan ID, dilengkapi validasi untuk mencegah nilai jumlah barang menjadi negatif akibat pengurangan.
*/
void updateJumlah(uint16_t id, int16_t perubahan){
Inventaris* item;
searchItem(id, &item);

if (item != NULL){
    if (perubahan < 0 && item->jumlah < (uint8_t)(-perubahan)){
        uart_print_P(PSTR(">< >< >< >< >< ERROR: Stok tidak mencukupi untuk dipinjam >< >< >< >< ><\n"));
    } else {
        item->jumlah = (uint8_t)(item->jumlah + perubahan);
        uart_print_P(PSTR("Sukses <('-'<) ^('-')^ (>'-')> Jumlah item berhasil diperbarui.\n"));
    }
} else {
    uart_print_P(PSTR(">< >< >< >< >< ERROR: : Item tidak ditemukan >< >< >< >< ><\n"));
}}

/*
Nama file/function: updateStatus
Deskripsi file/function: Fungsi untuk memperbarui flag status barang (seperti tersedia, rusak, dipinjam, dll) dalam database berdasarkan pencarian ID.
*/
void updateStatus(uint16_t id, uint8_t updateStatus){ //blm ad di interface
    Inventaris* item;
    searchItem(id, &item);

    if (item != NULL){
        item->status = updateStatus;
        uart_print_P(PSTR("Sukses <('-'<) ^('-')^ (>'-')> Status item berhasil diperbarui.\n"));
    } else {
        uart_print_P(PSTR(">< >< >< >< >< >< >< >< >< ERROR: : Item tidak ditemukan >< >< >< >< >< >< >< >< >< >< ><\n"));
    }
}

/*
Nama file/function: dump
Deskripsi file/function: Fungsi untuk mencetak seluruh isi database node demi node dalam satu baris log terformat menggunakan pemisah titik koma untuk kebutuhan debugging.
*/
void dump(){
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
}

/*
Nama file/function: show
Deskripsi file/function: Fungsi untuk mengumpulkan, menghitung, dan menampilkan ringkasan statistik database secara interaktif mencakup total barang, persebaran kategori, status, alokasi rak, serta estimasi sisa kapasitas item di RAM.
*/
void show(void) {
    uint16_t total_data = 0;
    uint16_t kat_counts[5] = {0, 0, 0, 0, 0};
    uint16_t stat_counts[4] = {0, 0, 0, 0};
    uint16_t rak_counts[26] = {0};

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        Inventaris* curr = hashTable[i];
        while (curr != NULL) {
            total_data++;
            
            if (curr->kategori <= 4) kat_counts[curr->kategori]++;
            if (curr->status <= 3) stat_counts[curr->status]++;

            char rak = curr->lokasi[0];
            if (rak >= 'A' && rak <= 'Z') {
                rak_counts[rak - 'A']++;
            }

            curr = curr->next;
        }
    }

    uint16_t mem_free = 0;
    cekMemori(&mem_free);
    mem_free = mem_free - 350;
    uint16_t max_items = mem_free / sizeof(Inventaris);

    uart_print_P(PSTR("\n~^~^~^ STATISTIK INVENTARIS ~^~^~^\n"));
    uart_print_P(PSTR("Total Item : ")); uart_print_num(total_data); uart_print_P(PSTR("\n"));

    uart_print_P(PSTR("\n~^~^~^ STATUS ~^~^~^\n"));
    uart_print_P(PSTR("Tersedia   : ")); uart_print_num(stat_counts[0]); uart_print_P(PSTR("\n"));
    uart_print_P(PSTR("Dipinjam   : ")); uart_print_num(stat_counts[1]); uart_print_P(PSTR("\n"));
    uart_print_P(PSTR("Rusak      : ")); uart_print_num(stat_counts[2]); uart_print_P(PSTR("\n"));
    uart_print_P(PSTR("Habis      : ")); uart_print_num(stat_counts[3]); uart_print_P(PSTR("\n"));

    uart_print_P(PSTR("\n~^~^~^ KATEGORI ~^~^~^\n"));
    uart_print_P(PSTR("Sensor     : ")); uart_print_num(kat_counts[0]); uart_print_P(PSTR("\n"));
    uart_print_P(PSTR("Aktuator   : ")); uart_print_num(kat_counts[1]); uart_print_P(PSTR("\n"));
    uart_print_P(PSTR("Mikon      : ")); uart_print_num(kat_counts[2]); uart_print_P(PSTR("\n"));
    uart_print_P(PSTR("Kabel      : ")); uart_print_num(kat_counts[3]); uart_print_P(PSTR("\n"));
    uart_print_P(PSTR("Instrumen  : ")); uart_print_num(kat_counts[4]); uart_print_P(PSTR("\n"));

    uart_print_P(PSTR("\n~^~^~^ LOKASI (PER RAK) ~^~^~^\n"));
    for (int i = 0; i < 26; i++) {
        if (rak_counts[i] > 0) {
            uart_print_P(PSTR("Rak "));
            uart_putchar('A' + i);
            uart_print_P(PSTR("      : "));
            uart_print_num(rak_counts[i]);
            uart_print_P(PSTR(" item\n"));
        }
    }

    uart_print_P(PSTR("\n~^~^ MEMORI SRAM ~^~^\n"));
    uart_print_P(PSTR("Sisa Memori: ")); uart_print_num(mem_free); uart_print_P(PSTR(" byte\n"));
    uart_print_P(PSTR("Kapasitas  : Muat sekitar ")); uart_print_num(max_items); uart_print_P(PSTR(" item lagi\n"));
    uart_print_P(PSTR("~^~^~^~^~^~^~^~^\n"));
}
