/*
 * Opdracht1B.c
 *
 * Created: 28/02/2025 12:13:44
 * Author : ST-se
 */ 


#define F_CPU 10e6
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BIT(x)	(1 << (x))

// wait(): busy waiting for 'ms' millisecond
// Used library: util/delay.h
void wait( int ms )
{
	for (int tms=0; tms<ms; tms++)
	{
		_delay_ms( 1 );			// library function (max 30 ms at 8MHz)
	}
}


// Initialize ADC: 10-bits (left justified), free running
void adcInit( void )
{
	ADMUX = 0b01100011;			// AREF=VCC, result left adjusted (ADLAR == 1), channel 3 at pin PF3
	//bit 7-8:vref == vcc. bit 6:ADLAR = 1 left adjusted. bits 4-0: selecteer ADC1 (PF1) dus channel 1 en pf1
	ADCSRA = 0b10000110;		// ADC-enable, no interrupt, start, free running oof, division by 64
	//bit 8: schakel ADC in, bit 6: free running mode uit, bit 2-1: prescaler - 64
	
	//free runnning: blijft continu de data registers updaten, totdat ze geterminated worden. 
}
uint8_t readADC(void)
{
	ADCSRA |= (1 << ADSC);        // Start conversie
	while (ADCSRA & (1 << ADSC)); // Wachten tot conversie klaar is
	return ADCH;                  // Alleen de 8 meest significante bits
}

int main(void)
{
	DDRF = 0x00;  // PORTF als input (ADC)
	DDRA = 0xFF;  // PORTA als output (voor de ADC waarde)

	adcInit();  

	while (1)
	{
		PORTA = readADC(); // Start conversie en toon resultaat
		wait(500);         
	}
}

/* Main program: ADC at PF1
int main( void )
{
	DDRF = 0x00;				// set PORTF for input (ADC)
	DDRA = 0xFF;				// set PORTA for output 
	DDRB = 0xFF;				// set PORTB for output
	adcInit();					// initialize ADC

	while (1)
	{
		PORTB = ADCL;			// Show MSB/LSB (bit 10:0) of ADC 2 andere bits hier
		PORTA = ADCH; //vloeiende beweging door ADLER == 1. 8 belangrijkste bits in ADCH.
		wait(100);				// every 100 ms (busy waiting)
	}
}
*/
