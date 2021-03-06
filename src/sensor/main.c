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

#include "main.h"
volatile uint8_t isr_interval;

void dhtpwr_on() {
        // PB2 == OC0A
        TCCR0A =    (1<<WGM01) | (1<<WGM00) | //
                    (1<<COM0A1); // Clear OC0A on match, set at TOP
        TCCR0B = (0<<CS01) | (1<<CS00); // no prescaling
        OCR0A = 127;

        // PB2 und PB0 Ausgang
        DDRB |= (1<<PB2) | (1<<PB0);
        // PB0 VCC für Doubler
        PORTB |= (1<<PB0);
}

void dhtpwr_off() {
        // PB2 und PB1 kein Ausgang
        PORTB &= ~(1<<PB0);
        DDRB &= ~(1<<PB2) | (1<<PB0);
        TCCR0A = (0<<WGM01) | (0<<WGM00);
        TCCR0B = (0<<CS01) | (0<<CS00);     // stop counter
}

void sleep_time(uint8_t sleepmode, uint8_t intervals ) {
    wdt_reset();
    isr_interval = intervals;
    WDTCR = (1<<WDIF) | (1<<WDIE) | (1<<WDP3) | (1<<WDP0);
    while(isr_interval > 0) {
        set_sleep_mode(sleepmode);
        sleep_mode();
    }
    wdt_disable();
}

void blink(uint8_t c) {
    do {
        PORTA |= (1<<PA0);
        _delay_ms(100);
        PORTA &= ~(1<<PA0);
        _delay_ms(100);
        c--;
    } while (c>0);
    _delay_ms(500);
}

int main (void) {

    uint16_t temperature;
    uint16_t humidity;
    uint8_t  dht22res;

    uint8_t payload[wl_module_PAYLOAD];		//Array for Payload
	uint8_t k;
	
	wl_module_init();	    //initialise nRF24L01+ Module
	_delay_ms(50);		    //wait for nRF24L01+ Module
    wl_module_tx_config(wl_module_TX_NR_0);		//Config Module

 //   dhtpwr(true);

    PORTA = (1<<PA1) | (1<<PA0);
    DDRA  = (1<<PA0) | (1<<PA1);
    _delay_ms(500);

    while(1) {
        // measure
        PORTA = (1<<PA1);
        dht22res = dhtmeasure(&temperature, &humidity );
        PORTA = 0;

        for (k=0; k<=wl_module_PAYLOAD-1; k++) {
            payload[k] = k;
        }
        payload[0] = dht22res;
        payload[1] = temperature;
        payload[2] = (temperature>>8);
        payload[3] = humidity;
        payload[4] = (humidity>>8);

        sei();
        wl_module_send(payload,wl_module_PAYLOAD);
        _delay_ms(5000);

    }
}

ISR(WDT_OVERFLOW_vect) {
    isr_interval--; 
}

#if defined(__AVR_ATtiny2313__) || defined(__AVR_ATtiny2313A__)
ISR(INT0_vect) {
#endif
    uint8_t status;

    // Read wl_module status
    wl_module_CSN_lo;                               // Pull down chip select
    status = spi_fast_shift(NOP);					// Read status register
    wl_module_CSN_hi;                               // Pull up chip select

    // Package has been sent
    if (status & (1<<TX_DS)) {
        wl_module_config_register(STATUS, (1<<TX_DS));	//Clear Interrupt Bit
        PTX=0;
    }

    // IRQ: Package has not been sent, send again
    if (status & (1<<MAX_RT)) {
        wl_module_config_register(STATUS, (1<<MAX_RT));	// Clear Interrupt Bit
        wl_module_CE_hi;								// Start transmission
        _delay_us(10);								
        wl_module_CE_lo;
    }

    //TX_FIFO Full <-- this is not an IRQ
    if (status & (1<<TX_FULL)) {
        wl_module_CSN_lo;                               // Pull down chip select
        spi_fast_shift(FLUSH_TX);						// Flush TX-FIFO
        wl_module_CSN_hi;                               // Pull up chip select
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
