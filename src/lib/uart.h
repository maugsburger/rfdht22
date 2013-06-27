#define BAUD 9600
#include <util/setbaud.h>
#include <avr/io.h>
#include <stdint.h>

void uart_putc(unsigned char c);
void uart_puts(char *s);
void uart_init(void);
