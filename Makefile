# Target microcontroller
MCU=attiny85

# Default program name (can be overridden from the console)
TARGET ?= blink

# Clock frequency
F_CPU=8000000UL

# Source files (use the target variable for source file)
SRC=$(TARGET).c

# Compiler flags
CFLAGS=-mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os

# AVR programmer settings
PROGRAMMER=usbtiny
PORT=usb

# Default target is to build the hex file
all: $(TARGET).hex

# Compile the C source file into an ELF file
%.elf: $(SRC)
	avr-gcc $(CFLAGS) -o $@ $^

# Convert the ELF file to a HEX file
%.hex: %.elf
	avr-objcopy -O ihex -R .eeprom $< $@

# Command to flash the compiled HEX file to the microcontroller
flash: $(TARGET).hex
	avrdude -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -U flash:w:$(TARGET).hex:i

# Command to check the microcontroller status
status:
	avrdude -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -v

# Clean up build files
clean:
	rm -f *.o *.elf *.hex
