#include "inventory.h"

void cetakInfoBarang(short id) {
    int index = cariBarang(id);
    if (index != -1) {
        Barang b = inventory[index];
        Serial.println("--- Detail Barang ---");
        Serial.print("ID: "); Serial.println(b.id);
        Serial.print("Nama: "); Serial.println(b.nama);
        
        // Memanggil fungsi switch
        Serial.print("Kategori: "); Serial.println(getKategoriNama(b.kategori));
        
        Serial.print("Stok: "); Serial.println(b.jumlah_stok);
        
        // Memanggil fungsi switch
        Serial.print("Lokasi: "); Serial.println(getLokasiNama(b.lokasi));
        Serial.print("Status: "); Serial.println(getStatusNama(b.status));
        
        Serial.print("Pemilik: "); Serial.println(b.pemilik);
        Serial.print("PIC: "); Serial.println(b.pic);
        Serial.println("---------------------");
    } else {
        Serial.print("Gagal mencari: ID ");
        Serial.print(id);
        Serial.println(" tidak ditemukan.");
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000); // Waktu jeda agar serial monitor siap

    // 1. Uji coba menambah data
    // Kategori 3 (Mikrokontroler), Lokasi 1 (Lemari A), Status 1 (Tersedia)
    int res1 = tambahBarang(101, "ESP32 DevKit", 3, 15, 1, 1, "Laboratorium", "Adit");
    if (res1 == 1) Serial.println("Berhasil menambah barang ID 101.");

    // Kategori 4 (Sensor), Lokasi 2 (Laci Bawah), Status 2 (Dipinjam)
    tambahBarang(102, "Modul MPU6050", 4, 3, 2, 2, "Michelle", "Raehan");

    // 2. Uji coba penanganan ID duplikat
    int res2 = tambahBarang(101, "Komponen Duplikat", 1, 5, 1, 1, "Lab", "PIC");
    if (res2 == -1) Serial.println("Sistem menolak: ID 101 sudah digunakan (Duplikat).");

    // 3. Uji coba mencari dan mencetak data
    cetakInfoBarang(101);
    cetakInfoBarang(102);

    // 4. Uji coba mencari ID yang tidak ada
    cetakInfoBarang(500);

    // 5. Uji coba menghapus data
    int res3 = hapusBarang(101);
    if (res3 == 1) Serial.println("Barang ID 101 berhasil dihapus dari sistem.");

    // 6. Uji coba mencari barang yang sudah dihapus
    cetakInfoBarang(101);

    // 7. Uji coba menghapus ID yang tidak ada
    int res4 = hapusBarang(999);
    if (res4 == -1) Serial.println("Gagal menghapus: ID 999 tidak ditemukan di sistem.");
}

void loop() {
    // Biarkan kosong untuk contoh ini
}