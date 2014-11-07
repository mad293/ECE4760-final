
SOURCES=i2c.c uart.c project.c
OBJECTS=$(SOURCES:.c=.o)

CC=avr-gcc
FLAGS=-mmcu=atmega1284p
CFLAGS= -Os -std=c99 -DF_CPU=16000000UL
CPU=atmega1284p
PROGRAMMER=usbtiny

EXECUTABLE=project

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(FLAGS) $(LDFLAGS) $(OBJECTS) -o $@
	avr-objcopy -O ihex -R .eeprom $(EXECUTABLE) $(EXECUTABLE).hex

install: $(EXECUTABLE)
	avrdude -c $(PROGRAMMER) -p $(CPU) -F -U flash:w:$(EXECUTABLE).hex

clean:
	rm $(OBJECTS) $(EXECUTABLE) $(EXECUTABLE).hex

.c.o:
	$(CC) $(FLAGS) $(CFLAGS) -c $< -o $@

