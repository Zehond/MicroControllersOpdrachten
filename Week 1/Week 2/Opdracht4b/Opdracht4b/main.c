/*
 * Opdracht4b.c
 *
 * Created: 14/02/2025 14:45:58
 */ 
#define  F_CPU 8e6
#include <avr/io.h>
#include <util/delay.h>


void wait(int delay){
	for (int i = 0; i < delay; i++)
	{
		_delay_ms(1);
	}
	
}
typedef struct {
	//struct voor het instellen van de pattern
	unsigned char data;
	unsigned int delay ;
} Table_Pattern;

Table_Pattern Table[] = {
	{0b00000001, 100}, 
	{0b00000010, 200}, 
	{0b00000100, 300}, 
	{0b00001000, 200},
	{0b00001000, 100}, 
	{0b00010000, 400}, 
	{0b00100000, 200}, 
	{0b00010000, 300},
	{0b00001000, 100},
	{0b00000100, 300},
	{0b00000010, 200},
	{0b00000001, 100}
		//table met het patroon
};


int main(void)
{
	DDRA = 0xFF;
    while (1) 
    {
		int index = 0;
		
		while( Table[index].delay != 0 ) {
			// Write data to PORTA
			PORTA = Table[index].data;
			
			wait(Table[index].delay);
			// increment voor volgende in array
			index++;
			//als index bij 12 is dan gaat die weer bij het begin starten
			if (index > 12)
			{
				index = 0;
			}
		}
    }
}

