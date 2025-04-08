#ifndef lcd_h
#define lcd_h

// LCD-screen settings:
#define LCD_E 	6  // RA6 UNI-6
#define LCD_RS	4  // RA4 UNI-6

// Functions:
void init_lcd();
void display_text(char *str);
void lcd_clear();
void set_cursor(int row, int position);

#endif