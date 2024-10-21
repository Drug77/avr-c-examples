#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Function prototypes
uint8_t* wheel(uint8_t pos);  // Add this declaration before rainbow_cycle
void ws2812_send_pixel(uint8_t r, uint8_t g, uint8_t b);
void ws2812_show(void);
void set_color(uint8_t r, uint8_t g, uint8_t b);
void rainbow_cycle(uint8_t wait);
void set_pixel_color(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b);
void update_leds(void);

#define LED_COUNT 16
volatile uint8_t mode = 0;  // Start with mode 0
uint8_t led_colors[LED_COUNT][3];

int main(void) {
    // Set PB0 as output for WS2812 data
    DDRB |= (1 << PB0);
    
    // Set PB1 as input for push-button with pull-up resistor
    DDRB &= ~(1 << PB1);
    PORTB |= (1 << PB1);

    // Enable Pin Change Interrupt for PB1 (PCINT1)
    GIMSK |= (1 << PCIE);
    PCMSK |= (1 << PCINT1);
    sei();  // Enable global interrupts

    while (1) {
        update_leds();
        _delay_ms(100);  // Small delay between updates
    }
}

// Interrupt Service Routine to toggle mode
ISR(PCINT0_vect) {
    if (!(PINB & (1 << PB1))) {  // Button pressed
        mode++;  // Cycle through modes
        if (mode > 5) mode = 0;  // Loop back to the first mode
        while (!(PINB & (1 << PB1)));  // Wait for button release
    }
}

void update_leds() {
    switch (mode) {
        case 0:
            set_color(255, 0, 0);  // Red
            break;
        case 1:
            set_color(0, 255, 0);  // Green
            break;
        case 2:
            set_color(0, 0, 255);  // Blue
            break;
        case 3:
            set_color(255, 255, 255);  // White
            break;
        case 4:
            rainbow_cycle(20);  // Rainbow cycle effect
            break;
        case 5:
            set_color(255, 255, 0);  // Yellow
            break;
    }
}

// Function to set all LEDs to the same color
void set_color(uint8_t r, uint8_t g, uint8_t b) {
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        set_pixel_color(i, r, g, b);
    }
    ws2812_show();
}

// Function to create rainbow cycle
void rainbow_cycle(uint8_t wait) {
    uint8_t i, j;
    for (j = 0; j < 256; j++) {
        for (i = 0; i < LED_COUNT; i++) {
            uint8_t* color = wheel(((i * 256 / LED_COUNT) + j) & 255);
            set_pixel_color(i, color[0], color[1], color[2]);
        }
        ws2812_show();
        for (uint8_t k = 0; k < wait; k++) {
            _delay_ms(1);  // Dynamic delay with a loop
        }
    }
}

// Function to map color wheel position to RGB
uint8_t* wheel(uint8_t pos) {
    static uint8_t color[3];
    if (pos < 85) {
        color[0] = pos * 3;
        color[1] = 255 - pos * 3;
        color[2] = 0;
    } else if (pos < 170) {
        pos -= 85;
        color[0] = 255 - pos * 3;
        color[1] = 0;
        color[2] = pos * 3;
    } else {
        pos -= 170;
        color[0] = 0;
        color[1] = pos * 3;
        color[2] = 255 - pos * 3;
    }
    return color;
}

// Function to set an individual pixel color
void set_pixel_color(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b) {
    led_colors[pixel][0] = r;
    led_colors[pixel][1] = g;
    led_colors[pixel][2] = b;
}

// Send pixel data to WS2812 LEDs
void ws2812_send_pixel(uint8_t r, uint8_t g, uint8_t b) {
    // Send GRB order
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

// Send the entire array of LED colors
void ws2812_show(void) {
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        ws2812_send_pixel(led_colors[i][0], led_colors[i][1], led_colors[i][2]);
    }
    // Explicitly set the pin low for 50 µs to send the reset signal
    PORTB &= ~(1 << PB0);  // Pull PB0 low
    _delay_us(50);  // Wait for reset
}
