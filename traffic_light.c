#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define RED_LED PB0
#define YELLOW_LED PB1
#define GREEN_LED PB2
#define BUTTON PB3
#define TX_PIN PB4

volatile uint8_t button_pressed = 0;

// Timing configuration
const uint16_t RED_DELAY = 30000;          // 30 seconds
const uint16_t YELLOW_DELAY = 5000;        // 5 seconds
const uint16_t GREEN_DELAY = 30000;        // 30 seconds
const uint16_t FLASH_DELAY = 500;          // 500 ms
const uint16_t COOLDOWN_TIME = 15000;      // 15 seconds cooldown

void setup();
void turn_off_all();
void flash_led(uint8_t led_pin, uint8_t times);
void start_cooldown_timer();
void uart_init(void);
void uart_transmit(char c);
void uart_print(const char *str);

int main(void) {
    setup();
    uart_init();

    while (1) {
        // Start with red light
        turn_off_all();
        PORTB |= (1 << RED_LED);
        uart_print("Red light ON\r\n");
        for (uint16_t i = 0; i < RED_DELAY / 1000; i++) {
            if (button_pressed) {
                button_pressed = 0;
                break;
            }
            _delay_ms(1000);
        }

        // Flash red light 3 times
        flash_led(RED_LED, 3);
        uart_print("Red light flashing\r\n");

        // Yellow light for 5 seconds
        turn_off_all();
        PORTB |= (1 << YELLOW_LED);
        uart_print("Yellow light ON\r\n");
        _delay_ms(YELLOW_DELAY);

        // Green light for 30 seconds
        turn_off_all();
        PORTB |= (1 << GREEN_LED);
        uart_print("Green light ON\r\n");
        _delay_ms(GREEN_DELAY);

        // Flash green light 3 times
        flash_led(GREEN_LED, 3);
        uart_print("Green light flashing\r\n");

        // Yellow light for 5 seconds
        turn_off_all();
        PORTB |= (1 << YELLOW_LED);
        uart_print("Yellow light ON\r\n");
        _delay_ms(YELLOW_DELAY);
    }
}

void setup() {
    // Set LEDs as output
    DDRB |= (1 << RED_LED) | (1 << YELLOW_LED) | (1 << GREEN_LED) | (1 << TX_PIN);
    // Set button as input with pull-up resistor
    DDRB &= ~(1 << BUTTON);
    PORTB |= (1 << BUTTON);

    // Enable pin change interrupt for button (PCINT3)
    GIMSK |= (1 << PCIE);
    PCMSK |= (1 << PCINT3);
    sei(); // Enable global interrupts
}

void turn_off_all() {
    PORTB &= ~((1 << RED_LED) | (1 << YELLOW_LED) | (1 << GREEN_LED));
}

void flash_led(uint8_t led_pin, uint8_t times) {
    for (uint8_t i = 0; i < times; i++) {
        PORTB |= (1 << led_pin); // Turn LED on
        _delay_ms(FLASH_DELAY);
        PORTB &= ~(1 << led_pin); // Turn LED off
        _delay_ms(FLASH_DELAY);
    }
}

// Interrupt Service Routine for button press
ISR(PCINT0_vect) {
    if (!(PINB & (1 << BUTTON))) { // If button is pressed
        if (button_pressed == 0) { // Check if not in cooldown
            button_pressed = 1;
            start_cooldown_timer();
        }
    }
}

// Start hardware timer for cooldown (using Timer0)
void start_cooldown_timer() {
    // Set Timer0 to CTC mode with 1 second delay (assuming prescaler and OCR0A set appropriately)
    TCCR0A = (1 << WGM01);  // CTC mode
    TCCR0B = (1 << CS02) | (1 << CS00); // Prescaler 1024
    OCR0A = 78;  // Approximately 1 second delay at 8 MHz clock with 1024 prescaler
    TIMSK |= (1 << OCIE0A);  // Enable Timer0 compare interrupt
}

// Timer0 Compare Match A Interrupt for cooldown
ISR(TIMER0_COMPA_vect) {
    static uint16_t cooldown_counter = 0;
    cooldown_counter++;
    if (cooldown_counter >= COOLDOWN_TIME / 1000) {
        cooldown_counter = 0;
        TIMSK &= ~(1 << OCIE0A); // Disable Timer0 interrupt
        TCCR0B = 0;  // Stop Timer0
    }
}

// Initialize UART for 9600 baud rate using software-based bit-banging
void uart_init(void) {
    // Set TX_PIN as output
    DDRB |= (1 << TX_PIN);
    PORTB |= (1 << TX_PIN); // Set TX_PIN high (idle)
}

// Transmit a character via UART using bit-banging
void uart_transmit(char c) {
    // Start bit
    PORTB &= ~(1 << TX_PIN); // Set TX_PIN low
    _delay_us(1000000 / 9600); // Delay for 1 bit period (9600 baud)

    // Data bits (LSB first)
    for (uint8_t i = 0; i < 8; i++) {
        if (c & (1 << i)) {
            PORTB |= (1 << TX_PIN); // Send 1
        } else {
            PORTB &= ~(1 << TX_PIN); // Send 0
        }
        _delay_us(1000000 / 9600); // Delay for 1 bit period
    }

    // Stop bit
    PORTB |= (1 << TX_PIN); // Set TX_PIN high
    _delay_us(1000000 / 9600); // Delay for 1 bit period
}

// Transmit a string via UART
void uart_print(const char *str) {
    while (*str) {
        uart_transmit(*str++);
    }
}
