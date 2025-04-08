#ifndef buzzer_h
#define buzzer_h

#include <stdint.h>
#include <stdbool.h>

#include "Main.h"

extern bool isNegative;

// Functions:
void init_buzzer();
void beep_for_digit(int digit, int frequency);

#endif