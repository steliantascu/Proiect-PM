#ifndef LCD_I2C_H
#define LCD_I2C_H

#include <avr/io.h>
#include <util/delay.h>

// PCF8574 adresa 8-bit write: 0x4E (adresa 7-bit 0x27, A0=A1=A2=0)
// Daca LCD-ul nu raspunde, incearca 0x7E (varianta cu A2=1)
#define LCD_ADDR 0x4E

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_write(uint8_t data);
void lcd_send_cmd(char cmd);
void lcd_send_data(char data);
void lcd_init(void);
void lcd_clear(void);
void lcd_print(char *str);
void lcd_set_cursor(uint8_t row, uint8_t col);

#endif
