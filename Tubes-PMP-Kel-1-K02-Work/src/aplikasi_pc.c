/*
TUGAS BESAR EL2008 KELAS 02 KELOMPOK 01
NAMA FILE: aplikasi_pc.c
Deskripsi: untuk koneksi antara PC atau laptop dengan arduino,
            upload data awal, menjalankan interface, mendapatkan perintah untuk arduino, dan
            menuliskan hasil update dara dari arduino ke file output dengan perintah DUMP atau EXIT*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> //windows API untuk connect sama OS biar bisa atur sleep, baudrate, sama fungsi low level

HANDLE hComm; //biar arduino bisa komunikasi sama OS windows
char input_cmd[100]; //buffer perintah terminal
char respon_buffer[256]; //buffer respon arduino
char file_buffer[256]; //buffer untuk file
DWORD bytesRead, bytesWritten; //untuk data yang keluar masuk dari arduino ke pc

void setup_serial(void);
void read_arduino(void);
void process_input_file(void);
void save_output_file(void);
void interface_loop(void);

void setup_serial(void) {
    hComm = CreateFileA("\\\\.\\COM8", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    //harus disesuaikan kalau ada perubahan COM
    if (hComm == INVALID_HANDLE_VALUE) {
        printf("⁴⁰⁴ Error ⁴⁰⁴: Tidak dapat membuka COM8.\n");
        exit(1);
    }
    DCB dcbSerialParams = {0}; //bikin struktur Device Control Block kosong untuk menampung pengaturan komunikasi hardware
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams); //kasih tau OS ukuran memori struktur DCB
    GetCommState(hComm, &dcbSerialParams); //ambil pengaturan bawaan port COM dari Windows ke variabel dsbserialparams
    dcbSerialParams.BaudRate = CBR_9600; // samain baudratenya dengan arduino
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT; //1 bit untuk penanda bahwa "paket" bytenya tuh udh sampai sana aja
    dcbSerialParams.Parity = NOPARITY;
    SetCommState(hComm, &dcbSerialParams);//diaturin ke com8 (atau com manapun yang arduinonya dikoneksikan)

    COMMTIMEOUTS timeouts = {0};//batas waktu tunggu maksimum program
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hComm, &timeouts);//biar terminal ga freeze kalau arduinonya mati

    printf("Halo Pak Rahadian ^^ bentar ya Arduino sedang reboot\n");
    printf("=^..^=    =^..^=    =^..^=   Rebooting for 2 seconds    .=^..^=   =^..^=   =^..^=\n");
    Sleep(2000);
    printf("Arduino di COM8 :-D\n");
    read_arduino();
}

void read_arduino(void) { //baca respon dari arduino
    while (1) {
        ReadFile(hComm, respon_buffer, sizeof(respon_buffer) - 1, &bytesRead, NULL);
        if (bytesRead > 0) {
            respon_buffer[bytesRead] = '\0';//biar data mentah terdaftar sebagai sebuah kalimat string utuh
            printf("%s", respon_buffer);//print ke terminal biar user tau
        } else {
            break;
        }
    }
}

void process_input_file(void) { //upload file input dari lokal komputer ke 
    FILE *file = fopen("inventory_input.txt", "r");
    if (file != NULL) {
        printf("Uploading file input inventory_input.txt ke Arduino <(-^,^-)=b\n");
        while (fgets(file_buffer, sizeof(file_buffer), file)) {
            file_buffer[strcspn(file_buffer, "\n")] = 0; 
            if (strlen(file_buffer) > 2) { 
                strcat(file_buffer, "\n");
                WriteFile(hComm, file_buffer, strlen(file_buffer), &bytesWritten, NULL);
                Sleep(150); 
            }
        }
        fclose(file);
        read_arduino(); 
        printf("\n<(-^,^-)=b Uploaded! <(-^,^-)=b\n");
    } else {
        printf(">< >< >< >< >< >< >< >< >< ERROR: kok ga ada input >< >< >< >< >< >< >< >< >< >< ><\n");
    }
}

void save_output_file(void) {
    printf("Uploading data ke file output inventory_output.txt\n");
    FILE *file = fopen("inventory_output.txt", "w");
    if (file == NULL) {
        printf(">< >< >< >< >< >< >< >< >< ERROR: gagal membuat file output >< >< >< >< >< >< >< >< >< >< ><\n");
        return;
    }
    fprintf(file, "ID;NAMA;KAT;JML;LOKASI;STAT;OWN;PIC\n");

    char cmd[] = "DUMP\n";
    WriteFile(hComm, cmd, strlen(cmd), &bytesWritten, NULL);

    char line[256];
    int line_idx = 0;
    
    while (1) {
        char c;
        ReadFile(hComm, &c, 1, &bytesRead, NULL);
        if (bytesRead > 0) {
            if (c == '\r') continue;
            if (c == '\n') {
                line[line_idx] = '\0';
                if (strcmp(line, "DUMP_DONE") == 0) {
                    fclose(file);
                    printf("yey! Rekap data berhasil disimpan!\n");
                    return;
                }
                if (strncmp(line, "DATA;", 5) == 0) {
                    fprintf(file, "%s\n", line + 5);
                }
                line_idx = 0;
            } else {
                if (line_idx < 255) line[line_idx++] = c;}
        }else {
            break; }
    }
    fclose(file);
}

void interface_loop(void) {
    while (1) {
        printf("\n=== TERMINAL INVENTARIS ===\n");
        printf("Ketik perintah (contoh: MENU) atau 'EXIT' untuk keluar: ");
        
        fgets(input_cmd, sizeof(input_cmd), stdin);
        input_cmd[strcspn(input_cmd, "\n")] = 0; 

        if (strcmp(input_cmd, "EXIT") == 0) {
            save_output_file();
            printf("Menutup aplikasi (^_^)\n");
            break;
        }

        strcat(input_cmd, "\n");
        WriteFile(hComm, input_cmd, strlen(input_cmd), &bytesWritten, NULL);

        Sleep(300); 
        read_arduino();
    }
}

int main(void) {
    setup_serial();
    process_input_file();
    interface_loop();
    CloseHandle(hComm);
    return 0;
}