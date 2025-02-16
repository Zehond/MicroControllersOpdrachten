/*
 * Opdracht 4B.c
 *
 * Created: 07/02/2025 15:34:49
 */ 

C
/* B. 4 */
#define F_CPU 10000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRD = 0xFF; /* Zet rij D als output */
	int val = 0b00000001;
	/* Replace with your application code */
	while (1)
	{
		PORTD = val; /* Toon 1 lampje op basis van val waarde */
		_delay_ms(50); /* Wacht 50ms */
		val = val<<1; /* Verschuif de 1 een positie naar links */
		if (val == 0b100000000) /* Als het patroon de maximale waarde heeft, reset terug naar het begin. */
		{
			val = 0b00000001;
		}
	}
}