#include "../include/inventory_db.h"
#include "../include/uart_driver.h"
#include <avr/pgmspace.h>

Inventaris *hashTable[HASH_TABLE_SIZE];

extern unsigned int __data_start;
extern unsigned int __bss_end;
extern unsigned char *__brkval;

void cekMemori(uint16_t* memoriFree){
    char top_of_stack;
    if(__brkval == 0){
        *memoriFree = (uint16_t)(&top_of_stack) - (uint16_t)(&__bss_end);
    }
    else{
        *memoriFree = (uint16_t)(&top_of_stack) - (uint16_t)(__brkval);
    }
}

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

void initDB(void){
    for (int i = 0; i < HASH_TABLE_SIZE; i++){
        hashTable[i] = NULL;
    }
}

void hashFunction(uint16_t id, uint8_t* index){
    *index = (uint8_t)(id % HASH_TABLE_SIZE);
}

void insertItem(uint16_t id, const char* nama, uint8_t kategori, uint8_t jumlah,
                const char* lokasi, uint8_t status, uint8_t pemilik, const char* PIC)
{
    uint8_t index;
    uint16_t memoriFree;
    hashFunction(id, &index);

    Inventaris* tesIdx = NULL;
    searchItem(id, &tesIdx);

    if(tesIdx != NULL){
        uart_print_P(PSTR("Error: Item dengan ID yang sama sudah ada!\n"));
        return;
    }

    cekMemori(&memoriFree);

    if (memoriFree < 350 + sizeof(Inventaris)){
        uart_print_P(PSTR("Error: Memori penuh, gagal menambahkan item!\n"));
        return;
    }
    else if (memoriFree < 500 + sizeof(Inventaris)){
        uart_print_P(PSTR("Peringatan: Memori mulai menipis.\n"));
    }

    Inventaris* newNode = (Inventaris*)malloc(sizeof(Inventaris));

    if (newNode == NULL){
        uart_print_P(PSTR("Error: Alokasi memori gagal!\n"));
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

    uart_print_P(PSTR("Sukses: Input diproses ke database.\n"));
}

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
            uart_print_P(PSTR("Sukses: Item berhasil dihapus.\n"));
            return;
        }
        prev = current;
        current = current->next;
    }

    uart_print_P(PSTR("Error: Item tidak ditemukan, gagal menghapus.\n"));
}

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

void updateJumlah(uint16_t id, uint8_t updateJumlah){
    Inventaris* item;
    searchItem(id, &item);

    if (item != NULL){
        item->jumlah = updateJumlah;
        uart_print_P(PSTR("Sukses: Jumlah item berhasil diperbarui.\n"));
    } else {
        uart_print_P(PSTR("Error: Item tidak ditemukan.\n"));
    }
}

void updateStatus(uint16_t id, uint8_t updateStatus){
    Inventaris* item;
    searchItem(id, &item);

    if (item != NULL){
        item->status = updateStatus;
        uart_print_P(PSTR("Sukses: Status item berhasil diperbarui.\n"));
    } else {
        uart_print_P(PSTR("Error: Item tidak ditemukan.\n"));
    }
}