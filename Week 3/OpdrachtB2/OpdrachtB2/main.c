#include <avr/io.h>
#include <avr/interrupt.h>

#define PRESCALER 1024
#define TIMER2_START 22  // Startwaarde voor 15ms
#define HIGH_TIME 146.484375
#define LOW_TIME 244.140625

volatile uint8_t state = 0;  // 0 = laag, 1 = hoog

ISR(TIMER2_OVF_vect) {
	TCNT2 = TIMER2_START;  // Reset timer startwaarde

	if (state) {
		PORTD &= ~(1 << PD7); // Zet laag
		state = 0;
		} else {
		PORTD |= (1 << PD7);  // Zet hoog
		state = 1;
	}
}

void timer2_init() {
	TCCR2 = (1 << CS22) | (1 << CS21) | (1 << CS20); // Prescaler 1024
	TIMSK |= (1 << TOIE2); // Enable Timer2 overflow interrupt
	TCNT2 = TIMER2_START; // Init startwaarde
	sei(); // Globale interrupts aan
}

int main(void) {
	DDRD |= (1 << PD7);  // Zet PD7 als output
	timer2_init(); // Start timer

	while (1) {
		// Hoofdlus, ISR regelt de timing
	}
}
