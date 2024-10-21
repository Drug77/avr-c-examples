## To PING the ATtiny85 chip
Avrdude version 8.0
Copyright see https://github.com/avrdudes/avrdude/blob/main/AUTHORS
avrdude.exe -c usbtiny -p t85 -v
System wide configuration file is C:\avrdude-v8.0-windows-x64\avrdude.conf

Using port            : usb
Using programmer      : usbtiny
AVR part              : ATtiny85
Programming modes     : SPM, ISP, HVSP, debugWIRE
Programmer type       : USBtiny
Description           : USBtiny simple USB programmer
Using SCK period of 10 usec

AVR device initialized and ready to accept instructions
Device signature = 1E 93 0B (ATtiny85)

Avrdude done.  Thank you.

## Check all fuses
avrdude.exe -c usbtiny -p t85 -U lfuse:r:-:i -U hfuse:r:-:i -U efuse:r:-:i

Processing -U lfuse:r:-:i
Reading lfuse memory ...
Writing 1 byte to output file <stdout>
:01000000629D
:00000001FF

Processing -U hfuse:r:-:i
Reading hfuse memory ...
Writing 1 byte to output file <stdout>
:01000000DF20
:00000001FF

Processing -U efuse:r:-:i
Reading efuse memory ...
Writing 1 byte to output file <stdout>
:01000000FF00
:00000001FF

Avrdude done.  Thank you.

## Low Fuse 0x62
The fuse settings you retrieved show the following values:

Low Fuse (lfuse): 0x62
High Fuse (hfuse): 0xDF
Extended Fuse (efuse): 0xFF
To determine the CPU speed of your ATtiny85, we need to decode the low fuse value (lfuse), which controls the clock source and frequency settings.

Analyzing the Low Fuse (0x62):
The low fuse (0x62) in binary is:

01100010
The bits in the low fuse represent the following configuration options:

Bit	Name	Description	Value (0x62)
7	CKDIV8	Divide clock by 8	0 (Disabled)
6	CKOUT	Clock output on PORTB0	1 (Disabled)
5	SUT1	Start-up time configuration	1
4	SUT0	Start-up time configuration	1
3	CKSEL3	Clock source select	0
2	CKSEL2	Clock source select	0
1	CKSEL1	Clock source select	1
0	CKSEL0	Clock source select	0
Clock Source Selection (CKSEL3..0):
From the CKSEL3..0 bits (0110), the clock source is:

Internal 8 MHz RC Oscillator.
Clock Division (CKDIV8):
CKDIV8 = 0: This means the clock is not divided by 8. So the full internal clock speed is used.
CPU Speed Conclusion:
Your ATtiny85 is using the internal 8 MHz oscillator without clock division. Therefore, the CPU speed is 8 MHz.
Summary:
CPU Speed: 8 MHz (internal oscillator, no clock division).

## Low fuse to 8mhz
avrdude -c usbtiny -p t85 -U lfuse:w:0xE2:m

## Flash blink.hex
avrdude -c usbtiny -p t85 -U flash:w:blink.hex:i

## Compile pushbutton.c script
docker-compose exec avr make status
docker-compose exec avr make TARGET=traffic_light
docker-compose exec avr make TARGET=traffic_light flash

## Read data from serial
sudo chmod 666 /dev/ttyUSB0
stty -F /dev/ttyUSB0 9600
cat /dev/ttyUSB0