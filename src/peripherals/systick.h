/*******

    Driver for the SysTick peripheral on Cortex-M. Provides delay functions.

*******/

#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include <stm32f1xx.h>

void systick_setup(void);

void systick_delay_ms(uint32_t ms);

uint32_t systick_get_tick(void);

#endif
