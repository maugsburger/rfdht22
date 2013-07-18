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

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "nRF24L01.h"
#include "spi.h"
#include "wl_module.h"
#include "dht22.h"

/* interval = seconds / 8 */
uint8_t EEMEM eep_interval = 3;

/* default autoconf channel */
uint8_t EEMEM eep_wl_conf_channel = 0x01;
uint8_t EEMEM eep_wl_conf_addr[5] = { 0x42, 0xaf, 0xfe, 0xde, 0xad };

/* runtime channel */
uint8_t EEMEM eep_wl_run_channel = 0x02;
uint8_t EEMEM eep_wl_run_cmd_addr[5] = { 0xc2, 0xc2, 0xc2, 0xc2, 0xc2 };
uint8_t EEMEM eep_wl_run_data_addr[5] = { 0xe7, 0xe7, 0xe7, 0xe7, 0xe7 };

void dhtpwr_on( void );
void dhtpwr_off( void );
void sleep_time(uint8_t sleepmode, uint8_t intervals );
void blink(uint8_t c);
