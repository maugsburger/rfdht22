/*
    Copyright (c) 2007 Stefan Engelke <mbox@stefanengelke.de>

    Permission is hereby granted, free of charge, to any person 
    obtaining a copy of this software and associated documentation 
    files (the "Software"), to deal in the Software without 
    restriction, including without limitation the rights to use, copy, 
    modify, merge, publish, distribute, sublicense, and/or sell copies 
    of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be 
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.

    $Id$
*/

#include "spi.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#if defined(__AVR_ATtiny2313__) || defined(__AVR_ATtiny2313A__)
#define PORT_USI    PORTB
#define DDR_USI     DDRB
#define DD_DO       DDB6 // MOSI
#define DD_DI       DDB5 // MISO
#define DD_SCK      DDB7
#else
#define PORT_SPI    PORTB
#define DDR_SPI     DDRB
#define DD_MISO     DDB4
#define DD_MOSI     DDB3
#define DD_SCK      DDB5
#endif

#if defined(__AVR_ATtiny2313__) || defined(__AVR_ATtiny2313A__)
// Initialize pins for spi communication
void spi_init() {
    DDR_USI &= ~(1<<DD_DI);
    // Define the following pins as output
    DDR_USI |= ((1<<DD_DO)|(1<<DD_SCK));
    // Pullup on DI
    PORT_USI |= (1<<DDB5);
}

// Shift full array through target device
void spi_transfer_sync (uint8_t *dataout, uint8_t *datain, uint8_t len) {
       uint8_t i;      
       for (i = 0; i < len; i++) {
            USIDR = dataout[i];
            USISR = (1<<USIOIF);
            while ( (USISR & (1<<USIOIF)) == 0 ) {
                    USICR = (1<<USIWM0)|(1<<USICS1)|(1<<USICLK)|(1<<USITC);
            }
            datain[i] = USIDR;
       }
}

// Shift full array to target device without receiving any byte
void spi_transmit_sync (uint8_t *dataout, uint8_t len) {
       uint8_t i;      
       for (i = 0; i < len; i++) {
            USIDR = dataout[i];
            USISR = (1<<USIOIF);
            while ( (USISR & (1<<USIOIF)) == 0 ) {
                    USICR = (1<<USIWM0)|(1<<USICS1)|(1<<USICLK)|(1<<USITC);
            }
       }
}

// Clocks only one byte to target device and returns the received one
uint8_t spi_fast_shift (uint8_t data) {
    USIDR = data;

    USISR = (1<<USIOIF);
    while ( (USISR & (1<<USIOIF)) == 0 ) {
            USICR = (1<<USIWM0)|(1<<USICS1)|(1<<USICLK)|(1<<USITC);
    }

    return USIDR;
}
#else
// Initialize pins for spi communication
void spi_init() {
    DDR_SPI &= ~(1<<DD_MISO);
    // Define the following pins as output
    DDR_SPI |= ((1<<DD_MOSI)|(1<<DD_SCK));

    SPCR = ((1<<SPE)|               // SPI Enable
            (0<<SPIE)|              // SPI Interupt Enable
            (0<<DORD)|              // Data Order (0:MSB first / 1:LSB first)
            (1<<MSTR)|              // Master/Slave select   
            (0<<SPR1)|(1<<SPR0)|    // SPI Clock Rate
            (0<<CPOL)|              // Clock Polarity (0:SCK low / 1:SCK hi when idle)
            (0<<CPHA));             // Clock Phase (0:leading / 1:trailing edge sampling)

    SPSR = (1<<SPI2X);              // Double Clock Rate
    
}

// Shift full array through target device
void spi_transfer_sync (uint8_t *dataout, uint8_t *datain, uint8_t len) {
       uint8_t i;      
       for (i = 0; i < len; i++) {
             SPDR = dataout[i];
             while((SPSR & (1<<SPIF))==0);
             datain[i] = SPDR;
       }
}

// Shift full array to target device without receiving any byte
void spi_transmit_sync (uint8_t *dataout, uint8_t len) {
       uint8_t i;      
       for (i = 0; i < len; i++) {
             SPDR = dataout[i];
             while((SPSR & (1<<SPIF))==0);
       }
}

// Clocks only one byte to target device and returns the received one
uint8_t spi_fast_shift (uint8_t data) {
    SPDR = data;
    while((SPSR & (1<<SPIF))==0);
    return SPDR;
}
#endif
