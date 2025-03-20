
#define F_CPU 10e6
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
//7 segment diplay instellingen
#define BIT(x)		( 1<<x )
#define DDR_SPI		DDRB	// spi Data direction register
#define PORT_SPI	PORTB	// spi Output register
#define SPI_SCK		1		// PB1: spi Pin System Clock
#define SPI_MOSI	2		// PB2: spi Pin MOSI (master out, slave in)
#define SPI_MISO	3		// PB3: spi Pin MISO (master in, slave out)
#define SPI_SS		0		// PB0: spi Pin Slave Select

//all methods

//temperatuur
void adcInit(void);
uint8_t readTemperature(void);
//7 segment display
void spi_masterInit(void);
void spi_write( unsigned char data );
void spi_slaveSelect(unsigned char chipNumber);
void spi_slaveDeSelect(unsigned char chipNumber);
void spi_writeWord ( unsigned char adress, unsigned char data );
void writeLedDisplayNegatieve(int value);
void displayDriverInit();
void displayOn();
void displayOff();


// wait(): busy waiting for 'ms' millisecond
// library: util/delay.h
void wait(int ms)
{
	for (int i=0; i<ms; i++)
	{
		_delay_ms(1);
	}
}



//temperatuur
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


//7 segment display
void spi_masterInit(void)
{
	DDR_SPI = 0xff;							// All pins output: MOSI, SCK, SS, SS_display as output
	DDR_SPI &= ~BIT(SPI_MISO);				// 	except: MISO input
	PORT_SPI |= BIT(SPI_SS);				// SS_ADC == 1: deselect slave
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1);	// or: SPCR = 0b11010010;
	// Enable spi, MasterMode, Clock rate fck/64, bitrate=125kHz
	// Mode = 0: CPOL=0, CPPH=0;
}

// Write a byte from master to slave
void spi_write( unsigned char data )
{
	SPDR = data;							// Load byte to Data register --> starts transmission
	while( !(SPSR & BIT(SPIF)) ); 			// Wait for transmission complete
}

// Select device on pinnumer PORTB
void spi_slaveSelect(unsigned char chipNumber)
{
	PORTB &= ~BIT(chipNumber);
}

// Deselect device on pinnumer PORTB
void spi_slaveDeSelect(unsigned char chipNumber)
{
	PORTB |= BIT(chipNumber);
}

void spi_writeWord ( unsigned char adress, unsigned char data ){
	spi_slaveSelect(0);				// Select dispaly chip
	spi_write(adress);      		// Register 0A: Intensity
	spi_write(data);    			//  -> Level in specified range
	spi_slaveDeSelect(0);			// Deselect display chip
}

void writeLedDisplayNegatieve(int value)
{
	int isNegative = 0;
	if (value < 0) {
		isNegative = 1;       // Markeer dat het een negatief getal is
		value = -value;       // Zet de waarde om naar positief
	}
	int lastZero;
	// Verkrijg de individuele cijfers van de waarde
	int digit1 = value % 10;       // Het eenheden cijfer
	int digit2 = (value / 10) % 10; // Het tientallen cijfer
	int digit3 = (value / 100) % 10; // Het honderdtallen cijfer
	int digit4 = (value / 1000) % 10; // Het duizendtallen cijfer
	
	// Stel de overige cijfers in op het display (de cijfers worden van rechts naar links weergegeven)
	spi_writeWord(1, digit1);
	spi_writeWord(2, digit2);  // Digit 2 (honderdtallen)
	spi_writeWord(3, digit3);  // Digit 3 (tientallen)
	spi_writeWord(4, digit4);  // Digit 4 (eenheden)

	// Stel het minteken in op de eerste digit als het getal negatief is
	if (isNegative) {
		// Vind de laatste positie die niet 0 is, dit wordt de plaats van de '-'
		int zeroPos = 4;
		if (digit4 == 0){
			zeroPos = 4;
		}
		if (digit3 == 0){
			zeroPos = 3;
			spi_writeWord(4, 0x0F);
		}
		if (digit2 == 0){
			zeroPos = 2;
			spi_writeWord(4, 0x0F);
			spi_writeWord(3, 0x0F);
		}
		spi_writeWord(zeroPos, 0x0A);
	}
}

// Initialize the driver chip (type MAX 7219)
void displayDriverInit()
{
	spi_writeWord(0x09,0xFF);

	spi_writeWord(0x0A,0x0F);

	spi_writeWord(0x0B,0x03);

	spi_writeWord(0x0C,0x01);
}

// Set display on ('normal operation')
void displayOn()
{
	spi_slaveSelect(0);				// Select display chip
	spi_write(0x0C); 				// Register 0B: Shutdown register
	spi_write(0x01); 				// 	-> 1 = Normal operation
	spi_slaveDeSelect(0);			// Deselect display chip
}

// Set display off ('shut down')
void displayOff()
{
	spi_slaveSelect(0);				// Select display chip
	spi_write(0x0C); 				// Register 0B: Shutdown register
	spi_write(0x00); 				// 	-> 1 = Normal operation
	spi_slaveDeSelect(0);			// Deselect display chip
}



int main()
{
	// inilialize
	DDRB=0x01;				// Set PB0 pin as output for display select
	spi_masterInit();       // Initialize spi module
	displayDriverInit();// Initialize display chip
	adcInit();

	char buffer[16];  // Buffer voor tekst op LCD

	// clear display (all zero's)
	for (char i =1; i<=4; i++)
	{
		spi_writeWord(i,0);
	}
	
		
	while (1) {
		// Lees temperatuur
		uint8_t temperatuur = readTemperature();
		writeLedDisplayNegatieve(temperatuur);
		wait(500); // Wacht 500ms voor volgende meting
	}

	

	return (1);
}

