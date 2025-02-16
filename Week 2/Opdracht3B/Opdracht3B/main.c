/*
 * Opdracht3B.c
 *
 * Created: 14/02/2025 13:17:28
 */ 
#define F_CPU 8e6
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
int digit = 0;
bool button1 = false;
bool button2 = false;

void wait( int ms ) {
	for (int i=0; i<ms; i++) {
		_delay_ms( 10 );
	}
}

unsigned char lookup[16] = {
	0b00111111, //0
	0b00000110, //1
	0b01011011, //2
	0b01001111,//3
	0b01100110, //4
	0b01101101,//5
	0b01111101,//6
	0b00000111, //7
	0b01111111, //8
	0b01101111, //9
	0b01110111,//a
	0b01111100,//b
	0b00111001,//c
	0b01011110,//d
	0b01111001,//e
	0b01110001,//f
};

void display(int digit){
	//gaat door de lookup heen met de juiste digit en displayed het cijfer uit de array.
	PORTA = lookup[digit];
	wait(100);
}

ISR( INT0_vect ) {
	//checkButtonsBoth();
	//checkt of de digit boven de 15 is zo ja dan error displayen. 
	if (digit > 15)
	{
		display(14); // display E
		return;
	}
	digit++;
	display(digit);
}

ISR( INT1_vect ) {
	//checkButtonsBoth();
	//checkt of het onder nul komt en zet het display op het juiste cijfer.
	if (digit == 0)
	{
		display(0);
		digit = 0;
		return;
	}
	digit--;
	display(digit);	
}
void checkButtonsBoth(){
	//poging voor het resetten 
	if (button1 && button2)
	{
		display(0);
		digit = 0;
		wait(50);
	}
}

int main(void)
{
  DDRD = 0xF0;			// PORTD(7:4) output, PORTD(3:0) input
  DDRA = 0xFF;

  // Init Interrupt hardware
  EICRA |= 0x0B;			// INT1 falling edge, INT0 rising edge
  EIMSK |= 0x03;			// Enable INT1 & INT0
  
  // Enable global interrupt system
  //SREG = 0x80;			// Of direct via SREG of via wrapper
  sei();
    while (1) 
    {
		
    }
}

