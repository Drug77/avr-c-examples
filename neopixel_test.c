#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

#define LED_COUNT 8  // 8 LEDs

void ws2812_send_pixel(uint8_t r, uint8_t g, uint8_t b);
void ws2812_show(void);
void set_color(uint8_t r, uint8_t g, uint8_t b);

int main(void) {
    // Set PB0 as output for WS2812 data
    DDRB |= (1 << PB0);

    while (1) {
        // Set all LEDs to Red
        set_color(255, 0, 0);  
        _delay_ms(1000);  // Delay 1 second

        // Set all LEDs to Green
        set_color(0, 255, 0);  
        _delay_ms(1000);  // Delay 1 second

        // Set all LEDs to Blue
        set_color(0, 0, 255);  
        _delay_ms(1000);  // Delay 1 second
    }
}

// Set all LEDs to the same color
void set_color(uint8_t r, uint8_t g, uint8_t b) {
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        ws2812_send_pixel(r, g, b);
    }
    ws2812_show();  // Send data to LEDs
}

// Send pixel data to WS2812 LEDs
void ws2812_send_pixel(uint8_t r, uint8_t g, uint8_t b) {
    // Send GRB order (Green, Red, Blue)
    for (uint8_t i = 0; i < 8; i++) {
        if (g & (1 << (7 - i))) {
            PORTB |= (1 << PB0); _delay_us(0.7); PORTB &= ~(1 << PB0); _delay_us(0.6);
        } else {
            PORTB |= (1 << PB0); _delay_us(0.35); PORTB &= ~(1 << PB0); _delay_us(0.8);
        }
    }
    for (uint8_t i = 0; i < 8; i++) {
        if (r & (1 << (7 - i))) {
            PORTB |= (1 << PB0); _delay_us(0.7); PORTB &= ~(1 << PB0); _delay_us(0.6);
        } else {
            PORTB |= (1 << PB0); _delay_us(0.35); PORTB &= ~(1 << PB0); _delay_us(0.8);
        }
    }
    for (uint8_t i = 0; i < 8; i++) {
        if (b & (1 << (7 - i))) {
            PORTB |= (1 << PB0); _delay_us(0.7); PORTB &= ~(1 << PB0); _delay_us(0.6);
        } else {
            PORTB |= (1 << PB0); _delay_us(0.35); PORTB &= ~(1 << PB0); _delay_us(0.8);
        }
    }
}

// Send the entire color data to LEDs
void ws2812_show(void) {
    // Explicitly set the pin low for 50 µs to send the reset signal
    PORTB &= ~(1 << PB0);  // Pull PB0 low
    _delay_us(50);  // Reset signal (hold data line low for at least 50us)
}
