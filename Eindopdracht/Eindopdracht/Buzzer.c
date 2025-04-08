#define F_CPU 10e6 // 10mHz clock speed

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

#include "Buzzer.h"

/* Busy waiting for 'us' microseconds */
void delay_us_variable(int us) {
	for (int i = 0; i < us; i++) {
		_delay_us(1); // 1 µs fixed delay
	}
}

/* Initialization method for the built in buzzer */
void init_buzzer() {
	DDRC |= (1 << PE1);  // Set PE1 (RC1) as output
}

/* Custom method to create 'BEEP' sound
   This code was written with help from AI! */
void play_beep_sound(uint16_t frequency, uint16_t durationMs) {
	uint32_t delayTime = (500000UL / frequency); // Half-period (half of total time) in µs
	uint32_t numCycles = ((uint32_t)durationMs * 1000) / (delayTime * 2UL); // Amount of cycles ON/OFF
	
	for (uint32_t i = 0; i < numCycles; i++) {
		PORTE |= (1 << PE1);  // Turn buzzer ON
		delay_us_variable(delayTime);
		PORTE &= ~(1 << PE1);  // Turn buzzer OFF
		delay_us_variable(delayTime);
	}
}

/* Beeps for a digit with a certain amount and frequency */
void beep_for_digit(int digit, int frequency) {
	if (digit == 0) return;
	
	for (uint32_t i = 0; i < digit; i++) {
		play_beep_sound(frequency, 100);  // Beep at set frequency for 100 milliseconds
		_delay_ms(100); // Wait 100 milliseconds before next beep
	}
}