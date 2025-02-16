/*
 * Opdracht 6B.c
 *
 * Created: 07/02/2025 15:33:50
 */ 

/* B. 6 */
#define F_CPU 10000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	PORTC |= (1<<PC0); /* Uit is 0, aan is 1. */
	
	DDRD = 0xFF; /* Zet rij D als output */
	/* Replace with your application code */
	while (1)
	{
		int val = 0b01000000;
		/* Knipper per 1 sec.*/
		PORTD = val; /* Toon 1 lampje op basis van val waarde */
		_delay_ms(1000); /* Wacht 1 sec */
		val = 0b00000000;
		PORTD = val;
		_delay_ms(1000); /* Wacht 1 sec */
		
		if (PINC & (1<<PC0)) /* Kijken of de knop ingedrukt is */
		{
			while (1)
			{
				int val = 0b01000000;
				/* Knipper per 0,25 sec.*/
				PORTD = val; /* Toon 1 lampje op basis van val waarde */
				_delay_ms(250);
				val = 0b00000000;
				PORTD = val;
				_delay_ms(250);
				
				if (PINC & (1<<PC0)) /* Kijken of de knop ingedrukt is */
				{
					break;
				}
			}
		}
	}
}

