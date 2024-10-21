#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t led_on = 0;  // LED state: 0 = off, 1 = blinking

// Function prototypes
void init_io();
void init_interrupts();

int main(void) {
    init_io();         // Initialize I/O ports
    init_interrupts(); // Set up pin change interrupt

    sei();  // Enable global interrupts

    while (1) {
        if (led_on) {
            PORTB ^= (1 << PB0);  // Toggle LED on PB0
            _delay_ms(500);       // 500ms delay for blinking
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

// Set up pin change interrupt for PB1
void init_interrupts() {
    // Enable Pin Change Interrupt on PCINT1 (PB1)
    GIMSK |= (1 << PCIE);       // Enable Pin Change Interrupts
    PCMSK |= (1 << PCINT1);     // Enable pin change interrupt for PB1
}

// Interrupt Service Routine for Pin Change Interrupt
ISR(PCINT0_vect) {
    // Toggle LED state (blink or off) when the button is pressed
    if (!(PINB & (1 << PB1))) {  // Check if button is pressed (active low)
        led_on = !led_on;        // Toggle the LED state
    }
}