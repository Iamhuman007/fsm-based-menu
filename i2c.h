#ifndef I2C_H  // Include guard to prevent multiple inclusions
#define I2C_H  

#include <msp430.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// OLED I2C Address
#define SSD1306_I2C_ADDR  0x3C
#define opt_no 3

// Function Prototypes
void I2C_Init();
void I2C_WriteByte(unsigned char data);
void OLED_Command(unsigned char command);
void OLED_Init();
void OLED_display_setup(unsigned char col_st, unsigned char col_end, unsigned char pg_st, unsigned char pg_end);
void OLED_display_clear(unsigned char col_st, unsigned char col_end, unsigned char pg_st, unsigned char pg_end);
void OLED_display_data(char c, size_t len);
void OLED_display_char(char c);
void OLED_display_string(char *s,size_t Inv);
void drawPixel_OLED(int x, int y, bool on);
void draw_string(const char *s, int x, int y);
void display_screen( int current);
void start_I2C();
void stop_I2C();
void delay_ms(unsigned int ms);
void unusedPinsConfiguration(void);// setting all pins as inputs

#endif  // I2C_H
