/*******

    A simple implementation of the Reset and Clock Control peripheral
    on STM32F103xB. System clock is configured to be 72MHz using
    a high-speed external crystal of 8MHz. RTC and backup domain is
    configured to use a low-speed external crystal of 32.768KHz.

*******/

#ifndef __RCC_H__
#define __RCC_H__

#include <stm32f1xx.h>

/* crystal values defined in Makefile for global access by all modules */
//#define HSE_VALUE 8000000
//#define LSE_VALUE 32768

#define RCC_AHB_DIV 1
#define RCC_APB1_DIV 2
#define RCC_APB2_DIV 1

/* See 'rcc.c' for complete setup procedure. */
void rcc_setup(void);

/* Make these functions externally accessible e.g for the RTC peripheral. */
void rcc_enable_pwr_bkp(void);
void rcc_disable_pwr_bkp(void);

#endif
