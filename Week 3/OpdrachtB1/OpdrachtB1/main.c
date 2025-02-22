#define F_CPU 10e6

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define LCD_E 	6  // RA6 UNI-6
#define LCD_RS	4  // RA4 UNI-6

void lcd_strobe_lcd_e(void);
void lcd_write_string(char *str);
void lcd_write_data(unsigned char byte);
void lcd_write_cmd(unsigned char byte);
void lcd_clear(void);
void display_text(char *str);
void init();
void wait(int ms);

// **Strobe signaal voor de LCD**
void lcd_strobe_lcd_e(void) {
	PORTA |= (1 << LCD_E);   // E hoog
	_delay_ms(1);            // Kleine vertraging
	PORTA &= ~(1 << LCD_E);  // E laag
	_delay_ms(1);
}

// **Stuur een string naar de LCD**
void lcd_write_string(char *str) {
	while (*str) {
		lcd_write_data(*str);
		str++;
	}
}

// **Stuur data (tekst) naar de LCD**
void lcd_write_data(unsigned char byte) {
	// Eerste nibble verzenden
	PORTC = byte;
	PORTA |= (1 << LCD_RS);  // RS = 1 (data)
	lcd_strobe_lcd_e();

	// Tweede nibble verzenden
	PORTC = (byte << 4);
	PORTA |= (1 << LCD_RS);
	lcd_strobe_lcd_e();
}

// **Stuur een command naar de LCD**
void lcd_write_cmd(unsigned char byte) {
	// Eerste nibble verzenden
	PORTC = byte;
	PORTA &= ~(1 << LCD_RS);  // RS = 0 (command)
	lcd_strobe_lcd_e();

	// Tweede nibble verzenden
	PORTC = (byte << 4);
	PORTA &= ~(1 << LCD_RS);
	lcd_strobe_lcd_e();
}

// **Wis het LCD-scherm**
void lcd_clear(void) {
	lcd_write_cmd(0x01);  // Clear display
	_delay_ms(2);
	lcd_write_cmd(0x80);  // Cursor terug naar startpositie
}

// **Tekst weergeven op het LCD**
void display_text(char *str) {
	lcd_clear();  // Maak het scherm leeg
	lcd_write_string(str);
}

// **Cursor verplaatsen**
void set_cursor(int position) {
	if (position < 0 || position > 15) return;
	lcd_write_cmd(0x80 + position);  // 0x80 is de beginpositie van de cursor
}

// **LCD initialiseren**
void init(void) {
	// Poorten instellen
	DDRC = 0xFF;   // LCD data-uitgangen
	DDRA = 0xFF;   // LCD besturing

	PORTC = 0x00;
	PORTA = 0x00;

	// Initialisatieproces
	_delay_ms(15);

	PORTC = 0x20;  // Function set
	lcd_strobe_lcd_e();

	PORTC = 0x20;
	lcd_strobe_lcd_e();
	PORTC = 0x80;
	lcd_strobe_lcd_e();

	PORTC = 0x00;
	lcd_strobe_lcd_e();
	PORTC = 0xF0;
	lcd_strobe_lcd_e();

	PORTC = 0x00;
	lcd_strobe_lcd_e();
	PORTC = 0x60;
	lcd_strobe_lcd_e();

	lcd_clear();  // Wis het display bij opstarten
}

// **Wachtfunctie**
void wait(int ms) {
	for (int i = 0; i < ms; i++) {
		_delay_ms(1);
	}
}
// **Toon tellerwaarde op LCD**
void display_count(uint8_t count) {
	char buffer[16];
	sprintf(buffer, "Count: %d", count);
	lcd_clear();
	display_text(buffer);
}

// **Timer2 instellen als teller (Counter Mode)**
void counter2_init() {
	DDRD &= ~(1 << PD7);   // PD7 als input (T2 pin)
	PORTD |= (1 << PD7);   // Optioneel: Pull-up weerstand aan (indien nodig)

	TCCR2 = 0b00000111; // Externe klok op stijgende flank
	TCNT2 = 0x00;       // Teller resetten
}

// **LCD en hardware initialiseren**
void init_hardware() {
	DDRA = 0xFF;  // LCD aansturing
	DDRC = 0xFF;  // LCD data
	counter2_init(); // Start Timer2 teller
	init();         // Init LCD
	lcd_clear();
	display_text("Ready...");
}

int main(void) {
	init_hardware();
	uint8_t prev_count = 0;

	while (1) {
		uint8_t new_count = TCNT2;  // Lees tellerwaarde

		if (new_count != prev_count) {  // Alleen updaten als waarde verandert
			prev_count = new_count;
			display_count(new_count);
		}

		wait(250);
	}

	return 0;
}	
