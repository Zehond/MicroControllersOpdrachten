#include <avr/io.h>

#include "Spi.h"

/* Init method for the 7 segment display (SPI) */
void init_spi(void) {
	DDR_SPI = 0xff;								// All pins output: MOSI, SCK, SS, SS_display as output
	DDR_SPI &= ~BIT(SPI_MISO);					// 	except: MISO input
	PORT_SPI |= BIT(SPI_SS);					// SS_ADC == 1: deselect slave
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR1);	// or: SPCR = 0b11010010;
	// Enable spi, MasterMode, Clock rate fck/64, bitrate=125kHz
	// Mode = 0: CPOL=0, CPPH=0;
}

/* Write a byte from master to slave */
void spi_write(unsigned char data) {
	SPDR = data;							// Load byte to Data register --> starts transmission
	while(!(SPSR & BIT(SPIF))); 			// Wait for transmission complete
}

/* Select device on pinnumer PORTB (7 segment display) */
void spi_slave_select(unsigned char chipNumber) {
	PORTB &= ~BIT(chipNumber);
}

/* Deselect device on pinnumer PORTB (7 segment display) */
void spi_slave_deselect(unsigned char chipNumber) {
	PORTB |= BIT(chipNumber);
}

/* Write a character to slave */
void spi_write_word (unsigned char adress, unsigned char data) {
	spi_slave_select(0);			// Select display chip
	spi_write(adress);      		// Register 0A: Intensity
	spi_write(data);    			// -> Level in specified range
	spi_slave_deselect(0);			// Deselect display chip
}

/* Clear 7 segment display (display all zero's) */
void clear_segment_display() {
	for (char i = 1; i <= 4; i++) {
		spi_write_word(i, 0);
	}
}

/* Initialize the driver chip (type MAX 7219) */
void init_display_driver() {
	spi_write_word(0x09, 0xFF);
	spi_write_word(0x0A, 0x0F);
	spi_write_word(0x0B, 0x03);
	spi_write_word(0x0C, 0x01);
	
	clear_segment_display(); // Clear when setup is complete
}

/* Helper method to quickly display digits */
void display_numbers(int digit1, int digit2, int digit3, int digit4) {
	spi_write_word(1, digit1);
	spi_write_word(2, digit2);
	spi_write_word(3, digit3);
	spi_write_word(4, digit4);
}