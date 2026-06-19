#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> 

HANDLE hComm;
char input_cmd[100];
char respon_buffer[256];
char file_buffer[256];
DWORD bytesRead, bytesWritten;

void setup_serial(void);
void read_arduino(void);
void process_input_file(void);
void save_output_file(void);
void interface_loop(void);

void setup_serial(void) {
    hComm = CreateFileA("\\\\.\\COM8", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hComm == INVALID_HANDLE_VALUE) {
        printf("Error: Tidak dapat membuka COM8.\n");
        exit(1);
    }
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    GetCommState(hComm, &dcbSerialParams);
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    SetCommState(hComm, &dcbSerialParams);

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hComm, &timeouts);

    printf("Halo Pak Rahadian ^^ bentar ya Arduino sedang reboot\n");
    Sleep(2000);
    printf("Arduino di COM8\n");
    read_arduino();
}

void read_arduino(void) {
    while (1) {
        ReadFile(hComm, respon_buffer, sizeof(respon_buffer) - 1, &bytesRead, NULL);
        if (bytesRead > 0) {
            respon_buffer[bytesRead] = '\0';
            printf("%s", respon_buffer);
        } else {
            break;
        }
    }
}

void process_input_file(void) {
    FILE *file = fopen("inventory_input.txt", "r");
    if (file != NULL) {
        printf("Uploading file input inventory_input.txt ke Arduino\n");
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
        printf("\nUploaded!\n");
    } else {
        printf("Mana deh\n");
    }
}

void save_output_file(void) {
    printf("Sistem: Menyimpan data ke inventory_output.txt...\n");
    FILE *file = fopen("inventory_output.txt", "w");
    if (file == NULL) {
        printf("Error: Gagal membuat file output.\n");
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
                    printf("Sistem: Rekap data berhasil disimpan!\n");
                    return;
                }
                if (strncmp(line, "DATA;", 5) == 0) {
                    fprintf(file, "%s\n", line + 5);
                }
                line_idx = 0;
            } else {
                if (line_idx < 255) line[line_idx++] = c;
            }
        } else {
            break; 
        }
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
            printf("Menutup aplikasi...\n");
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