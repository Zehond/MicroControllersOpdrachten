/*
 * Opdracht 5B.c
 *
 * Created: 07/02/2025 15:15:30
 */ 
#define F_CPU 10000000UL
#include <avr/io.h>
#include <util/delay.h>

#define PATTERN_LENGTH 8  


int lightPattern[] = {
	0b10101010,  // Knipperend patroon
	0b01010101,
	0b11110000,  // Eerste helft aan
	0b00001111,  // Tweede helft aan
	0b00000000   // Alles uit
};


void wait(int ms) {
	for (int i = 0; i < ms; i++) {
		_delay_ms(1);
	}
}

int main(void) {
	DDRD = 0xFF;  /* Stel PORTD in als output (8 LEDs)*/

	while (1) {
		for (int i = 0; i < PATTERN_LENGTH; i++) {
			PORTD = lightPattern[i]; /*Stuur patroon naar LEDs*/
			wait(200);
		}
	}
}
