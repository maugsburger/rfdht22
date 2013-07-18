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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> 
#define BAUD 9600
#include <util/setbaud.h>
#include <avr/pgmspace.h>
#include "lcd.h"
#include "wl_module.h"

#if F_CPU == 8000000 
    #define TCCR1DIVIDE (0<<CS12) | (1<<CS11) | (0<<CS10)
#elif F_CPU == 1000000 
    #define TCCR1DIVIDE (0<<CS12) | (0<<CS11) | (1<<CS10)
#else
    #error No supported CPU Freq
#endif

volatile uint8_t PTX;

int uart_putc(unsigned char c) {
    while (!(UCSR0A & (1<<UDRE0))){}  /* warten bis Senden moeglich */
    UDR0 = c;                      /* sende Zeichen */
    return 0;
}
 
/* puts ist unabhaengig vom Controllertyp */
void uart_puts (char *s) {
    while (*s) {   /* so lange *s != '\0' also ungleich dem "String-Endezeichen(Terminator)" */
        uart_putc(*s);
        s++;
    }
}

void dhtpwr( bool on ) {
    if( on ) {
        // PD6 == OC0A
        TCCR0A =    (1<<WGM01) | (1<<WGM00) | //
                    (1<<COM0A1); // Clear OC0A on match, set at TOP
        TCCR0B = (0<<CS01) | (1<<CS00); // no prescaling
        OCR0A = 127; 

        DDRD |= (1<<PD6);
    } else {
        // PB2 und PB1 kein Ausgang
        DDRD &= ~(1<<PD6);
        TCCR0A = (0<<WGM01) | (0<<WGM00);   //normal port operation
        TCCR0B = (0<<CS01) | (0<<CS00);     // stop counter
    }
}

void dhtpwrt(void) {
    if( DDRB & (1<<PB2) ) {
        dhtpwr(false);
    } else {
        dhtpwr(true);
    }
}

void lcd_itoa(uint8_t c) {
    char itoabuffer[9];
    itoa(c, itoabuffer, 2);
    for ( int i = strlen( itoabuffer ); i < 8; i++ ) {
        lcd_putc('0');
    } 
    lcd_puts(itoabuffer);
}

int main (void) {
    /* init serial */
    UCSR0B |= (1<<TXEN0);                // UART TX einschalten

    UCSR0C = (1 << UCSZ01)|(1 << UCSZ00); // Asynchron 8N1
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    #if USE_2X
        UCSR0A |= (1 << U2X0);
    #else
        UCSR0A &= ~(1 << U2X0);
    #endif
    uart_puts("\n\rrestart\n\r");

    dhtpwr(1);
    lcd_init(LCD_DISP_ON);
    lcd_clrscr();
    lcd_puts("HELLO\n");

    char itoabuffer[20];
	uint8_t payload[wl_module_PAYLOAD];		//holds the payload
	uint8_t nRF_status;						//STATUS information of nRF24L01+
	uint8_t zaehler = 0;
	wl_module_init();		//Init nRF Module
	_delay_ms(50);			//wait for Module
//	sei();					//activate Interrupts
	wl_module_config();		//config nRF as RX Module, simple Version
    
    while(1) {
//        lcd_puts("waiting");
		while (!wl_module_data_ready());			//waits for RX_DR Flag in STATUS
		nRF_status = wl_module_get_data(payload);	//reads the incomming Data to Array payload
		lcd_clrscr();
		lcd_home();
        /*          1         2         3         4
         * 1........0.........0.........0.........0
         * 0 xxxxxxxx 1 xxxxxxxx 2 xxxxxxxx
         * 3 xxxxxxxx 4 xxxxxxxx 5 xxxxxxxx
         */
        lcd_gotoxy(10,0);
        lcd_putc('A');
        lcd_itoa( payload[0] );

        lcd_puts("\n");

        lcd_putc('B');
        lcd_itoa( payload[1] );
        lcd_putc(' ');

        lcd_putc('C');
        lcd_itoa( payload[2] );

        lcd_puts("\n");

        lcd_putc('D');
        lcd_itoa( payload[3] );
        lcd_putc(' ');

        lcd_putc('E');
        lcd_itoa( payload[4] );

        lcd_puts("\n");
        uint16_t out;
        out = payload[0];
        lcd_putc('s');
        itoa(out, itoabuffer, 10);
        lcd_puts(itoabuffer);
        lcd_putc(' ');

        out = payload[1] | (payload[2]<<8);
        lcd_putc('t');
        itoa(out, itoabuffer, 10);
        lcd_puts(itoabuffer);
        lcd_putc(' ');

        out = payload[3] | (payload[4]<<8);
        lcd_putc('h');
        itoa(out, itoabuffer, 10);
        lcd_puts(itoabuffer);
        lcd_putc(' ');
    }
}
