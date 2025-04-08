#include <avr/io.h>

#include "Button.h"

/* Initializes button for interrupts
   Pressed == HIGH, otherwise LOW */
void init_button() {
	BUTTON_DDR &= ~(1 << BUTTON_PIN);  // Set button pin as input
	BUTTON_PORT |= (1 << BUTTON_PIN);  // Set internal pull-up resistance
	
	EICRA |= (1 << ISC01);  // ISC01 = 1 (rising edge trigger)
	EICRA |= (1 << ISC00);  // ISC00 = 1 (rising edge trigger)

	// Enable external interrupt INT0
	EIMSK |= (1 << INT0);  // Enable INT0 interrupt
}

/* Button pressed event */
int is_button_pressed() {
	return (BUTTON_PINE & (1 << BUTTON_PIN));  // Return 1 if button is pressed (HIGH)
}