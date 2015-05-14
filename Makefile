
TARGET	= main
MCU		= atmega328p
F_CPU	= 16000000UL
CC		= avr-gcc
OBJCOPY	= avr-objcopy

AVRDUDE_PROGRAMMER = arduino
AVRDUDE_PORT = /dev/ttyUSB0
AVRDUDE_BAUD = 57600

CFLAGS	=-g -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues -save-temps -fno-common -std=gnu99
CFLAGS += -DF_CPU=$(F_CPU)
EXT		= c

SOURCES :=$(wildcard *.$(EXT))
OBJECTS :=$(patsubst %.$(EXT),%.o,$(SOURCES))

all: $(TARGET).hex

$(TARGET).hex : $(TARGET).elf
	$(OBJCOPY) -j .data -j .text -O ihex $^ $@
	avr-size $@

$(TARGET).elf : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ -Wl,-Map,$(TARGET).map $^

%.o: %.$(EXT)
	$(CC) $(CFLAGS) -c $< -o $@

flash:
	avrdude -p $(MCU) -c $(AVRDUDE_PROGRAMMER) -P $(AVRDUDE_PORT) -b $(AVRDUDE_BAUD) -U flash:w:$(TARGET).hex	


.PHONY: clean
clean:
	rm -f *.i *.s $(TARGET).elf $(TARGET).hex $(TARGET).out $(TARGET).s $(TARGET).i $(TARGET).dep $(TARGET).map $(OBJECTS)
