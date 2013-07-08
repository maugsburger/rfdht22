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
