/*
Tugas Besar PMP K02 Kelompok 01
Nama file/function: inventory_db.h
Deskripsi file/function: File header yang berisi deklarasi struktur data, makro, dan fungsi-fungsi untuk manajemen database inventaris menggunakan hash table.
*/

#ifndef INVENTORY_DB_H
#define INVENTORY_DB_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <avr/pgmspace.h>

// Ukuran Hash Table
#define HASH_TABLE_SIZE 7

// Kategori Barang Inventaris
typedef enum {K_SENSOR, K_AKTUATOR, K_MIKON, K_INSTRUMEN} Kategori;

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

/*
Nama file/function: initNode
Deskripsi file/function: Fungsi untuk menginisialisasi node inventaris baru dengan nilai default.
*/
void initNode(Inventaris* node);

/*
Nama file/function: initHashTable
Deskripsi file/function: Fungsi untuk menginisialisasi array hash table dengan mengatur semua pointer menjadi NULL.
*/
void initHashTable(void);                           

/*
Nama file/function: initDB
Deskripsi file/function: Fungsi untuk menginisialisasi database secara keseluruhan beserta data awalnya.
*/
void initDB(void);                                  

/*
Nama file/function: hash_function
Deskripsi file/function: Fungsi hash untuk menghitung dan menentukan indeks suatu item di dalam hash table berdasarkan ID.
*/
void hash_function(uint16_t id, uint8_t* index);    

// Fungsi CRUD

/*
Nama file/function: insertItem
Deskripsi file/function: Fungsi untuk menambahkan item inventaris baru beserta detail informasinya ke dalam database.
*/
void insertItem(uint16_t id, const char* nama, uint8_t kategori, uint8_t jumlah, 
                const char* lokasi, uint8_t status, uint8_t pemilik, const char* PIC);

/*
Nama file/function: deleteItem
Deskripsi file/function: Fungsi untuk menghapus item inventaris dari database berdasarkan ID yang diberikan.
*/
void deleteItem(uint16_t id);

/*
Nama file/function: searchItem
Deskripsi file/function: Fungsi untuk mencari item dalam database berdasarkan ID dan mengembalikan hasilnya melalui pointer result.
*/
void searchItem(uint16_t id, Inventaris** result);

/*
Nama file/function: updateJumlah
Deskripsi file/function: Fungsi untuk mengubah jumlah stok suatu barang dalam database berdasarkan ID.
*/
void updateJumlah(uint16_t id, int16_t perubahan);

/*
Nama file/function: updateStatus
Deskripsi file/function: Fungsi untuk memperbarui status kondisi suatu barang dalam database berdasarkan ID.
*/
void updateStatus(uint16_t id, uint8_t newStatus);

/*
Nama file/function: dump
Deskripsi file/function: Fungsi untuk mencetak seluruh isi struktur database secara mentah untuk keperluan debugging.
*/
void dump(void);

/*
Nama file/function: show
Deskripsi file/function: Fungsi untuk menampilkan seluruh data inventaris dalam database secara terstruktur kepada pengguna.
*/
void show(void);

#endif // INVENTORY_DB_H
