#ifndef button_h
#define button_h

// Button settings:
#define BUTTON_PIN PE0 // Button detection pin
#define BUTTON_PORT PORTE
#define BUTTON_DDR DDRE
#define BUTTON_PINE PINE

// Functions:
void init_button();
int is_button_pressed();

#endif