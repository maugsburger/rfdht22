#include "uart.h"

void uart_putc(unsigned char c) {
    while (!(UCSRA & (1<<UDRE))){}
    UDR = c;
}
 
 
void uart_puts (char *s) {
    while (*s) {
        uart_putc(*s);
        s++;
    }
}

void uart_init(void) {
    // init serial
    UCSRB |= (1<<TXEN);                // UART TX einschalten
    UCSRC = (1 << UCSZ1)|(1 << UCSZ0); // Asynchron 8N1
    UBRRH = UBRRH_VALUE;
    UBRRL = UBRRL_VALUE;
    #if USE_2X
        UCSRA |= (1 << U2X);
    #else
        UCSRA &= ~(1 << U2X);
    #endif
}
