/*
 * Opdracht B2.c
 *
 * Created: 07/02/2025 13:39:26
 * Author : ST-se
 */ 

#define F_CPU 10000000UL
#include <avr/io.h>
#include <util/delay.h>
/*Method for delay input*/
void wait(int ms){
	for (int i = 0; i<ms; i++)
	{
		_delay_ms(1);
	}
}

int main(void)
{
	/*Setting row of LEDS to input or output*/
	DDRD = 0xFF;
    /* 1 is for on and 0 is for off, PORTA means row A of the LEDS*/
    while (1) 
    {
		PORTD = 0b01000000;
		wait(500);
		PORTD = 0b00100000;
		wait(500);
    }
	return 1;
}

