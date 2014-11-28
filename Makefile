
SOURCES=uart.c project.c imu.c twimaster.c capture.c midi.c

#ASM=i2cmaster.S
ASM=
OBJECTS=$(SOURCES:.c=.o)

CC=avr-gcc
CPU=atmega1284p
FLAGS=-mmcu=$(CPU)
CFLAGS= -Os -std=c99 -DF_CPU=16000000UL -D SERIAL=1
PROGRAMMER=usbtiny

LDFLAGS=-Wl,-u,vfprintf -lprintf_flt -lm -Os -fdce
LDFLAGS+=-Wl,-u,vfscanf -lscanf_flt -lm


EXECUTABLE=project

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(ASM)
	$(CC) $(FLAGS) $(LDFLAGS) $(ASM) $(OBJECTS) -o $@
	avr-objcopy -O ihex -R .eeprom $(EXECUTABLE) $(EXECUTABLE).hex

install: $(EXECUTABLE)
	avrdude -c $(PROGRAMMER) -p $(CPU) -F -B 1 -U flash:w:$(EXECUTABLE).hex

clean:
	rm $(OBJECTS) $(EXECUTABLE) $(EXECUTABLE).hex

.c.o:
	$(CC) $(FLAGS) $(CFLAGS) -c $< -o $@


