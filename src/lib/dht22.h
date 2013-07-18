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

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> 
#include <stdint.h>

#include "c_dht22.h"

#if F_CPU == 8000000 
    #define TCCR1DIVIDE (0<<CS12) | (1<<CS11) | (0<<CS10)
    #define DHT22_0_MIN 20
    #define DHT22_0_MAX 30
    #define DHT22_1_MIN 65
    #define DHT22_1_MAX 76
#elif F_CPU == 1000000 
    #define TCCR1DIVIDE (0<<CS12) | (0<<CS11) | (1<<CS10)
    #define DHT22_0_MIN 20
    #define DHT22_0_MAX 30
    #define DHT22_1_MIN 65
    #define DHT22_1_MAX 76
#elif F_CPU == 4000000
    #define TCCR1DIVIDE (0<<CS12) | (1<<CS11) | (0<<CS10)
    #define DHT22_0_MIN 20 / 2
    #define DHT22_0_MAX 30 / 2
    #define DHT22_1_MIN 65 / 2
    #define DHT22_1_MAX 76 / 2
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

void dhtinit( void );
