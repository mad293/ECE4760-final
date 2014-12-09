
_SOURCES=uart.c project.c imu.c twimaster.c capture.c midi.c
SOURCES=$(addprefix src/,$(_SOURCES))

OBJECTS=$(addprefix obj/,$(notdir $(SOURCES:.c=.o)))

CONFIG=doxygen.config
CC=avr-gcc
CPU=atmega1284p
FLAGS=-mmcu=$(CPU)
CFLAGS= -Os -std=c99 -DF_CPU=16000000UL -D SERIAL=1
PROGRAMMER=usbtiny

LDFLAGS=-Wl,-u,vfprintf -lprintf_flt -lm -Os -fdce
LDFLAGS+=-Wl,-u,vfscanf -lscanf_flt -lm


EXECUTABLE=project

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(FLAGS) $(LDFLAGS) $(OBJECTS) -o $@
	avr-objcopy -O ihex -R .eeprom $(EXECUTABLE) $(EXECUTABLE).hex
	rm project

install: $(EXECUTABLE)
	avrdude -c $(PROGRAMMER) -p $(CPU) -F -B 1 -U flash:w:$(EXECUTABLE).hex

clean:
	rm -rf html
	rm $(OBJECTS) $(EXECUTABLE).hex

obj/%.o: src/%.c
	$(CC) $(FLAGS) $(CFLAGS) -c $< -o $@

doc:
	doxygen $(CONFIG)
