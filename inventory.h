#ifndef INVENTORY_H
#define INVENTORY_H

#include <string.h>

#define MAX_ITEMS 50
#define STR_LEN 32

// Definisi tipe data inventaris
typedef struct {
    short id;
    char nama[STR_LEN];
    short kategori;
    int jumlah_stok;
    short lokasi;
    short status;
    char pemilik[STR_LEN];
    char pic[STR_LEN];
} Barang;

// Deklarasi variabel global statis
extern Barang inventory[MAX_ITEMS];
extern int itemCount;

// Prototipe Fungsi Algoritma
int tambahBarang(short id, const char* nama, short kategori, int stok, short lokasi, short status, const char* pemilik, const char* pic);
int hapusBarang(short id);
int cariBarang(short id);

// Prototipe Fungsi Switch Case
const char* getKategoriNama(short kategori);
const char* getLokasiNama(short lokasi);
const char* getStatusNama(short status);

#endif