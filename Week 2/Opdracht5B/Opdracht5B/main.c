/*
*
*14/02/2025
*/

#define F_CPU 10e6

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "main.h"

#define LCD_E 	6  // RA6 UNI-6
#define LCD_RS	4  // RA4 UNI-6

void lcd_strobe_lcd_e(void);
void lcd_write_string(char *str);
void lcd_write_data(unsigned char byte);
void lcd_write_cmd(unsigned char byte);
void lcd_clear(void);


void wait( int ms ) {
	for (int i=0; i<ms; i++) {
		_delay_ms( 1 );	
	}
}
void display_text(char *str){
	for(;*str; str++){//loopt door de tekst heen.
		lcd_write_data(*str);
	}
}
void init(){
	// PORTC output mode and all low (also E and RS pin)
	DDRD = 0xFF;
	DDRA = 0xFF;
	PORTC = 0x00;
	PORTA = 0x00;
	//PORTA = 0xFF;

	// Step 2 (table 12)
	PORTC = 0x20;	// function set
	lcd_strobe_lcd_e();

	// Step 3 (table 12)
	PORTC = 0x20;   // function set
	lcd_strobe_lcd_e();
	PORTC = 0x80;
	lcd_strobe_lcd_e();

	// Step 4 (table 12)
	PORTC = 0x00;   // Display on/off control
	lcd_strobe_lcd_e();
	PORTC = 0xF0;
	lcd_strobe_lcd_e();

	// Step 4 (table 12)
	PORTC = 0x00;   // Entry mode set
	lcd_strobe_lcd_e();
	PORTC = 0x60;
	lcd_strobe_lcd_e();
}
void set_cursor(int position){
	if (position < 0 || position > 15) return;  //het kan niet hoger dan 15, je moet dan op de 2de rij zien te komen nog niet gelukt

	lcd_write_command(0x80 + position);
	//0x80 is de hex voor de standaard plek voor de cursor. dit is 16x2 dus dit zou juist nu 0 moeten zijn. dus links boven
}

void lcd_strobe_lcd_e(void) {
	PORTA |= (1<<LCD_E);	// E high
	_delay_ms(1);			// nodig
	PORTA &= ~(1<<LCD_E);  	// E low
	_delay_ms(1);			// nodig?
}



void lcd_write_data(unsigned char byte) {
	// First nibble.
	PORTC = byte;//opslaan data
	PORTA |= (1<<LCD_RS);// code 1 want het geeft aan dat het een tekst is.
	lcd_strobe_lcd_e();//versturen van data

	// Second nibble
	PORTC = (byte<<4);
	PORTA |= (1<<LCD_RS);
	lcd_strobe_lcd_e();
}


void lcd_write_command(unsigned char byte) {
	// First nibble.
	PORTC = byte;
	PORTA &= ~(1<<LCD_RS);// code 0 voor aangeven het is een commando
	lcd_strobe_lcd_e();

	// Second nibble
	PORTC = (byte<<4);
	PORTA &= ~(1<<LCD_RS);
	lcd_strobe_lcd_e();
}


void lcd_clear() {
	lcd_write_command (0x01);						//Leeg display
	_delay_ms(2);
	lcd_write_command (0x80);						//Cursor terug naar start
}

void main(void){
	// Init I/O
	DDRC = 0xFF;			// PORTD(7) output, PORTD(6:0) input
	PORTC = 0xFF;

	// Init LCD
	init();
	
	_delay_ms(10);
	
	lcd_clear();

	// Write sample string
	display_text("Hello world!!!!");
	
	set_cursor(5);

	// Loop forever
	while (1) {
		PORTC ^= (1<<0);	// Toggle PORTD.7
		wait( 250 );
	}

	return 1;
}