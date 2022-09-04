#include <stm32f1xx.h>
#include <rcc.h>
#include <i2c.h>
#include <rtc.h>
#include <systick.h>

#include <buttons.h>
#include <led.h>
#include <lcd_1602.h>

#include <common.h>
#include <base32.h>
#include <totp.h>
#include <ui.h>
#include <storage.h>

void print_totp(uint32_t idx, uint32_t time)
{
    storage_data_t *totp_params;
    storage_get(&totp_params, idx);

    char code[TOTP_CODE_LEN + 1];
    totp_get_code(code, totp_params->key, totp_params->key_len, time);

    ui_print_name(totp_params->name);
    ui_print_code(code);
}

int main(void)
{
    rcc_setup();
    i2c_setup();
    rtc_setup();
    systick_setup();

    buttons_setup();

    led_setup();
    led_disable();

    lcd_setup();
    lcd_clear();

    // reset time if backup domain has just been reset, but not otherwise
    rtc_set_time(COMPILE_DATE, false);

    uint32_t idx = 0, idx_mod = storage_num_stored();

    uint32_t last_time = 0;
    for (;;)
    {
        uint32_t curr_time = rtc_get_time();

        // process button presses to change code being viewed
        bool up_pressed = buttons_poll_up(),
             down_pressed = buttons_poll_down();
        if (up_pressed || down_pressed)
        {
            if (up_pressed)
                idx = (idx + 1) % idx_mod;
            if (down_pressed)
                idx = (idx_mod + idx - 1) % idx_mod;

            print_totp(idx, curr_time);
        }

        // print the seconds countdown
        if (curr_time > last_time)
        {
            // renew the TOTP code and print it if 30s elapsed
            if ((curr_time / TOTP_TIME_STEP) > (last_time / TOTP_TIME_STEP))
            {
                print_totp(idx, curr_time);
            }

            ui_print_time_left(curr_time);

            last_time = curr_time;
        }
    }
}
