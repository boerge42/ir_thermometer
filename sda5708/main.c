/* ******************************************************
 *
 *       Infrarot-Termometer
 *       (MLX90617, sda5708)
 *       ===================
 *        Uwe Berger, 2016 
 * 
 * Hardware:
 * ---------
 *   * ATTiny2313 in Standard-Beschaltung
 *   * IR-Sensor MLX90614 (via I2C)
 *   * Display sda5708
 *   * Rot/Gruen-LED
 * 
 * Verwendete MCU-PINs siehe Quelltext ;-)
 * 
 * Es wird Objekttemperatur (IR-Sensor) in Grad Celsius 
 * auf dem Display ausgegeben.
 * 
 * ---------
 * Have fun!
 * 
 * 
 ********************************************************/

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <util/delay.h>

#include "i2cmaster.h"
#include "sda5708.h"

// Hardwarekonfiguration (ATTiny2313)
// ...SDA5708; siehe sda5708.h
// RESET 	-->	PB0	(12)
// SDCLK 	-->	PB1	(13)
// DATA		-->	PB2	(14)
// LOAD		-->	PB3	(15)

// ...MLX90614 (I2C); siehe i2cmaster.S
// SDA   --> PD0 (2)
// SDL   --> PD1 (3)

// ...Rot/Gruen-LED
#define LED1_PORT			PORTD
#define LED1_DDR			DDRD
#define LED1 				PD4		// (8)
#define LED2_PORT			PORTD
#define LED2_DDR			DDRD
#define LED2		 		PD5		// (9)

// ...ISP (vorgegeben)
// RESET --> PA2 (1)
// MOSI  --> PB5 (17)
// MISO  --> PB6 (18)
// SCK   --> PB7 (19)

// Definitionen fuer MLX90614
#define MLX90614_TWIADDR	(0x5A<<1)
#define MLX90614_RAWIR1		0x04
#define MLX90614_RAWIR2		0x05
#define MLX90614_TA			0x06
#define MLX90614_TOBJ1		0x07
#define MLX90614_TOBJ2		0x08 

// Definitionen LED schalten
#define LED_INIT	LED1_DDR  |= (1<<LED1); LED2_DDR  |= (1<<LED2);
#define LED_RED		LED1_PORT |= (1<<LED1); LED2_PORT &= ~(1<<LED2);
#define LED_GREEN	LED2_PORT |= (1<<LED2); LED1_PORT &= ~(1<<LED1);

// Anzeigemodus
#define MODE_OBJECT		0
#define MODE_AMBIENTE	1

//****************************************************
void long_delay(uint16_t ms)
{
	for(; ms>0; ms--) _delay_ms(1);
}
 
//****************************************************
int16_t abs(int16_t val) {
	if (val<0) return -val; else return val;
}

//****************************************************
uint16_t mlx90614_read16(uint8_t reg)
{ 
	uint16_t val; 
	i2c_start_wait(MLX90614_TWIADDR+I2C_WRITE); 
	i2c_write(reg); 
	i2c_rep_start(MLX90614_TWIADDR+I2C_READ); 
	val = i2c_readAck(); 
	val |= (i2c_readAck() & 0x7F) << 8; 
	i2c_readNak();
	i2c_stop();
	return val;
}
 
//****************************************************
int16_t mlx90614_get_temperature(uint8_t reg)
{
	int32_t val;
	val = (int32_t)mlx90614_read16(reg);
	val = val*2-27315;
	return (int16_t)val;
} 

//****************************************************
// 01234567
//  -70,0°C
//    0,0°C
//  380,0°C
void output_temperature(int16_t t)
{
	uint8_t i;
	uint16_t v;
	
	sda5708_clear();
	// Display von rechts nach links beschreiben
	i=7;
	// Grad Celsius
	sda5708_out_digit('C', i--);
	// ....in meinem Zeichensatz ist | das °-Zeichen ... ;-)
	sda5708_out_digit('|', i--);	
	// eine Nachkommastelle (mehr macht keinen Sinn!)
	v = abs(t%100);
	sda5708_out_digit((v/10)+0x30, i--);		
	// Komma selbst
	sda5708_out_digit(',', i--);	
	// Vorkommastellen (min. 1; max. 3)
	v = abs(t/100);
	sda5708_out_digit((v%10)+0x30, i--);
	v = v/10;
	if (v > 0) {		
		sda5708_out_digit((v%10)+0x30, i--);
		v = v/10;
		// Modul 10 nur zur Sicherheit
		if (v > 0) sda5708_out_digit((v%10)+0x30, i--);
	}		
 	// negatives Vorzeichen?
	if (t < 0) sda5708_out_digit('-', i);
}

//****************************************************
//****************************************************
//****************************************************
int main(void) 
{
	uint8_t mode = MODE_OBJECT;
	uint8_t count = 0;
	
	// LEDs 
	LED_INIT;
	// I2C initialisieren
	i2c_init();
	// Port-Initialisierung fuer Display
	sda5708_init();
  	// Helligkeit Display einstellen 
	sda5708_brightness(2);
	// Endlos-Loop
	while(1) {
		// Temperatur auslesen/ausgeben
		if (mode==MODE_OBJECT) {
			LED_RED;
			output_temperature(mlx90614_get_temperature(MLX90614_TOBJ1));
		} else {
			LED_GREEN;
			output_temperature(mlx90614_get_temperature(MLX90614_TA));
		}
		long_delay(1000);
		// Anzeigemodus umschalten
		count++;
		if (count==5) {
			count=0;
			if (mode==MODE_OBJECT) mode=MODE_AMBIENTE;
			else mode=MODE_OBJECT;
		}

	}
}
