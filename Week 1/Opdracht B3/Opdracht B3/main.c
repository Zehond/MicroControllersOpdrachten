/*
 * Opdracht B3.c
 *
 * Created: 07/02/2025 13:49:22
 */ 
#define F_CPU 10000000UL
#include <avr/io.h>
#include <util/delay.h>
/*Method for delay input*/

void wait(int ms){
	for (int i = 0; i < ms; i++) {
		_delay_ms(1);
	}
}

int main(void)
{
	
	/*zet de nummer 7 op 1 dus output*/
	DDRD |= (1 << PD7);
	/*zet als input (0)*/
	DDRC &= ~(1 << PC0);
	/*zet pull up weerstand voor pc0, zodat je schakel hebt tussen wel en geen licht  dus laag en hoog*/
	PORTC |= (1 << PC0);

	while (1) {
	/*controleren of knop is ingedrukt*/
		if ((PINC & (1 << PC0))) {
/*knipper led*/
			PORTD ^= (1 << PD7);
			wait(500); 
			} else {
			/*led uit als knop is ingedrukt*/
			PORTD &= ~(1 << PD7);
		}
	}
	
}


