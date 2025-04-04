// General:
#define F_CPU 10e6 // 10mHz clock speed

// Libraries:
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdbool.h>

// Settings:
//7-segment diplay settings
#define BIT(x)		( 1<<x )
#define DDR_SPI		DDRB	// spi Data direction register
#define PORT_SPI	PORTB	// spi Output register
#define SPI_SCK		1		// PB1: spi Pin System Clock
#define SPI_MOSI	2		// PB2: spi Pin MOSI (master out, slave in)
#define SPI_MISO	3		// PB3: spi Pin MISO (master in, slave out)
#define SPI_SS		0		// PB0: spi Pin Slave Select

//DHT11 sensor settings
#define DHT11_PIN PD0  // DHT11 DATA pin
#define DHT11_PORT PORTD
#define DHT11_DDR DDRD
#define DHT11_PINC PIND

//Button settings
#define BUTTON_PIN PA2
#define BUTTON_PORT PORTA
#define BUTTON_DDR DDRA
#define BUTTON_PINA PINA

//General settings
volatile uint8_t latestTemperature = 0; // Latest read temperature
int isNegative = 0; // Checks if the temperature is negative
int digitsToBeep[4]; // Holds the digit numbers that need to be beeped
int frequenciesToPlay[4] = { 350, 550, 750, 900 }; // Beeping frequecies

/* Busy waiting for 's' second */
void wait_s(int s) {
	for (int i = 0; i < s; i++) {
		_delay_ms(1000);
	}
}

/* Initializes button for interrupts */
//button when pressed is on low en normal is high
void init_button() {
	BUTTON_DDR &= ~(1 << BUTTON_PIN);  // Set button pin as input
	BUTTON_PORT |= (1 << BUTTON_PIN);  // Set internal pull-up resistance
}

/* Button pressed event */
int is_button_pressed() {
	return !(BUTTON_PINA & (1 << BUTTON_PIN));  // Return 1 if button is pressed (LOW)
}

/* Custom method to delay for X microseconds
   This code was made by AI! */
void delay_us_variable(uint16_t us) {
	while (us--) {
		_delay_us(1);  // 1 탎 fixed delay inside loop
	}
}

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

/* Custom method to create 'BEEP' sound
   This code was written with help from AI! */
void play_beep_sound(uint16_t frequency, uint16_t durationMs) {
	uint32_t delayTime = (500000UL / frequency); // Half-period (half of total time) in 탎
	uint32_t numCycles = ((uint32_t)durationMs * 1000) / (delayTime * 2UL); // Amount of cycles ON/OFF

	if (isNegative){
		PORTC |= (1 << PC1);  // Turn buzzer ON
		delay_us_variable(1000);
		PORTC &= ~(1 << PC1);  // Turn buzzer OFF
		delay_us_variable(1000);
	}
	
	for (uint32_t i = 0; i < numCycles; i++) {
		PORTC |= (1 << PC1);  // Turn buzzer ON
		delay_us_variable(delayTime);
		PORTC &= ~(1 << PC1);  // Turn buzzer OFF
		delay_us_variable(delayTime);
	}
}

/* Beeps for a digit with a certain amount and frequency */
void beep_for_digit(int digit, int frequency) {
	if (digit == 0) {
		return;
	}
	
	for (uint32_t i = 0; i < digit; i++) {
		play_beep_sound(frequency, 100);  // Beep at set frequency for 100 milliseconds
		_delay_ms(100); // Wait 100 milliseconds before next beep
	}
}

/* Helper method to quickly display digits */
void display_numbers(int digit1, int digit2, int digit3, int digit4) {
	spi_write_word(1, digit1);
	spi_write_word(2, digit2);
	spi_write_word(3, digit3);
	spi_write_word(4, digit4);
}

/* Writes a number value to the 7 segment display 
   The number can be both positive and negative, and include decimals */
void calculate_digits(int temp) {
	isNegative = 0;
	if (temp < 0) {
		isNegative = 1;	// Mark number as negative
		temp = -(temp);  // Make positive
	}

	// NOTE: The DHT11 sensor cannot read digits after the ','. We need a DHT22 for those digits.
	// Get individual numbers to display
	int digit1 = temp % 10;       // Single units
	int digit2 = (temp / 10) % 10; // Tens
	int digit3 = (temp / 100) % 10; // Hunderds
	int digit4 = (temp / 1000) % 10; // Thousands
	
	// Display the numbers (0x0A = '-' and 0x0F = empty)
	if (digit4 == 0 && digit3 != 0 && digit2 != 0 && digit1 != 0) {
		if (isNegative) {
			display_numbers(digit1, digit2, digit3, 0x0A);
		} else {
			display_numbers(digit1, digit2, digit3, 0x0F);
		}
	} else if (digit4 == 0 && digit3 == 0 && digit2 != 0 && digit1 != 0) {
		if (isNegative) {
			display_numbers(digit1, digit2, 0x0A, 0x0F);
		} else {
			display_numbers(digit1, digit2, 0x0F, 0x0F);
		}
	} else if (digit4 == 0 && digit3 == 0 && digit2 == 0) {
		if (isNegative) {
			display_numbers(digit1, 0x0A, 0x0F, 0x0F);
		} else {
			display_numbers(digit1, 0x0F, 0x0F, 0x0F);
		}
	} else if (digit4 == 0 && digit3 == 0 && digit2 != 0 && digit1 == 0) {
		if (isNegative) {
			display_numbers(0, 0x0A, 0x0F, 0x0F);
		} else {
			display_numbers(0, 0x0F, 0x0F, 0x0F);
		}
	}
	
	digitsToBeep[0] = digit4;
	digitsToBeep[1] = digit3;
	digitsToBeep[2] = digit2;
	digitsToBeep[3] = digit1;
}

void clear_segment_display() {
	// Clear 7 segment display (display all zero's)
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
	
	clear_segment_display();
}

/* Initialization method for the built in buzzer */
void init_buzzer() {
	DDRC |= (1 << PC1);  // Set PC1 (RC1) as output
}

/* The microcontroller sends a start signal:
	Pulls the line LOW for 18ms.
	Pulls the line HIGH for 20-40탎.
	The microcontroller switches the pin to input mode to listen.
	The DHT11 responds by pulling the line LOW and then HIGH (which DHT11_response() detects).
If the response is valid, the microcontroller can proceed to read temperature and humidity data. */
/* Requests the data from the DHT11 sensor 
   This code was written with help from AI! */
void DHT11_request() {
	DHT11_DDR |= (1 << DHT11_PIN);  // Set DHT11 pin as output. This is necessary because the microcontroller must actively drive the data line LOW to send the start signal.
	DHT11_PORT &= ~(1 << DHT11_PIN); // Pull low for 18ms. This (&=) operation clears the bit, setting the pin LOW.
	_delay_ms(18); // Long wait indicates start signal.
	DHT11_PORT |= (1 << DHT11_PIN);  // Pull high for 20-40us. This releases the bus, allowing the DHT11 to take control.
	_delay_us(30);
	DHT11_DDR &= ~(1 << DHT11_PIN); // Set pin as input
}

/* The microcontroller sends a start signal (pulls the data line LOW for ~18ms and then releases it).
	The DHT11 responds:
	Pulls the line LOW for 80 탎.
	Pulls the line HIGH for 80 탎.
	The microcontroller calls DHT11_response() to verify this response.
If successful, the microcontroller proceeds to read data bytes (like humidity and temperature).*/
/* Responds the data from the DHT11 sensor 
   This code was written with help from AI! */
uint8_t DHT11_response() {
	_delay_us(40); // Wait so that the request can be sent first.
	if (!(DHT11_PINC & (1 << DHT11_PIN))) { // Waits for pin LOW. Which confirms the sensor has responded.
		_delay_us(80); // This aligns with the DHT11 protocol, where the sensor keeps the line LOW for 80 탎 before pulling it HIGH.
		if (DHT11_PINC & (1 << DHT11_PIN)) { // Waits for pin HIGH. If the pin is HIGH, it confirms that the sensor has acknowledged the request and is ready to send data.
			_delay_us(80); // Ensures synchronization with the sensor뭩 response phase.
			return 1; // Successful response.
		}
	}
	return 0; // Unsuccessful response.
}

/* The DHT11 sensor sends each bit with a specific timing:
	"0" bit: LOW ? HIGH (26-28 탎) ? LOW
	"1" bit: LOW ? HIGH (70 탎) ? LOW
The function uses the 30 탎 delay to differentiate between 0 and 1. */
/* Reads a single byte (8 bits) from the DHT11 sensor 
   This code was written with help from AI! */
uint8_t DHT11_read_byte() {
	uint8_t i, result = 0;
	for (i = 0; i < 8; i++) { // 8 bits per byte.
		while (!(DHT11_PINC & (1 << DHT11_PIN)));  // Waits for pin HIGH. Ensures synchronization with the sensor's data transmission.
		_delay_us(30); // Wait before checking the pin again.
		if (DHT11_PINC & (1 << DHT11_PIN))  // If pin is still HIGH, bit is 1. Otherwise it's 0. The most significant bit (MSB) is received first, so the bit is placed in position (7 - i).
		result |= (1 << (7 - i)); // Add byte to the result array.
		while (DHT11_PINC & (1 << DHT11_PIN)); // Waits for pin LOW. This marks the end of the bit transmission before moving to the next bit.
	}
	return result; // Returns array of bits.
}

/* Method that gets the data from the DHT11 sensor 
   This code was written with help from AI! */
uint8_t DHT11_get_data() {
	uint8_t humInt, humDec, tempInt, tempDec, checksum;
	
	DHT11_request(); // Initiate communication with the DHT11.
	if (DHT11_response()) { // Get either 1 or 0 based on if there is a response.
		// Read bytes using custom digital one-wire protocol, which consists of 5 bytes.
		humInt = DHT11_read_byte();
		humDec = DHT11_read_byte();
		tempInt = DHT11_read_byte();
		tempDec = DHT11_read_byte();
		checksum = DHT11_read_byte();

		if ((humInt + humDec + tempInt + tempDec) == checksum) { // The checksum checks if the sent data is correct
			// If the data is correct, return the temperature
			return tempInt; // temperature (캜) is tempInt, because tempDec is always 0 using a DHT11 sensor.
		}
	}
	return 0; // If no data was read
}

/* Initializes Timer1 */
void init_timer1() {
	// Set timer in CTC mode (Clear Timer on Compare Match)
	TCCR1B |= (1 << WGM12);  // (CTC mode)
	
	// Prescaler on 1024
	TCCR1B |= (1 << CS12) | (1 << CS10);
	
	// Value compare register (for 2 seconds)
	OCR1A = 19530;  // 2 seconds delay time for prescaler
	
	// Enable interrupts for Timer1
	TIMSK |= (1 << OCIE1A);  // Enable interrupt for compare match A
}

/* Timer1 */
ISR(TIMER1_COMPA_vect) {
	// Get temperature
	latestTemperature = DHT11_get_data();
	
	// Write to 7-segmented display
	calculate_digits(latestTemperature);
}

/* Running method */
int main() {
	DDRB=0x01; // Set PB0 pin as output for display select
	init_buzzer(); // Initialize buzzer
	init_spi(); // Initialize spi module
	init_display_driver(); // Initialize display chip
	init_timer1(); // Initialize Timer1
	init_button(); // Initialize button for interrupts
	sei(); // Enable interrupts
	
	uint8_t buttonPressedLast = 0; // Prevent button from being spammed
	while (1) {
		if (is_button_pressed()) {
			if (!buttonPressedLast) {
				// Play beep for all of the digits of the temperature
				for (int i = 0; i < sizeof(digitsToBeep)/sizeof(digitsToBeep[0]); i++) {
					beep_for_digit(digitsToBeep[i], frequenciesToPlay[i]);
				}
				buttonPressedLast = 1;
				_delay_ms(200); // Debounce delay
			}
		} else {
			buttonPressedLast = 0;
		}
	}
	
	return (1);
}