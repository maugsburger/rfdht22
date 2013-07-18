/*
 * Copyright 2013 Moritz Augsburger <code@moritz.augsburger.name>
 *
 * This file is part of rfdht22.
 *
 * rfdht22 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
