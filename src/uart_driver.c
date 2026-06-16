#include "../include/uart_driver.h"
#include <avr/interrupt.h>

// Definisi CPU Clock untuk Kalkulasi Baud Rate Register
#define F_CPU 16000000UL

// Alokasi Penyimpanan Ring Buffer RX
static volatile char rx_buffer[UART_RX_BUFFER_SIZE];
static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;

// =========================================================================
// PRIMITIF 1: Inisialisasi Hardware Register USART & Interrupt
// =========================================================================
void uart_init(uint32_t baudrate) {
    // 1. Hitung Nilai Register UBRR (Baud Rate Generator)
    uint16_t ubrr_val = (uint16_t)((F_CPU / (16UL * baudrate)) - 1);
    UBRR0H = (uint8_t)(ubrr_val >> 8);
    UBRR0L = (uint8_t)(ubrr_val);

    // 2. Aktifkan Sirkuit Transmitter (TXEN0) dan Receiver (RXEN0)
    // CRITICAL: Aktifkan RX Complete Interrupt Enable (RXCIE0) agar ISR terpicu!
    UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);

    // 3. Konfigurasi Frame Format: 8 Data Bits, 1 Stop Bit, Tanpa Paritas
    UCSR0C = (3 << UCSZ00);

    // 4. Pastikan Global Interrupts Diaktifkan agar Mikrokontroler Bisa Merespon
    sei();
}

// =========================================================================
// INTERRUPT SERVICE ROUTINE (ISR): Terpicu Otomatis Tiap Kali 1 Byte Masuk ke Pin Rx
// =========================================================================
ISR(USART_RX_vect) {
    // Membaca karakter dari register I/O hardware UDR0 (Ini juga otomatis membersihkan interrupt flag)
    char received_char = UDR0;

    // Hitung posisi index berikutnya untuk menampung data di ring buffer
    uint8_t next_head = (uint8_t)((rx_head + 1) % UART_RX_BUFFER_SIZE);

    // Jika buffer belum penuh, simpan karakter baru dan geser head forward
    if (next_head != rx_tail) {
        rx_buffer[rx_head] = received_char;
        rx_head = next_head;
    }
    // Jika penuh, karakter terpaksa di-drop (Overflow Protection)
}

// =========================================================================
// PRIMITIF 2: Kirim Satu Karakter ke Hardware Wire (Polled/Blocking)
// =========================================================================
void uart_putchar(char c) {
    // Tunggu sampai buffer transmit hardware (UDRE0) kosong dan siap menerima data
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

// =========================================================================
// UTILITAS STRING: Kirim Untaian Teks Null-Terminated
// =========================================================================
void uart_print(const char* str) {
    while (*str) {
        // Otomatis ubah '\n' menjadi '\r\n' agar baris teks rapi di Serial Monitor laptop
        if (*str == '\n') {
            uart_putchar('\r');
        }
        uart_putchar(*str++);
    }
}

// =========================================================================
// UTILITAS ANGKA: Konversi Integer 16-bit ke String Secara Manual (0 Byte RAM Buffer)
// =========================================================================
void uart_print_num(uint16_t num) {
    if (num == 0) {
        uart_putchar('0');
        return;
    }

    char buf[5]; // Nilai maksimum uint16_t adalah 65535 (5 digit)
    int8_t i = 0;

    // Ekstrak angka digit demi digit dari belakang menggunakan operasi modulo
    while (num > 0) {
        buf[i++] = (char)('0' + (num % 10));
        num /= 10;
    }

    // Cetak digit secara terbalik agar urutannya benar di layar monitor
    for (int8_t j = (int8_t)(i - 1); j >= 0; j--) {
        uart_putchar(buf[j]);
    }
}
// =========================================================================
// UTILITAS STRING PROGMEM: Kirim Untaian Teks Null-Terminated Dari Progmem
// =========================================================================
void uart_print_p(const char* flash_str) {
    char c;
    // Read one byte at a time directly from the Flash memory bus
    while ((c = pgm_read_byte(flash_str++))) {
        if (c == '\n') {
            uart_putchar('\r');
        }
        uart_putchar(c);
    }
}

// =========================================================================
// UTILITAS PEMBACAAN 1: Ambil Jumlah Karakter Tersedia (Void Rule Compliance)
// =========================================================================
void uart_available(uint8_t* available) {
    // Menggunakan operasi bitwise math untuk menghitung jarak antara Head dan Tail
    *available = (uint8_t)((UART_RX_BUFFER_SIZE + rx_head - rx_tail) % UART_RX_BUFFER_SIZE);
}

// =========================================================================
// UTILITAS PEMBACAAN 2: Ambil Karakter Tertua (Blocking Read)
// =========================================================================
void uart_getchar(char* c) {
    // Tunggu di tempat (blocking) jika ring buffer dalam keadaan kosong murni
    while (rx_head == rx_tail);

    // Ambil karakter tertua dari posisi tail
    *c = rx_buffer[rx_tail];

    // Geser index tail maju secara melingkar (circular shift)
    rx_tail = (uint8_t)((rx_tail + 1) % UART_RX_BUFFER_SIZE);
}

// =========================================================================
// UTILITAS PEMBACAAN 3: Kosongkan Buffer secara Instan
// =========================================================================
void uart_flush(void) {
    // Menyamakan tail dengan head secara instan membatalkan semua indeks data tertunda
    rx_tail = rx_head;
}