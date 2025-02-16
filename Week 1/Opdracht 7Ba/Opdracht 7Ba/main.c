/*
 * Opdracht 7Ba.c
 *
 * Created: 08/02/2025 13:17:02
 */ 
#define F_CPU 10000000UL
#include <avr/io.h>
#include <util/delay.h>

typedef enum{
	START,
	S1,
	S2,
	S3,
	END
	} State_t;
	
	State_t currentState = START;
/*prev state is om te kijken wat de state was van de knop, anders blijft die doorgaan als je de knop ingedrukt houd inplaats van dat je het 1 keer wilt doen*/

int detecterenKnop(int pin, int * prevState){
	int current = PIND & (1 << pin);
	int knopIngedrukt = (!(*prevState) && current);
	*prevState = current;
	printf("statusCurrent: %s," "statusPREv: %s," "KNOP: %s",current, *prevState, knopIngedrukt );
	_delay_ms(100);
	return knopIngedrukt;
}


int main(void)
{
    DDRD = 0x00; /*zet PORTDs als input knopppen*/
	PORTD = 0xFF; /*pull up weerstand*/
	
	int prevD5 = 0, prevD6 = 0, prevD7 = 0;/*standaard is uit*/
    while (1) 
    {
		switch (currentState)
		{
			case START:
			if(detecterenKnop(PD6, &prevD6)) currentState = S1;
			else if(detecterenKnop(PD5, &prevD5)) currentState = S2;
			break;
			case S1:
			if(detecterenKnop(PD7, &prevD7)) currentState = START;
			else if (detecterenKnop(PD5, &prevD5)) currentState = S2;
			break;
			case S2:
			if (detecterenKnop(PD6,&prevD6)) currentState = S1;
			else if (detecterenKnop(PD7, &prevD7)) currentState = START;
			else if (detecterenKnop(PD5, &prevD5)) currentState = S3;
			break;
			case S3:
			if (detecterenKnop(PD7, &prevD7)) currentState = START;
			else if (detecterenKnop(PD5, &prevD5) || detecterenKnop(PD6, &prevD6)) currentState = END;
			break;
			case END:
			if (detecterenKnop(PD7, &prevD7)) currentState = START;
			break;
			
		}
		_delay_ms(100); 
		
    }
}

