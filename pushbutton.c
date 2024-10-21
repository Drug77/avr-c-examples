#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Function prototypes
void init_io();
uint8_t debounce_button();

int main(void) {
    init_io();  // Initialize I/O ports

    uint8_t led_on = 0;  // LED state: 0 = off, 1 = blinking

    while (1) {
        if (debounce_button()) {  // If button is pressed
            if (led_on) {
                // Turn off the LED
                PORTB &= ~(1 << PB0);
                led_on = 0;
            } else {
                // Start LED blinking
                led_on = 1;
            }
        }

        if (led_on) {
            PORTB ^= (1 << PB0);  // Toggle LED on PB0
            _delay_ms(500);       // Delay for blinking
        } else {
            PORTB &= ~(1 << PB0);  // Ensure the LED is off when not blinking
        }
    }
}

// Initialize the I/O ports for the LED and button
void init_io() {
    DDRB |= (1 << PB0);   // Set PB0 (LED) as output
    DDRB &= ~(1 << PB1);  // Set PB1 (Button) as input
    PORTB |= (1 << PB1);  // Enable internal pull-up resistor for PB1
}

// Debounce the button press (basic software debounce)
uint8_t debounce_button() {
    if (!(PINB & (1 << PB1))) {  // Check if button is pressed (active LOW)
        _delay_ms(50);           // Debounce delay
        if (!(PINB & (1 << PB1))) {  // Check again to confirm button press
            while (!(PINB & (1 << PB1)));  // Wait for the button to be released
            return 1;  // Button pressed and debounced
        }
    }
    return 0;  // Button not pressed
}
