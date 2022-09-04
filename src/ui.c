#include <ui.h>
#include <lcd_1602.h>

void ui_print_name(char *name)
{
    lcd_set_cursor(0, 0);
    lcd_print(name);

    uint32_t name_len = -1;
    while (name[++name_len] != '\0');

    char blank[LCD_NCOLS] = "                ";
    blank[LCD_NCOLS - name_len] = '\0';
    lcd_print(blank);
}

void ui_print_code(char *code)
{
    lcd_set_cursor(1, 0);
    lcd_print(code);
}

void ui_print_time_left(uint32_t time)
{
    uint32_t time_left = TOTP_TIME_STEP - (time % TOTP_TIME_STEP);

    // TODO: implement a generic integer-to-ascii conversion
    char time_str[4] = "  s";
    time_str[1] = ((time_left) % 10) + '0';
    time_str[0] = (((time_left) / 10) % 10) + '0';

    lcd_set_cursor(1, 13);
    lcd_print(time_str);
}

