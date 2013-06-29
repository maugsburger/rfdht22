#include "dht22.h"

#ifdef DHT22_UARTDEBUG
#include "uart.h"
#endif

#define PINTIME_SIZE 42
static volatile uint8_t pintime[PINTIME_SIZE];
static volatile uint8_t pintime_pos = 0;

void dhtinit() {
#if defined(__AVR_ATtiny2313__) || defined(__AVR_ATtiny2313A__)
    DDRD &= ~(1<<PD6); //PD6 is ICP1
#else
    #error not defined for this uc
#endif
}


int dhtmeasure(uint16_t *temp, uint16_t *hum) {

#ifdef DHT22_UARTDEBUG
    char s[7];
    uart_puts( "measuring\n\r" );
    uart_puts( "\n\r" );
#endif

#if defined(__AVR_ATtiny2313__) || defined(__AVR_ATtiny2313A__)
    TCCR1B = _TCCR1BEDGE(1);

    TIFR = (1<<ICF1); // clear input capture flag, set after every interrupt handler
    TIMSK |= (1<<ICIE1); // enable input capture interrupt
#else
    #error not defined for this uc
#endif


    // start pulse 2ms
    PORTD &= ~(1<<PD6);
    DDRD |= (1<<PD6);
    _delay_ms(1);
    // pb0 high, input
    DDRD &= ~(1<<PD6);


    sei(); // enable interrupts
    pintime_pos = 0;

    // read data for 500ms
    _delay_ms(100); 
    cli();

#if defined(__AVR_ATtiny2313__) || defined(__AVR_ATtiny2313A__) 
    // disable timer1 clock
    TCCR1B = 0x00;
    TIMSK &= ~(1<<ICIE1); // enable input capture interrupt
#else
    #error not defined for this uc
#endif

    // measure complete
    uint8_t dht22data[6] = {0,0,0,0,0,0};
    uint8_t dht22data_bitcount = 0;
    *temp = 0;
    *hum = 0;

    for (int i = 0; i < pintime_pos; i++) {
        if ( pintime[i] < 30 && pintime[i] > 20 ) {
            dht22data[ dht22data_bitcount / 8 ] <<= 1;
            dht22data_bitcount++;
        } else if ( pintime[i] > 65 && pintime[i] < 76 ) {
            dht22data[ dht22data_bitcount / 8 ] <<= 1;
            dht22data[ dht22data_bitcount / 8 ] |= 1;
            dht22data_bitcount++;
        } else {
            continue;
        }
    }
    // checksum
    dht22data[5] = dht22data[0]+dht22data[1]+dht22data[2]+dht22data[3];

    if ( dht22data_bitcount != 40 ) {
        return 2;
    } else if ( dht22data[5] == dht22data[4] ) {
        *temp = (dht22data[2] << 8 | dht22data[3]);
        *hum = (dht22data[0] << 8 | dht22data[1]);
#ifdef DHT22_UARTDEBUG
        uart_puts("checksum correct\r\n");
        uart_puts("hum: ");
        uart_puts( utoa( *hum, s, 10) );
        uart_puts("\r\n");
        uart_puts("temp: ");
        uart_puts( utoa( *temp, s, 10) );
        uart_puts("\r\n");
#endif
        return 0;
    } else {
#ifdef DHT22_UARTDEBUG
        uart_puts("incorrect data\r\n");
        for (int i = 0; i < PINTIME_SIZE; i++) {
            uart_puts( utoa(pintime[i], s, 10) );
            uart_putc( ' ' );
            if ( pintime[i] < 30 && pintime[i] > 20 ) {
                uart_putc('0');
            } else if ( pintime[i] > 65 && pintime[i] < 76 ) {
                uart_putc('1');
            } else {
                uart_putc('-');
            }
            uart_putc( ' ' );
            if( (i-1)%8 == 0 ) {
                uart_puts( "\n\r" );
            }
        }
#endif
        return 1;
    }
}



ISR( TIMER1_CAPT_vect ) {
    if( TCCR1B == _TCCR1BEDGE(1) ) {
        // triggered on rising edge
        TCNT1 = 0; // count high time
        TCCR1B = _TCCR1BEDGE(0);
    } else {
        // edge fallen, time!
        TCCR1B = _TCCR1BEDGE(1);
        pintime[pintime_pos] = ICR1L;
        if ( pintime_pos < PINTIME_SIZE ) pintime_pos++;
    }
}

