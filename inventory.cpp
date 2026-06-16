#include "inventory.h"

Barang inventory[MAX_ITEMS];
int itemCount = 0;

int tambahBarang(short id, const char* nama, short kategori, int stok, short lokasi, short status, const char* pemilik, const char* pic) {
    // Penanganan kasus: Cek ID duplikat
    for (int i = 0; i < itemCount; i++) {
        if (inventory[i].id == id) {
            return -1; // Kode error untuk ID duplikat
        }
    }

    if (itemCount >= MAX_ITEMS) {
        return -2; // Kode error untuk memori array penuh
    }

    // Proses menambah data
    inventory[itemCount].id = id;
    
    // Menggunakan strncpy agar memori lebih aman
    strncpy(inventory[itemCount].nama, nama, STR_LEN - 1);
    inventory[itemCount].nama[STR_LEN - 1] = '\0';
    
    inventory[itemCount].kategori = kategori;
    inventory[itemCount].jumlah_stok = stok;
    inventory[itemCount].lokasi = lokasi;
    inventory[itemCount].status = status;
    
    strncpy(inventory[itemCount].pemilik, pemilik, STR_LEN - 1);
    inventory[itemCount].pemilik[STR_LEN - 1] = '\0';
    
    strncpy(inventory[itemCount].pic, pic, STR_LEN - 1);
    inventory[itemCount].pic[STR_LEN - 1] = '\0';

    itemCount++;
    return 1; // Berhasil
}

int hapusBarang(short id) {
    int index = -1;
    
    // Cari index posisi data berdasarkan ID
    for (int i = 0; i < itemCount; i++) {
        if (inventory[i].id == id) {
            index = i;
            break;
        }
    }

    // Penanganan kasus: ID tidak ditemukan
    if (index == -1) {
        return -1; 
    }

    // Menggeser data di sebelah kanannya mundur ke kiri untuk menimpa data yang dihapus
    for (int i = index; i < itemCount - 1; i++) {
        inventory[i] = inventory[i + 1];
    }

    itemCount--;
    return 1; // Berhasil
}

int cariBarang(short id) {
    for (int i = 0; i < itemCount; i++) {
        if (inventory[i].id == id) {
            return i; // Mengembalikan index lokasi barang di array
        }
    }
    return -1; // Penanganan kasus: ID tidak ditemukan
}

// Proses switch untuk kategori
const char* getKategoriNama(short kategori) {
    switch(kategori) {
        case 1: return "Komponen Pasif";
        case 2: return "IC / Semikonduktor";
        case 3: return "Modul Mikrokontroler";
        case 4: return "Sensor";
        default: return "Tidak Diketahui";
    }
}

// Proses switch untuk lokasi penyimpanan
const char* getLokasiNama(short lokasi) {
    switch(lokasi) {
        case 1: return "Lemari Komponen A";
        case 2: return "Laci Bawah Meja";
        case 3: return "Gudang Peralatan";
        default: return "Tidak Diketahui";
    }
}

// Proses switch untuk status barang
const char* getStatusNama(short status) {
    switch(status) {
        case 1: return "Tersedia";
        case 2: return "Dipinjam";
        case 3: return "Rusak";
        case 4: return "Habis";
        default: return "Tidak Diketahui";
    }
}