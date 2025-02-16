/*
 * Opdracht2B.c
 *
 * Created: 14/02/2025 12:31:14
 */ 


#define F_CPU 8e6
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
volatile uint8_t lampID = 0;

void wait( int ms ) {
	for (int i=0; i<ms; i++) {
		_delay_ms( 10 );		
	}
}

ISR( INT0_vect ) {
	//checken wat lampID is en dan die lamp aan zetten, en lampID verhogen
	if (lampID < 8)
	{
		 PORTD |= (1<<lampID);
		 wait(50);
		 lampID++;
	}	
}

ISR( INT1_vect ) {
	//checken wat lampID is en dan naar beneden gaan en de lamp uit zetten, en lampID verlagen
	if(lampID >= 0){
		if (lampID == 0)
		{
			PORTD &= ~(1<<lampID);
			wait(50);
			return;
		}
		PORTD &= ~(1<<lampID);
		wait(50);
		lampID--;	

	}
}

int main( void ) {
	// Init I/O
	DDRD = 0xF0;			// PORTD(7:4) output, PORTD(3:0) input	

	// Init Interrupt hardware
	EICRA |= 0x0B;			// INT1 falling edge, INT0 rising edge
	EIMSK |= 0x03;			// Enable INT1 & INT0
	
	// Enable global interrupt system
	//SREG = 0x80;			// Of direct via SREG of via wrapper
	sei();				

	while (1) {
	}
	return 1;
}
