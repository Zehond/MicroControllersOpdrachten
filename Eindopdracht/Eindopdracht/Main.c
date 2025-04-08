// General:
#define F_CPU 10e6 // 10mHz clock speed

// Libraries:
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdbool.h>

#include "Main.h"

volatile DHT11_Data gottenData; // The data gotten by the DHT11 sensor
volatile DHT11_Data lastGottenData = { 0, 0 }; // The last data gotten by the DHT11 sensor

bool isNegative = false; // Checks if the temperature is negative
DHT11_Data result = {0, 0}; // Result of the DHT11_get_data
int digitsToBeep[4]; // Holds the digit numbers that need to be beeped
int frequenciesToPlay[4] = { 350, 550, 750, 900 }; // Beeping frequecies
uint8_t buttonPressedLast = 0; // Prevent button from being spammed
char buffer[16]; // Buffer to write to Lcd display

/* Busy waiting for 's' second */
void wait_s(int s) {
	for (int i = 0; i < s; i++) {
		_delay_ms(1000);
	}
}

/* Busy waiting for 'ms' milliseconds */
void wait(int ms) {
	for (int i = 0; i< ms; i++) {
		_delay_ms(1);
	}
}

/* Calculates numbers to write to the 7 segment display 
   The total number can be both positive and negative! */
void calculate_digits(int temp) {
	if (temp < 0) {
		temp = -(temp);  // Make positive
	}

	// NOTE: The DHT11 sensor cannot read digits after the ','. We need a DHT22 for those digits.
	// Get individual numbers to display
	int digit1 = temp % 10;       // Single units
	int digit2 = (temp / 10) % 10; // Tens
	int digit3 = (temp / 100) % 10; // Hunderds
	int digit4 = (temp / 1000) % 10; // Thousands
	
	// Display the numbers (0x0A = '-' and 0x0F = empty)
	if (digit4 == 0 && digit3 != 0) { // Temperatures: 100-999
		if (isNegative) {
			display_numbers(digit1, digit2, digit3, 0x0A);
		} else {
			display_numbers(digit1, digit2, digit3, 0x0F);
		}
	} else if (digit4 == 0 && digit3 == 0 && digit2 != 0) { // Temperatures: 10-99
		if (isNegative) {
			display_numbers(digit1, digit2, 0x0A, 0x0F);
		} else {
			display_numbers(digit1, digit2, 0x0F, 0x0F);
		}
	} else if (digit4 == 0 && digit3 == 0 && digit2 == 0) { // Temperatures: 0-9
		if (isNegative) {
			display_numbers(digit1, 0x0A, 0x0F, 0x0F);
		} else {
			display_numbers(digit1, 0x0F, 0x0F, 0x0F);
		}
	} else if (digit4 == 0 && digit3 == 0 && digit2 == 0 && digit1 == 0) { // Temperatures: Exactly 0
		if (isNegative) {
			display_numbers(0, 0x0A, 0x0F, 0x0F);
		} else {
			display_numbers(0, 0x0F, 0x0F, 0x0F);
		}
	}
	
	// Add to array to beep
	digitsToBeep[0] = digit4;
	digitsToBeep[1] = digit3;
	digitsToBeep[2] = digit2;
	digitsToBeep[3] = digit1;
}

/* Initializes Timer1 */
void init_timer1() {
	// Set timer in CTC mode (Clear Timer on Compare Match)
	TCCR1B |= (1 << WGM12);  // (CTC mode)
	
	// Prescaler on 1024
	TCCR1B |= (1 << CS12) | (1 << CS10);
	
	// Value A compare register
	OCR1A = 19530; // 2 second delay time
	
	// Value B compare register
	OCR1B = 78124; // 8 second delay time
	
	// Enable interrupts for Timer1
	TIMSK |= (1 << OCIE1A) | (1 << OCIE1B); // Enable interrupt for compare match A and compare match B
}

/* Timer1 Compare match A interupt for getting DHT11 data */
ISR(TIMER1_COMPA_vect) {
	lastGottenData = gottenData; // Keep track of last data
	gottenData = DHT11_get_data(); // Get temperature and Humidity
	
	//gottenData.temperature = -21; // Example to show negative temperatures
	
	if (gottenData.temperature < 0) { // Mark number as negative if below 0
		isNegative = true;
	} else {
		isNegative = false;
	}
}

/* Helper method to update the Lcd display */
void update_lcd() {
	lcd_clear(); // Clear plus go to first row
	sprintf(buffer, "Temperature: %d", gottenData.temperature);
	display_text(buffer);
	
	set_cursor(2, 0); // Write to second row
	sprintf(buffer, "Humidity: %d", gottenData.humidity);
	display_text(buffer);
}

/* Timer1 Compare Match B interrupt for updating displays */
ISR(TIMER1_COMPB_vect) {
	if (lastGottenData.temperature == gottenData.temperature &&
	lastGottenData.humidity == gottenData.humidity) return; // Only update on new data
	
	if (lastGottenData.temperature != gottenData.temperature) { // Temperature changed
		// Update 7-segmented display
		calculate_digits(gottenData.temperature);
		
		// Update LCD
		update_lcd();
	} else if (lastGottenData.humidity != gottenData.humidity) { // Humidity changed
		// Update LCD
		update_lcd();
	}
}

/* Interrupt for button press (PE0) */
ISR(INT0_vect) {
	if (is_button_pressed() && !buttonPressedLast) {
		// Play beep for all of the digits of the temperature
		buttonPressedLast = 1;
		
		// If the number is negative, beep the '-' symbol on a high frequency
		if (isNegative) {
			beep_for_digit(1, 1500);
		}
		// Beep each digit seperatively
		for (int i = 0; i < sizeof(digitsToBeep)/sizeof(digitsToBeep[0]); i++) {
			beep_for_digit(digitsToBeep[i], frequenciesToPlay[i]);
		}
		_delay_ms(200); // Debounce delay
		buttonPressedLast = 0;
	}
}

/* General init */
void init_all() {
	init_button(); // Initialize button for interrupts
	
	init_buzzer(); // Initialize buzzer
	
	DDRC = 0xFF; // PORTD(7) output, PORTD(6:0) input
	PORTC = 0xFF;  // Set PORTC pin as output for LCD display select
	init_lcd(); // Initialize LCD-screen
	lcd_clear(); // Also clear on startup
	
	init_timer1(); // Initialize Timer1
	
	init_spi(); // Initialize spi module
	init_display_driver(); // Initialize display chip
}

/* Running method */
int main() {
	init_all();
	sei(); // Enable interrupts
	
	while (1) { }
	return (1);
}
