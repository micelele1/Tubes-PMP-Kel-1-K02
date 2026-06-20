/*
Tugas Besar PMP K02 Kelompok 01
Nama file/function: uart_driver.c
Deskripsi file/function: File implementasi fungsi-fungsi driver UART untuk komunikasi serial, menggunakan metode ring buffer dan interrupt untuk penerimaan data.
*/

#include "../include/uart_driver.h"

// Definisi CPU Clock untuk Kalkulasi Baud Rate Register
#define F_CPU 16000000UL

// Alokasi Penyimpanan Ring Buffer RX
static volatile char rx_buffer[UART_RX_BUFFER_SIZE];
static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;

/*
Nama file/function: uart_init
Deskripsi file/function: Menginisialisasi register baud rate dan mengaktifkan fitur pengiriman (TX), penerimaan (RX), serta interupsi penerimaan UART.
*/
void uart_init(uint32_t baudrate) {
    uint16_t ubrr_val = (uint16_t)((F_CPU / (16UL * baudrate)) - 1);
    UBRR0H = (uint8_t)(ubrr_val >> 8);
    UBRR0L = (uint8_t)(ubrr_val);
    UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
    UCSR0C = (3 << UCSZ00);
    sei();
}

/*
Nama file/function: ISR(USART_RX_vect)
Deskripsi file/function: Rutinitas pelayanan interupsi (Interrupt Service Routine) yang dipicu setiap kali ada data masuk melalui UART untuk disimpan ke dalam ring buffer.
*/
ISR(USART_RX_vect) {
    char received_char = UDR0;
    uint8_t next_head = (uint8_t)((rx_head + 1) % UART_RX_BUFFER_SIZE);
    if (next_head != rx_tail) {
        rx_buffer[rx_head] = received_char;
        rx_head = next_head;
    }
}

/*
Nama file/function: uart_putchar
Deskripsi file/function: Mengirimkan satu karakter tunggal melalui UART dengan menunggu hingga buffer register pengiriman kosong.
*/
void uart_putchar(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

/*
Nama file/function: uart_print
Deskripsi file/function: Mengirimkan serangkaian karakter (string) dari memori SRAM satu per satu melalui UART.
*/
void uart_print(const char* str) {
    while (*str) {
        if (*str == '\n') {
            uart_putchar('\r');
        }
        uart_putchar(*str++);
    }
}

// FUNGSI INI SEBELUMNYA HILANG
/*
Nama file/function: uart_print_P
Deskripsi file/function: Membaca dan mengirimkan string yang disimpan di memori Flash (PROGMEM) untuk menghemat penggunaan RAM.
*/
void uart_print_P(const char* str) {
    char c;
    while ((c = (char)pgm_read_byte(str++))) {
        if (c == '\n') {
            uart_putchar('\r');
        }
        uart_putchar(c);
    }
}

/*
Nama file/function: uart_print_num
Deskripsi file/function: Mengonversi nilai bilangan bulat 16-bit menjadi format karakter string dan mengirimkannya secara berurutan melalui UART.
*/
void uart_print_num(uint16_t num) {
    if (num == 0) {
        uart_putchar('0');
        return;
    }
    char buf[5]; 
    int8_t i = 0;
    while (num > 0) {
        buf[i++] = (char)('0' + (num % 10));
        num /= 10;
    }
    for (int8_t j = (int8_t)(i - 1); j >= 0; j--) {
        uart_putchar(buf[j]);
    }
}

/*
Nama file/function: uart_available
Deskripsi file/function: Menghitung dan mengecek jumlah karakter yang tersedia dan belum dibaca di dalam ring buffer RX.
*/
void uart_available(uint8_t* available) {
    *available = (uint8_t)((UART_RX_BUFFER_SIZE + rx_head - rx_tail) % UART_RX_BUFFER_SIZE);
}

/*
Nama file/function: uart_getchar
Deskripsi file/function: Mengambil dan membaca satu karakter terlama dari ring buffer penerimaan (RX) berdasarkan prinsip FIFO.
*/
void uart_getchar(char* c) {
    while (rx_head == rx_tail);
    *c = rx_buffer[rx_tail];
    rx_tail = (uint8_t)((rx_tail + 1) % UART_RX_BUFFER_SIZE);
}

/*
Nama file/function: uart_flush
Deskripsi file/function: Mengosongkan secara paksa seluruh isi data sisa di dalam ring buffer RX dengan menyamakan posisi tail dengan head.
*/
void uart_flush(void) {
    rx_tail = rx_head;
}
