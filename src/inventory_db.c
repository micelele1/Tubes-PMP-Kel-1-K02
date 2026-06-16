#include "../include/inventory_db.h"

// Inisialisasi Hash Table
Inventaris *hashTable[HASH_TABLE_SIZE];

// Compiler Internal AVR Untuk Menghitung Memori yang Tersisa
extern unsigned int __data_start;
extern unsigned int __bss_end;
extern unsigned char *__brkval;

// Primitif Utilitas: Cek Memori yang Tersisa
void cekMemori(uint16_t* memoriFree){
    char top_of_stack;      // Variabel Lokal untuk Menentukan Posisi Top of Stack
    if(__brkval == 0){      // Jika Belum Ada Alokasi Dinamis, Gunakan Akhir BSS
        *memoriFree = (uint16_t)(&top_of_stack) - (uint16_t)(&__bss_end);
    }
    else{                   // Jika Sudah Ada Alokasi Dinamis, Gunakan Akhir Heap
        *memoriFree = (uint16_t)(&top_of_stack) - (uint16_t)(__brkval);
    }

}

// Primitif 1: Inisialisasi Node Inventaris
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

// Primitif 2: Inisialisasi Hash Table
void initDB(void){
    for (int i = 0; i < HASH_TABLE_SIZE; i++){
        hashTable[i] = NULL;
    }
}

// Primitif 3: Fungsi Hash untuk Menentukan Index dalam Hash Table
void hashFunction(uint16_t id, uint8_t* index){
    *index = (uint8_t)(id % HASH_TABLE_SIZE);
}

// CRUD 1: Menambahkan Item Baru ke Database
void insertItem(uint16_t id, const char* nama, uint8_t kategori, uint8_t jumlah,
                const char* lokasi, uint8_t status, uint8_t pemilik, const char* PIC)
{
    uint8_t index;          // Variabel Lokal untuk Menyimpan Index Hash
    uint16_t memoriFree;    // Variabel Lokal untuk Menyimpan Memori yang Tersisa
    hashFunction(id, &index); // Hitung Index Hash untuk ID yang Diberikan      
    
    // Cek Duplikasi ID dalam Hash Table
    Inventaris* tesIdx = NULL;
    searchItem(id, &tesIdx);
    
    if(tesIdx != NULL){
        // ITEM DENGAN ID YANG SAMA SUDAH ADA
        // Print: ERROR ITEM DENGAN ID YANG SAMA SUDAH ADA
        return;
    }

    // Cek Memori yang Tersisa Sebelum Alokasi Dinamis
    cekMemori(&memoriFree); // Cek Memori yang Tersisa Sebelum Alokasi Dinamis

    if (memoriFree < 350 + sizeof(Inventaris)){
        // MEMORI TIDAK CUKUP UNTUK MENAMBAHKAN ITEM BARU
        // Print: ERROR MEMORI PENUH
        return;
    }
    else if (memoriFree < 500 + sizeof(Inventaris)){
        // MEMORI MULAI MENIPIS, PERINGATAN UNTUK MENGOSONGKAN DATABASE
        // Print: PERINGATAN MEMORI MULAI MENIPIS
        // Sisa: (memoriFree - 350) / sizeof(Inventaris) item lagi yang bisa ditambahkan sebelum penuh
    }

    // Alokasi Dinamis untuk Node Inventaris Baru
    Inventaris* newNode = (Inventaris*)malloc(sizeof(Inventaris));

    // Cek Alokasi Memori Berhasil atau Tidak
    if (newNode == NULL){
        // Gagal Alokasi Memori
        // Print: ERROR ALOKASI MEMORI
        return; 
    }
    
    // Inisialisasi Data Node Baru
    newNode->id = id;
    newNode->kategori = kategori;
    newNode->jumlah = jumlah;
    newNode->status = status;
    newNode->pemilik = pemilik;

    strncpy(newNode->nama, nama, sizeof(newNode->nama) - 1);
    newNode->nama[sizeof(newNode->nama)-1] = '\0';              // Pastikan Null Terminator
    strncpy(newNode->lokasi, lokasi, sizeof(newNode->lokasi) - 1);
    newNode->lokasi[sizeof(newNode->lokasi)-1] = '\0';          // Pastikan Null Terminator
    strncpy(newNode->PIC, PIC, sizeof(newNode->PIC) - 1);
    newNode->PIC[sizeof(newNode->PIC)-1] = '\0';                // Pastikan Null Terminator

    // Insert Node Baru ke Hash Table (Insert di Awal Linked List)
    newNode->next = hashTable[index];   // Insert di Awal Linked List
    hashTable[index] = newNode;         // Update Head Linked List di Hash Table
}

// CRUD 2: Hapus Item dari Database Berdasarkan ID
void deleteItem(uint16_t id){
    uint8_t index;
    hashFunction(id, &index); // Hitung Index Hash untuk ID yang Diberikan

    Inventaris* current = hashTable[index];
    Inventaris* prev = NULL;

    while (current != NULL){
        if (current->id == id){
            // ITEM DITEMUKAN, HAPUS NODE

            // KASUS 1: Node Awal Linked List
            if(prev == NULL){
                hashTable[index] = current->next; //Update Head Linked List
            }

            // KASUS 2: Node Tengah atau Akhir Linked List
            else{
                prev->next = current->next; 
            }

            free(current);  // Bebaskan Memori Node yang Dihapus

            // ITEM BERHASIL DIHAPUS
            // Print: ITEM BERHASIL DIHAPUS

            return;
        }
        prev = current;
        current = current->next;
    }

    // ITEM TIDAK DITEMUKAN
    // Print: ERROR ITEM TIDAK DITEMUKAN
}

// CRUD 3: Cari Item dalam Database Berdasarkan ID
void searchItem(uint16_t id, Inventaris** result){
    uint8_t index;
    hashFunction(id, &index); // Hitung Index Hash untuk ID yang Diberikan

    Inventaris* current = hashTable[index];
    *result = NULL; // Inisialisasi Hasil Pencarian

    while (current != NULL){
        if (current->id == id){
            *result = current;
            return;
        }
        current = current->next;
    }
}

// Tambahan 1: Memperbarui Jumlah Item dalam Database Berdasarkan ID
void updateJumlah(uint16_t id, uint8_t updateJumlah){
    Inventaris* item;
    searchItem(id, &item); // Cari Item Berdasarkan ID

    if (item != NULL){
        item->jumlah = updateJumlah;
        // Jumlah Item Berhasil Diperbarui
        // Print: JUMLAH ITEM BERHASIL DIPERBARUI
    }
    else{
        // ITEM TIDAK DITEMUKAN
        // Print: ERROR ITEM TIDAK DITEMUKAN
    }
}

// Tambahan 2: Memperbarui Status Item dalam Database Berdasarkan ID
void updateStatus(uint16_t id, uint8_t updateStatus){
    Inventaris* item;
    searchItem(id, &item); // Cari Item Berdasarkan ID

    if (item != NULL){
        item->status = updateStatus;
        // Status Item Berhasil Diperbarui
        // Print: STATUS ITEM BERHASIL DIPERBARUI
    }
    else{
        // ITEM TIDAK DITEMUKAN
        // Print: ERROR ITEM TIDAK DITEMUKAN 
    }
}