#ifndef spi_h
#define spi_h

// 7-segment diplay settings:
#define BIT(x)		( 1<<x )
#define DDR_SPI		DDRB	// spi Data direction register
#define PORT_SPI	PORTB	// spi Output register
#define SPI_SCK		1		// PB1: spi Pin System Clock
#define SPI_MOSI	2		// PB2: spi Pin MOSI (master out, slave in)
#define SPI_MISO	3		// PB3: spi Pin MISO (master in, slave out)
#define SPI_SS		0		// PB0: spi Pin Slave Select

// Functions:
void init_spi(void);
void spi_write_word (unsigned char adress, unsigned char data);
void init_display_driver();
void display_numbers(int digit1, int digit2, int digit3, int digit4);

#endif
