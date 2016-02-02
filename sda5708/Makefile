TARGET	= main
MCU		= attiny2313
F_CPU	= 1000000UL
CC		= avr-gcc
OBJCOPY	= avr-objcopy
AVRSIZE = avr-size

CFLAGS	= -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os 
CFLAGS += -fno-common -std=gnu99
CFLAGS += -DF_CPU=$(F_CPU)

ASMFLAGS = -DF_CPU=$(F_CPU) -mmcu=$(MCU)

AVRDUDE_PROGRAMMER = usbasp
AVRDUDE_PORT = /dev/ttyUSB0
AVRDUDE_BAUD = 57600

build: $(TARGET).hex

$(TARGET).hex: $(TARGET).c sda5708.c i2cmaster.S
	$(CC) $(CFLAGS) -c -o $(TARGET).o $(TARGET).c
	$(CC) $(CFLAGS) -c -o sda5708.o sda5708.c
	$(CC) $(ASMFLAGS) -c -o i2cmaster.o i2cmaster.S
	$(CC) -mmcu=$(MCU) -o $(TARGET).elf *.o 
	$(OBJCOPY) -O ihex $(TARGET).elf $(TARGET).hex
	$(AVRSIZE) $(TARGET).elf $(TARGET).hex

flash: $(TARGET).hex
	sudo avrdude -p $(MCU) -c $(AVRDUDE_PROGRAMMER) -V -U flash:w:$(TARGET).hex	

clean:
	rm -f *.o *.elf *.hex
