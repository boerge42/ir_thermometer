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
int16_t abs(int16_t val) {
	if (val<0) return -val; else return val;
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

	// TWI initialisieren
	i2c_init();

	// TWI-LCD initialisieren
	lcd_init();
	lcd_backlight(ON);
	
	// serielle Schnittstelle initialisieren
	usart_init(BAUDRATE); 
	printf_uart("IR-Thermometer; Uwe Berger; 2015\r\n"); 
	
	while(1) {

		// Temperaturen auslesen
		ta=get_temp_mlx90614(MLX90614_TA); // Umgebungstemperatur
		to=get_temp_mlx90614(MLX90614_TOBJ1); // Objekttemperatur

		// Ausgabe UART
		printf_uart("TA: %i.%i°C\t", ta/100, abs(ta%100));
		printf_uart("TO: %i.%i°C\n\r", to/100, abs(to%100));

		// Ausgabe LCD
		lcd_command(LCD_CLEAR);
		lcd_gotolr(1,1);
		lcd_printf("TA: %i.%i%cC", ta/100, abs(ta%100), 223);
		lcd_gotolr(2,1);
		lcd_printf("TO: %i.%i%cC", to/100, abs(to%100), 223);

		// 2s Pause
		long_delay(2000);
	}
}
