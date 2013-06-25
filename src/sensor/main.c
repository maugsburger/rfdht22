#include <stdbool.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> 
#define BAUD 9600
#include <util/setbaud.h>

#if F_CPU == 8000000 
    #define TCCR1DIVIDE (0<<CS12) | (1<<CS11) | (0<<CS10)
#elif F_CPU == 1000000 
    #define TCCR1DIVIDE (0<<CS12) | (0<<CS11) | (1<<CS10)
#else
    #error No supported CPU Freq
#endif

#define _TCCR1BEDGE(e) ((1<<ICNC1) | (e<<ICES1) | TCCR1DIVIDE)


#define PINTIME_SIZE 42
volatile uint8_t pintime[PINTIME_SIZE];
volatile uint8_t pintime_pos = 0;

int uart_putc(unsigned char c) {
    while (!(UCSRA & (1<<UDRE))){}  /* warten bis Senden moeglich */
    UDR = c;                      /* sende Zeichen */
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

void dhtmeasure(void) {
    TCCR1B = _TCCR1BEDGE(1);

    TIFR = (1<<ICF1); // clear input capture flag, set after every interrupt handler
    TIMSK = (1<<ICIE1); // enable input capture interrupt
    sei(); // enable interrupts

    pintime_pos = 0;

    // start pulse 2ms
    PORTD &= ~(1<<PD6);
    DDRD |= (1<<PD6);
    _delay_ms(1);
    // pb0 high, input
    DDRD &= ~(1<<PD6);


    // read data for 500ms
    _delay_ms(500); 
    cli();

    // disable timer1 clock
    TCCR1B = 0x00;
    TIMSK = 0; // input capture interrupt

    // measure complete
    char s[7];
    uint8_t dht22data[6] = {0,0,0,0,0,0};
    uint8_t dht22data_bitcount = 0;
    uint16_t temp = 0;
    uint16_t hum = 0;

    uart_puts( "measure\n\r" );
    uart_puts( "\n\r" );

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

    if ( dht22data[5] == dht22data[4]  && dht22data_bitcount == 40 ) {
        uart_puts("checksum correct\r\n");
        temp = (dht22data[2] << 8 | dht22data[3]);
        hum = (dht22data[0] << 8 | dht22data[1]);
        uart_puts("hum: ");
        uart_puts( utoa( hum, s, 10) );
        uart_puts("\r\n");
        uart_puts("temp: ");
        uart_puts( utoa( temp, s, 10) );
        uart_puts("\r\n");
    } else {
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
    }
}

int main (void) {
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

    uart_puts("\n\rrestart\n\r");

    dhtpwr(true);

    PORTA = (1<<PA1) | (1<<PA0);
    DDRA  = (1<<PA0) | (1<<PA1);
    DDRD &= ~(1<<PD6); //PD6 als Eingang
    _delay_ms(500); 

    while(1) {
        PORTA = (1<<PA1);
        dhtmeasure();
        PORTA = 0;
        _delay_ms(10000); 
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

