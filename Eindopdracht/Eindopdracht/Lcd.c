#define F_CPU 8e6 // 8mHz clock speed
// WE NEED THIS FOR THE LCD TO NOT SHOW GIBBERISH

#include <avr/io.h>
#include <util/delay.h>

#include "Lcd.h"

/* Helper method */
void lcd_strobe_lcd_e(void) {
	PORTA |= (1<<LCD_E);	// E high
	_delay_ms(1);			// Time needed to register
	PORTA &= ~(1<<LCD_E);  	// E low
	_delay_ms(1);			// Time needed to register
}

/* Init LCD-screen registers */
void init_lcd() {
	// PORTC output mode and all low (also E and RS pin)
	DDRD = 0xFF;
	DDRA = 0xFF;
	PORTC = 0x00;
	PORTA = 0x00;

	// Step 2 (table 12)
	PORTC = 0x20;	// Set first row
	lcd_strobe_lcd_e();
	PORTC = 0x20;	// Set second row
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

/* Writes data to the LCD-screen */
void lcd_write_data(unsigned char byte) {
	// First nibble.
	PORTC = byte; // Save data
	PORTA |= (1<<LCD_RS); // Code 1 = sending text
	lcd_strobe_lcd_e(); // Send data

	// Second nibble
	PORTC = (byte<<4);
	PORTA |= (1<<LCD_RS);
	lcd_strobe_lcd_e();
}

/* Display text on the LCD-screen */
void display_text(char *str) {
	for(;*str; str++){ // Write each character in the string individually
		lcd_write_data(*str);
	}
}

/* Writes a command to the LCD-screen */
void lcd_write_command(unsigned char byte) {
	// First nibble.
	PORTC = byte; // Save data
	PORTA &= ~(1<<LCD_RS); // Code 0 = sending command
	lcd_strobe_lcd_e(); // Send data

	// Second nibble
	PORTC = (byte<<4);
	PORTA &= ~(1<<LCD_RS);
	lcd_strobe_lcd_e();
}

/* Sets the cursor on the LCD-screen */
void set_cursor(int row, int position) {
	if (row != 1 && row != 2) return;
	
	if (row == 1) { // 15 is the max width on the LCD-screen
		lcd_write_command(0x80 + position);
	} else if (row == 2) {
		lcd_write_command(0xC0 + position);
	}
	//0x80 is the hex for the standard position of the cursor (16x2 so above row on the left)
}

/* Clears the LCD-screen */
void lcd_clear() {
	lcd_write_command (0x01); // Empty display
	_delay_ms(2); // Time needed to register
	lcd_write_command (0x80); // Set cursor to start
}