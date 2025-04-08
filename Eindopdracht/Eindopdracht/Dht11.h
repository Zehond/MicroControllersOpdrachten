#ifndef dht11_h
#define dht11_h

#include <stdint.h>

// DHT11 sensor settings:
#define DHT11_PIN PD0  // DHT11 DATA pin
#define DHT11_PORT PORTD
#define DHT11_DDR DDRD
#define DHT11_PIND PIND

typedef struct {
	int8_t temperature;
	uint8_t humidity;
} DHT11_Data;

extern DHT11_Data result;

// Functions:
DHT11_Data DHT11_get_data();

#endif