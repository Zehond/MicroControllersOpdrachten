#define F_CPU 10e6 // 10mHz clock speed

#include <avr/io.h>
#include <util/delay.h>

#include "Dht11.h"

/* The DHT11 sensor can get temperatures within the range of 0-50 캜. These temperatures come in whole (integer) values.
    This means that we cannot get negative values or decimal values with this sensor. */

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
	if (!(DHT11_PIND & (1 << DHT11_PIN))) { // Waits for pin LOW. Which confirms the sensor has responded.
		_delay_us(80); // This aligns with the DHT11 protocol, where the sensor keeps the line LOW for 80 탎 before pulling it HIGH.
		if (DHT11_PIND & (1 << DHT11_PIN)) { // Waits for pin HIGH. If the pin is HIGH, it confirms that the sensor has acknowledged the request and is ready to send data.
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
		while (!(DHT11_PIND & (1 << DHT11_PIN)));  // Waits for pin HIGH. Ensures synchronization with the sensor's data transmission.
		_delay_us(30); // Wait before checking the pin again.
		if (DHT11_PIND & (1 << DHT11_PIN))  // If pin is still HIGH, bit is 1. Otherwise it's 0. The most significant bit (MSB) is received first, so the bit is placed in position (7 - i).
		result |= (1 << (7 - i)); // Add byte to the result array.
		while (DHT11_PIND & (1 << DHT11_PIN)); // Waits for pin LOW. This marks the end of the bit transmission before moving to the next bit.
	}
	return result; // Returns array of bits.
}

/* Method that gets the data from the DHT11 sensor 
   This code was written with help from AI! */
DHT11_Data DHT11_get_data() {
	uint8_t humInt, humDec, tempInt, tempDec, checksum;
	DHT11_Data newData = { 0, 0 };
	
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
			newData.temperature = tempInt; // temperature (캜) is tempInt, because tempDec is always 0 using a DHT11 sensor.
			newData.humidity = humInt; // Hum is humInt, because humDec is always 0 using a DHT11 sensor.
		}
	}
	return newData;
}