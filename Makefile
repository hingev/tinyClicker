
PRG = main
DEV = attiny85

CC = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS = -c -DF_CPU=8000000UL -mmcu=$(DEV)
LDFLAGS =  -mmcu=$(DEV)

PROG = usbtiny

SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)

all: $(PRG).hex

$(PRG).hex: $(PRG).elf
	$(OBJCOPY) -j .text -j .data -O ihex $<

$(PRG).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<
