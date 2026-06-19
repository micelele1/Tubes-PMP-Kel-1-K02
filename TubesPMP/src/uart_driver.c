#include "../include/uart_driver.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h> // TAMBAHAN PENTING

// Definisi CPU Clock untuk Kalkulasi Baud Rate Register
#define F_CPU 16000000UL

// Alokasi Penyimpanan Ring Buffer RX
static volatile char rx_buffer[UART_RX_BUFFER_SIZE];
static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;

void uart_init(uint32_t baudrate) {
    uint16_t ubrr_val = (uint16_t)((F_CPU / (16UL * baudrate)) - 1);
    UBRR0H = (uint8_t)(ubrr_val >> 8);
    UBRR0L = (uint8_t)(ubrr_val);
    UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
    UCSR0C = (3 << UCSZ00);
    sei();
}

ISR(USART_RX_vect) {
    char received_char = UDR0;
    uint8_t next_head = (uint8_t)((rx_head + 1) % UART_RX_BUFFER_SIZE);
    if (next_head != rx_tail) {
        rx_buffer[rx_head] = received_char;
        rx_head = next_head;
    }
}

void uart_putchar(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void uart_print(const char* str) {
    while (*str) {
        if (*str == '\n') {
            uart_putchar('\r');
        }
        uart_putchar(*str++);
    }
}

// FUNGSI INI SEBELUMNYA HILANG
void uart_print_P(const char* str) {
    char c;
    while ((c = (char)pgm_read_byte(str++))) {
        if (c == '\n') {
            uart_putchar('\r');
        }
        uart_putchar(c);
    }
}

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

void uart_available(uint8_t* available) {
    *available = (uint8_t)((UART_RX_BUFFER_SIZE + rx_head - rx_tail) % UART_RX_BUFFER_SIZE);
}

void uart_getchar(char* c) {
    while (rx_head == rx_tail);
    *c = rx_buffer[rx_tail];
    rx_tail = (uint8_t)((rx_tail + 1) % UART_RX_BUFFER_SIZE);
}

void uart_flush(void) {
    rx_tail = rx_head;
}