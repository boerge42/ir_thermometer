/* ******************************************************
 *
 *       Infrarot-Termometer
 *       ===================
 *        Uwe Berger, 2015 
 * 
 * Hardware:
 * ---------
 *   * Arduino Nano (es ist natuerlich auch jedes
 *     andere Board mit einem Atmega verwendbar...) 
 *   * IR-Sensor MLX90614 via TWI an den MC ange-
 *     schlossen
 *   * TWI-LCD (via PCF8574) siehe auch:
 *     http://bralug.de/wiki/BLIT2008-Board-LCD
 * 
 * PullUps an SDA und SCL nicht vergessen!
 * 
 * Es werden Umgebungstemperatur und Objekttemperatur
 * (IR-Sensor) in Grad Celsius ueber die serielle
 * Schnittstelle und auf dem LCD ausgegeben.
 * 
 * ---------
 * Have fun!
 * 
 ********************************************************/

#include <util/delay.h>
#include "usart.h"
#include "i2cmaster.h"
#include "i2clcd.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define BAUDRATE 57600

#define printf_uart(...) usart_write(__VA_ARGS__)

// Definitionen fuer MLX90614
#define MLX90614_TWIADDR	(0x5A<<1)
#define MLX90614_RAWIR1		0x04
#define MLX90614_RAWIR2		0x05
#define MLX90614_TA			0x06
#define MLX90614_TOBJ1		0x07
#define MLX90614_TOBJ2		0x08 

//****************************************************
void long_delay(uint16_t ms)
{
	for(; ms>0; ms--) _delay_ms(1);
}
 

//****************************************************
uint16_t read16_mlx90614(uint8_t reg)
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
int16_t get_temp_mlx90614(uint8_t reg)
{
	int32_t val;
	val = (int32_t)read16_mlx90614(reg);
	val = val*2-27315;
	return (int16_t)val;
} 

//****************************************************
//****************************************************
//****************************************************
int main(void) {
	int16_t ta, to;
	uint8_t x;
	char buf[5];

	// TWI initialisieren
	i2c_init();

	// TWI-LCD initialisieren
	lcd_init();
	lcd_backlight(ON);
	
	// serielle Schnittstelle
	usart_init(BAUDRATE); 
	printf_uart("IR-Thermometer; Uwe Berger; 2015\r\n"); 
	
	while(1) {

		// Temperaturwerte holen...
		// ...Umgebungstemperatur
		ta=get_temp_mlx90614(MLX90614_TA);
		// ...Objekttemperatur (IR-Sensor)
		to=get_temp_mlx90614(MLX90614_TOBJ1);
		
		// Ausgabe UART
		printf_uart("TA: %i.%i°C\t", ta/100, ta%100);
		printf_uart("TOBJ: %i.%i°C\n\r", to/100, to%100);
		
		// Ausgabe LCD
		lcd_command(LCD_CLEAR);
		x=1;
		lcd_printlc(1, x, "TA...:");
		x += 7;
		itoa(ta/100, buf, 10);	
		lcd_printlr(1, x, buf);
		x += strlen(buf);
		lcd_printlc(1, x, ".");
		x += 1;
		itoa(ta%100, buf, 10);	
		lcd_printlr(1, x, buf);
		x += strlen(buf);
		lcd_putcharlr(1, x, 223);
		x += 1;
		lcd_printlr(1, x, "C");
		x=1;
		lcd_printlc(2, x, "TOBJ.:");
		x += 7;
		itoa(to/100, buf, 10);	
		lcd_printlr(2, x, buf);
		x += strlen(buf);
		lcd_printlc(2, x, ".");
		x += 1;
		itoa(to%100, buf, 10);	
		lcd_printlr(2, x, buf);
		x += strlen(buf);
		lcd_putcharlr(2, x, 223);
		x += 1;
		lcd_printlr(2, x, "C");
		
		// 2s Pause
		long_delay(2000);
	}
}
