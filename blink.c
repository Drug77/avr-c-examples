#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    // Set PB0 as output (Pin 5 on ATtiny85)
    DDRB |= (1 << PB0);

    while (1) {
        // Toggle LED on PB0
        PORTB ^= (1 << PB0);
        _delay_ms(500);  // 500ms delay (based on F_CPU defined in Makefile)
    }
}
