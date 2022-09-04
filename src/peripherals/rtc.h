/*******

    A simple implementation of the Real Time Clock peripheral
    on STM32F103xB.

    TODO: Implement 64-bit counting using a second variable to count overflows.

*******/

#ifndef __RTC_H__
#define __RTC_H__

#include <stm32f1xx.h>
#include <stdbool.h>

/* Use a backup register (BKP in ref. manual) to flag RTC initialized. */
/* Used to prevent resetting the clock counter on system resets. */
#define RTC_INIT_REG (BKP->DR1)
#define RTC_INIT_MAGIC 0x47C2

/* Use a backup register to flag RTC time has been set already. */
#define RTC_TIMESET_REG (BKP->DR2)
#define RTC_TIMESET_MAGIC 0xbEEf

/* See 'rtc.c' for complete setup procedure. */
void rtc_setup(void);

/* Get the time from the RTC->CNT{L,H} registers. */
uint32_t rtc_get_time(void);

/* Set the time in the RTC->CNT{L,H} registers. */
/* If 'force' is false, don't set if time has been set since last backup domain reset. */
void rtc_set_time(uint32_t time, bool force);

#endif
