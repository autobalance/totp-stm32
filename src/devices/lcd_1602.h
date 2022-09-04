/*******

    A driver for an LCD1602 display (with Hitachi HD44780U controller).
    Using I2C via the PCF8574 GPIO expander for displays assembled
    with this attachment.

*******/

#ifndef __LCD1602_H__
#define __LCD1602_H__

#include <i2c.h>

// Address of the PCF8574 extender
#define I2C_ADDR 0x27

#define LCD_NROWS 2
#define LCD_NCOLS 16

void lcd_setup(void);

/* Clears the entire screen (use set cursor and print to partial-clear). */
void lcd_clear(void);

/* Sets cursor position and does nothing if out of bounds. */
void lcd_set_cursor(uint32_t row, uint32_t col);

/* Simple print function. Requires 'str' to be null-terminated. */
/* Doesn't quit if overflows, to allow for scrolling. */
void lcd_print(char *str);

#endif
