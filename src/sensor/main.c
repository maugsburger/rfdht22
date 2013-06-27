#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> 
#include "wl_module.h"
#include "uart.h"
#include "dht22.h"


void dhtpwr( bool on ) {
    if( on ) {
        // PB2 == OC0A
        TCCR0A =    (1<<WGM01) | (1<<WGM00) | //
                    (1<<COM0A1); // Clear OC0A on match, set at TOP
        TCCR0B = (0<<CS01) | (1<<CS00); // no prescaling
        OCR0A = 127; 

        // PB1 VCC für Doubler
        PORTB |= (1<<PB1);
        // PB2 und PB1 Ausgang
        DDRB |= (1<<PB2) | (1<<PB1); 
    } else {
        // PB2 und PB1 kein Ausgang
        DDRB &= ~(1<<PB2) | (1<<PB1); 
        PORTB &= ~(1<<PB1);
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


int main (void) {
    uart_init();
    uart_puts("\n\rrestart\n\r");

    uint16_t temperature;
    uint16_t humidity;
    uint8_t  dht22res;

    dhtpwr(true);

    PORTA = (1<<PA1) | (1<<PA0);
    DDRA  = (1<<PA0) | (1<<PA1);
    DDRD &= ~(1<<PD6); //PD6 als Eingang
    _delay_ms(500); 

    char s[7];

    while(1) {
        PORTA = (1<<PA1);
        dht22res = dhtmeasure(&temperature, &humidity );
        uart_puts("return ");
        uart_puts( utoa( dht22res, s, 10) );
        uart_puts("\n\r");
        uart_puts("hum ");
        uart_puts( utoa( humidity, s, 10) );
        uart_puts("\n\r");
        uart_puts("temp ");
        uart_puts( utoa( temperature, s, 10) );
        uart_puts("\n\r");
        PORTA = 0;
        _delay_ms(10000); 
    }
}
/*
static volatile uint8_t zahl[5];

        asm (   "sec"       "\n\t"
                "rol %0"    "\n\t"
                "rol %1"    "\n\t"
                "rol %2"    "\n\t"
                "rol %3"    "\n\t"
                "rol %4"
                :"+r"( zahl[4] ),
                 "+r"( zahl[3] ),
                 "+r"( zahl[2] ),
                 "+r"( zahl[1] ),
                 "+r"( zahl[0] ) );

=> 26 zyklen
*/
