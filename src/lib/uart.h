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

#define BAUD 9600
#include <util/setbaud.h>
#include <avr/io.h>
#include <stdint.h>

void uart_putc(unsigned char c);
void uart_puts(char *s);
void uart_init(void);
