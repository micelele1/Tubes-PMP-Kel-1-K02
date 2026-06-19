#ifndef INVENTORY_DB_H
#define INVENTORY_DB_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Ukuran Hash Table
#define HASH_TABLE_SIZE 7

// Definisi Flag Status Insert
#define INSERT_SUCCESS 0            // Insert Item Berhasil
#define INSERT_SUCESS_MOSTLY 1      // Insert Item Berhasil Namun Memori Hampir Penuh
#define INSERT_FAILED_DUP 2         // Insert Item Gagal Karena Duplikasi
#define INSERT_FAILED_FULL 3        // Insert Item Gagal Karena Memori Penuh
#define INSERT_FAILED_ERR 4         // Insert Item Gagal Karena Error Alokasi Memori

// Kategori Barang Inventaris
typedef enum {K_SENSOR, K_AKTUATOR, K_MIKON, K_KABEL, K_INSTRUMEN} Kategori;

// Status Barang Inventaris
typedef enum {S_TERSEDIA, S_DIPINJAM, S_RUSAK, S_HABIS} Status;

// Pemilik Barang Inventaris
typedef enum {P_LDTE, P_HME, P_DOSEN} Pemilik;

// Struktur Data Barang Inventaris
typedef struct NodeInventaris{
    uint16_t id;                    // ID Dari Kode QR (0-65535)
    char nama[6];                   // Nama Barang (max 5 karakter + 1 null terminator)
    uint8_t kategori;               // Kategori Barang (0-4)
    uint8_t jumlah;                 // Jumlah Barang (0-255)
    char lokasi [4];                // Lokasi Barang (max 3 karakter + 1 null terminator)
    uint8_t status;                 // Status Barang (0-3)
    uint8_t pemilik;                // Pemilik Barang (0-2)
    char PIC [5];                   // PIC Barang (max 4 karakter + 1 null terminator)
    struct NodeInventaris* next;    // Pointer ke Node Selanjutnya
} Inventaris;

// Deklarasi Fungsi Hash untuk Menentukan Index dalam Hash Table
extern Inventaris* hashTable[HASH_TABLE_SIZE];

// Fungsi Primitif
void initNode(Inventaris* node);
void initHashTable(void);                           // Inisialisasi Hash Table
void initDB(void);                                  // Inisialisasi Database
void hash_function(uint16_t id, uint8_t* index);    // Fungsi Hash untuk Menentukan Index dalam Hash Table
void cekMemori(uint16_t* memori);                   // Cek Besar Memori yang Tersisa

// Fungsi CRUD
// Menambahkan Item Baru ke Database
void insertItem(uint16_t id, const char* nama, uint8_t kategori, uint8_t jumlah, 
                const char* lokasi, uint8_t status, uint8_t pemilik, const char* PIC,
                uint8_t* statusFlag);

// Menghapus Item dari Database Berdasarkan ID
void deleteItem(uint16_t id);

// Mencari Item dalam Database Berdasarkan ID
void searchItem(uint16_t id, Inventaris** result);

// Mengupdate Jumlah Barang dalam Database Berdasarkan ID
void updateJumlah(uint16_t id, uint8_t newJumlah);

// Mengupdate Status Barang dalam Database Berdasarkan ID
void updateStatus(uint16_t id, uint8_t newStatus);

#endif // INVENTORY_DB_H