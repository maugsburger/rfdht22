#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> 
#include <stdint.h>

#include "c_dht22.h"

#if F_CPU == 8000000 
    #define TCCR1DIVIDE (0<<CS12) | (1<<CS11) | (0<<CS10)
#elif F_CPU == 1000000 
    #define TCCR1DIVIDE (0<<CS12) | (0<<CS11) | (1<<CS10)
#else
    #error No supported CPU Freq
#endif

#define _TCCR1BEDGE(e) ((1<<ICNC1) | (e<<ICES1) | TCCR1DIVIDE)

/*
 * read dht22 sensor
 * return
 *  0 no error
 *  1 wrong checksum
 *  2 no data
 *
 *  *temp   temperature * 10
 *  *hum    humidity * 10
 *
 */

int dhtmeasure(uint16_t *temp, uint16_t *hum);
