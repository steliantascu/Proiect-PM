#include "lcd_i2c.h"

// =====================================================
// LAB 6: Driver TWI (I2C) manual, registri direct
// A4 = PC4 = SDA, A5 = PC5 = SCL (hardware pe 328P)
// TWBR = (16000000 / 100000 - 16) / 2 = 72 => 100kHz
// =====================================================

void i2c_init(void) {
    TWSR = 0x00;
    TWBR = 72;
    TWCR = (1 << TWEN);
}

void i2c_start(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

void i2c_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

// Trimite un byte catre LCD prin PCF8574 in mod 4-bit
// mode=0 comanda, mode=1 date (RS bit)
static void lcd_send_byte(char data, uint8_t mode) {
    uint8_t hi = (data & 0xF0);
    uint8_t lo = ((data << 4) & 0xF0);

    uint8_t seq[4];
    seq[0] = hi | mode | 0x0C; // EN=1, BL=1
    seq[1] = hi | mode | 0x08; // EN=0, BL=1
    seq[2] = lo | mode | 0x0C; // EN=1, BL=1
    seq[3] = lo | mode | 0x08; // EN=0, BL=1

    i2c_start();
    i2c_write(LCD_ADDR);
    for (int i = 0; i < 4; i++) i2c_write(seq[i]);
    i2c_stop();
}

void lcd_send_cmd(char cmd)   { lcd_send_byte(cmd, 0); }
void lcd_send_data(char data) { lcd_send_byte(data, 1); }

void lcd_init(void) {
    i2c_init();
    _delay_ms(50);
    lcd_send_cmd(0x33);
    lcd_send_cmd(0x32);
    lcd_send_cmd(0x28); // 4-bit, 2 linii, font 5x8
    lcd_send_cmd(0x0C); // Display ON, cursor OFF
    lcd_send_cmd(0x06); // Auto-increment
    lcd_clear();
}

void lcd_clear(void) {
    lcd_send_cmd(0x01);
    _delay_ms(2);
}

void lcd_print(char *str) {
    while (*str) lcd_send_data(*str++);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t addr = (row == 0) ? (0x80 + col) : (0xC0 + col);
    lcd_send_cmd(addr);
}
