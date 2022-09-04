/*******

    UI related functions (to print things in specific areas on the LCD).

*******/

#ifndef __UI_H__
#define __UI_H__

#include <common.h>

/* Print a TOTP name on second row, beginning at first column. */
void ui_print_name(char *name);

/* Print a TOTP code on second row, beginning at first column. */
void ui_print_code(char *code);

/* Print time remaining in right-most of second row (should be UNIX time). */
void ui_print_time_left(uint32_t time);

#endif
