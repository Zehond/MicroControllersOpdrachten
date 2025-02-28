/*
 * Temperatuurmeting met LCD
 * LM35 sensor en ATmega128
 * Datum: 28/02/2025
 */

#define F_CPU 10e6

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h> 

// LCD instellingen
#define LCD_E 	6  // RA6 UNI-6
#define LCD_RS	4  // RA4 UNI-6

void lcd_strobe_lcd_e(void);
void lcd_write_string(char *str);
void lcd_write_data(unsigned char byte);
void lcd_write_command(unsigned char byte);
void lcd_clear(void);
void set_cursor(int position);
void display_text(char *str);
void init(void);

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

// Functie voor wachtvertraging
void wait(int ms) {
	for (int i=0; i<ms; i++) {
		_delay_ms(1);
	}
}


void lcd_clear() {
	lcd_write_command (0x01);//Leeg display
	_delay_ms(2);
	lcd_write_command (0x80);//Cursor terug naar start
}

void adcInit(void) {
	ADMUX  = 0b11100011;  // Vref = 2.56V intern, left adjusted (ADLAR=1), kanaal 3 (PF3)
	ADCSRA = 0b10000110;  // ADC enable, GEEN free-running, prescaler = 64
}

// Start een enkele ADC conversie en retourneert de temperatuur in °C
uint8_t readTemperature(void) {
	ADCSRA |= (1 << ADSC);        // Start ADC conversie
	while (ADCSRA & (1 << ADSC)); // Wachten tot ADC klaar is
	return ADCH;                  // ADC-uitkomst is direct temperatuur in °C
}


int main(void) {
	DDRC = 0xFF;  // PORTC als output voor LCD
	DDRA = 0xFF;  
	init();       
	adcInit();     

	char buffer[16];  // Buffer voor tekst op LCD

	lcd_clear();  // Wis scherm
	display_text("Temp: "); // Statische tekst op LCD

	while (1) {
		// Lees temperatuur
		uint8_t temperatuur = readTemperature();
		sprintf(buffer, "%d C", temperatuur); // Omzetten naar tekst
		
		// Weergeven op LCD
		set_cursor(6);  // Cursor naar juiste plek
		display_text(buffer); // Temperatuur printen
		PORTA = readTemperature();
		wait(500); // Wacht 500ms voor volgende meting
	}

	return 1;
}
