/* ******************************************************
 *
 *             sda5708.h
 * ===================================
 * Uwe Berger (bergeruw@gmx.net); 2016
 * 
 * Routinen zur Ansteuerung eines Displays SDA5708
 * 
 * Hardwarebeschreibung: 
 * ---------------------
 * https://www.grautier.com/grautier/archives/63-Nokia-DBOX-1-Display-SDA-5708-24.html
 *
 * ---------
 * Have fun!
 *  
 * ---------------------------------------------------------------------
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 ********************************************************/
#ifndef _SDA5708_H
#define _SDA5708_H 

#include <avr/io.h>
#include <avr/pgmspace.h>

#define SDA5708_PORT_RESET	PORTB
#define SDA5708_DDR_RESET	DDRB
#define SDA5708_RESET 		PB0		
#define SDA5708_PORT_SDCLK	PORTB
#define SDA5708_DDR_SDCLK	DDRB
#define SDA5708_SDCLK 		PB1		
#define SDA5708_PORT_DATA	PORTB
#define SDA5708_DDR_DATA	DDRB
#define SDA5708_DATA 		PB2		
#define SDA5708_PORT_LOAD	PORTB
#define SDA5708_DDR_LOAD	DDRB
#define SDA5708_LOAD 		PB3		

void sda5708_reset(void);
void sda5708_init(void);
void sda5708_send_byte(uint8_t byte);
void sda5708_brightness(uint8_t val);
void sda5708_clear(void);
void sda5708_out_digit(uint8_t sign, uint8_t digit);

#endif
