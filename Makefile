
PRG = main
DEV = attiny85

CC = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS = -Os -c -DF_CPU=8000000UL -mmcu=$(DEV)
LDFLAGS =  -mmcu=$(DEV)

PROG = usbtiny

SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)

all: $(PRG).hex

flash: $(PRG).hex
	avrdude -p t85 -c usbtiny -U flash:w:$(PRG).hex:i

fuse:
	avrdude -p t85 -c usbtiny  -U lfuse:w:0xe2:m

reset:
	avrdude -p t85 -c usbtiny -E reset

clean:
	rm -rf *.hex *.elf *.o

$(PRG).hex: $(PRG).elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

$(PRG).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<
