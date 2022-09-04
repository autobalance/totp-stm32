#include <lcd_1602.h>
#include <systick.h>

/*
    PCF8574-to-HD44780U pin map (for my display at least):

        P0 - RS     # 0 for instruction register, 1 for data register
        P1 - RW     # 0 for write, 1 for read
        P2 - E      # 1 to start a data read/write
        P3 - BKL    # 0 to disable backlight, 1 to enable backlight
        P4 - D4     # Rest are bidirectional data pins used in 4-bit mode
        P5 - D5
        P6 - D6
        P7 - D7

        Note that PCF8574 receives (I2C sends) the most-significant bit first.
*/

#define LCD_RS_PIN   0
#define LCD_RW_PIN   1
#define LCD_E_PIN    2
#define LCD_BKL_PIN  3
#define LCD_DB4_PIN  4
#define LCD_DB5_PIN  5
#define LCD_DB6_PIN  6
#define LCD_DB7_PIN  7

#define LCD_MODE_NOSHIFT 0
#define LCD_MODE_SHIFT   (1 << 0)
#define LCD_MODE_DECR    0
#define LCD_MODE_INCR    (1 << 1)

#define LCD_CTRL_NOBLINK 0
#define LCD_CTRL_BLINK   (1 << 0)
#define LCD_CTRL_NOCURS  0
#define LCD_CTRL_CURS    (1 << 1)
#define LCD_CTRL_OFF     0
#define LCD_CTRL_ON      (1 << 2)

#define LCD_SHIFT_LEFT   0
#define LCD_SHIFT_RIGHT  (1 << 2)
#define LCD_SHIFT_CURS   0
#define LCD_SHIFT_DISP   (1 << 3)

#define LCD_FUNC_5X8     0
#define LCD_FUNC_5X10    (1 << 2)
#define LCD_FUNC_1LINE   0
#define LCD_FUNC_2LINE   (1 << 3)
#define LCD_FUNC_4BIT    0
#define LCD_FUNC_8BIT    (1 << 4)

#define LCD_BFAC_BF_MASK (0x80)
#define LCD_BFAC_AC_MASK (0x7f)


// Offsets corresponding to rows in DDRAM (Figure 4. in HD44780U datasheet).
const uint32_t LCD_ROW_DDRAM_OFS[LCD_NROWS] = {0x00, 0x40};



enum rs_type {
    RS_INSTR = 0,
    RS_DATA  = (1 << LCD_RS_PIN)
};

enum rw_type {
    RW_WRITE = 0,
    RW_READ  = (1 << LCD_RW_PIN)
};

enum e_type {
    E_OFF = 0,
    E_ON  = (1 << LCD_E_PIN)
};

enum bkl_type {
    BKL_OFF = 0,
    BKL_ON  = (1 << LCD_BKL_PIN)
};

enum instr_type {
    INSTR_CLEAR  = 0x01,
    INSTR_HOME   = 0x02,
    INSTR_MODE   = 0x04,
    INSTR_CTRL   = 0x08,
    INSTR_SHIFT  = 0x10,
    INSTR_FUNC   = 0x20,
    INSTR_CGADDR = 0x40,
    INSTR_DDADDR = 0x80
};


// read the busy flag and address counter (needed before sending instr./data)
uint8_t lcd_read_bf_ac(void)
{
    uint8_t data_high = 0, data_low = 0;

    // set RW pin to tell LCD to start a read (and always keep backlight on)
    uint8_t pinout = RW_READ | BKL_ON;
    i2c_write_bytes(I2C_ADDR, &pinout, 1);

    // turn on E pin to start reading high-order nibble of data
    pinout |= E_ON;
    i2c_write_bytes(I2C_ADDR, &pinout, 1);

    // read high order nibble of data from LCD
    i2c_read_bytes(I2C_ADDR, &data_high, 1);

    // turn off E pin to do second read
    pinout &= ~E_ON;
    i2c_write_bytes(I2C_ADDR, &pinout, 1);

    // turn on E pin to start reading low-order nibble of data
    pinout |= E_ON;
    i2c_write_bytes(I2C_ADDR, &pinout, 1);

    // read low order nibble of data from LCD
    i2c_read_bytes(I2C_ADDR, &data_low, 1);

    // turn off E and RW pins to resume other operations
    pinout &= ~(E_ON | RW_READ);
    i2c_write_bytes(I2C_ADDR, &pinout, 1);

    return (data_high & 0xf0) | ((data_low & 0xf0) >> 4);
}

void lcd_write_nib(uint8_t nib, enum rs_type rs)
{
    // add BKL (backlight always on) and add RS
    uint8_t pinout = nib | BKL_ON | rs;

    // assert data on the LCD data bus and turn on the E pin to start a write
    pinout |= E_ON;
    i2c_write_bytes(I2C_ADDR, &pinout, 1);

    // turn off the E pin to resume other operations
    pinout &= ~E_ON;
    i2c_write_bytes(I2C_ADDR, &pinout, 1);
}

void lcd_write_byte(uint8_t dat, enum rs_type rs)
{
    // as per HD44780U datasheet, send most-significant nibble first
    // data pins are always upper most nibble, followed by BKL,E,RW,RS
    lcd_write_nib(((dat & 0xf0) << 0), rs);
    lcd_write_nib(((dat & 0x0f) << 4), rs);
}

void lcd_send_instr(enum instr_type instr, uint8_t args)
{
    while (lcd_read_bf_ac() & LCD_BFAC_BF_MASK);

    lcd_write_byte(instr | args, RS_INSTR);
}

void lcd_send_data(uint8_t data)
{
    while (lcd_read_bf_ac() & LCD_BFAC_BF_MASK);

    lcd_write_byte(data, RS_DATA);
}

// Followed 'Initializing by Instruction' section in datasheet
void lcd_setup(void)
{
    // delay for LCD initial power-up
    systick_delay_ms(50);

    // 3 'function set' instructions need to be sent in 8-bit mode
    // can't use byte-level writes yet, so send one nibble at a time
    for (int i = 0; i < 3; i++)
    {
        lcd_write_nib(INSTR_FUNC | LCD_FUNC_8BIT, RS_INSTR);

        // over-delaying, but should be delay >4.1ms, then >100us, then none
        systick_delay_ms(25);
    }
    // send 'function set' to now change to 4-bit mode
    lcd_write_nib(INSTR_FUNC | LCD_FUNC_4BIT, RS_INSTR);

    // initialize the LCD parameters
    lcd_send_instr(INSTR_FUNC, LCD_FUNC_4BIT | LCD_FUNC_2LINE | LCD_FUNC_5X8);
    lcd_send_instr(INSTR_CTRL, LCD_CTRL_ON | LCD_CTRL_NOCURS | LCD_CTRL_NOBLINK);
    lcd_send_instr(INSTR_MODE, LCD_MODE_INCR | LCD_MODE_NOSHIFT);
    lcd_send_instr(INSTR_HOME, 0);
}

void lcd_clear(void)
{
    lcd_send_instr(INSTR_CLEAR, 0);
}

/* TODO: handle case when display shift is performed */
void lcd_set_cursor(uint32_t row, uint32_t col)
{
    if (row >= LCD_NROWS || col >= LCD_NCOLS)
    {
        return;
    }

    lcd_send_instr(INSTR_DDADDR, (LCD_ROW_DDRAM_OFS[row] | col));
}

void lcd_print(char *s)
{
    while (*s)
    {
        lcd_send_data(*s++);
    }
}
