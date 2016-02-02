/* ******************************************************
 *
 *             sda5708.c
 * ===================================
 * Uwe Berger (bergeruw@gmx.net); 2016
 * 
 * Routinen zur Ansteuerung eines Displays SDA5708
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

#include "sda5708.h"
#include "font.h"

static uint8_t brightness;

//*********************************************************************
void sda5708_reset(void)
{
	// RESET toggeln
	SDA5708_PORT_RESET &= ~(1<<SDA5708_RESET);
	SDA5708_PORT_RESET |=  (1<<SDA5708_RESET);
}

//*********************************************************************
void sda5708_init(void)
{
	// Ausgaenge initialisieren
	SDA5708_DDR_RESET |= (1<<SDA5708_RESET);
	SDA5708_DDR_SDCLK |= (1<<SDA5708_SDCLK);
	SDA5708_DDR_DATA  |= (1<<SDA5708_DATA);
	SDA5708_DDR_LOAD  |= (1<<SDA5708_LOAD);
	sda5708_reset();
}

//*********************************************************************
void sda5708_send_byte(uint8_t byte)
{
	uint8_t x;	
	// LOAD auf Low
	SDA5708_PORT_LOAD &= ~(1<<SDA5708_LOAD);
	// jede Byte-Stelle ans Display senden
	for (x=0; x<=7;x++)	{
		// DATA entsprechend setzen
		if ((byte>>x)&1) SDA5708_PORT_DATA |= (1<<SDA5708_DATA); 
		else SDA5708_PORT_DATA &= ~(1<<SDA5708_DATA);
		// SDCLK toggeln
		SDA5708_PORT_SDCLK |=  (1<<SDA5708_SDCLK);
		SDA5708_PORT_SDCLK &= ~(1<<SDA5708_SDCLK);
	}
	// LOAD auf High
	SDA5708_PORT_LOAD |= (1<<SDA5708_LOAD);
}

//*********************************************************************
void sda5708_clear(void)
{
	// 5.Bit --> Low
	sda5708_send_byte(0b11000000|(brightness&0b00000111));
}

//*********************************************************************
void sda5708_brightness(uint8_t val)
{
	// Helligkeit von 0...8, wobei 0 ganz hell ist!
	// mit dem 4.Bit koennte man noch die Multiplex-Impulslaenge des 
	// Display-Controllers manipulieren und damit das Ding noch dunkler
	// machen...
	// Helligkeitswert benoetigen wir auch beim Loeschen des Displays...
	brightness = val;
	sda5708_send_byte(0b11100000|(brightness&0b00000111));
}

//*********************************************************************
void sda5708_out_digit(uint8_t sign, uint8_t digit)
{
	uint8_t i;
	// anzeigbares Zeichen? Wenn nein, dann Leerzeichen!
	if ((sign < 0x20) || (sign > 0x7F)) sign = 0x20;
	// Digit im Bereich?
	if (digit > 7) digit = 0;
	// Digit-Adresse zum Display senden
	sda5708_send_byte(0b10100000|digit);
	// jede Zeile des Fonts adressieren, lesen, ausgeben
	for(i=0; i<7; i++) {
		// Font ist linksbuendig, deshalb 3 Bit nach rechts schieben...
		sda5708_send_byte(pgm_read_byte(&font[(sign-0x20)*7+i])/8);
	}
}
