/*******

    LED driver for the 'BluePill' STM32F103xB based boards with
    PC13 as the GPIO for the LED (in open-drain configuration).

*******/

#ifndef __LED_H__
#define __LED_H__

#include <stm32f1xx.h>

#define LED_GPIO_DEV GPIOC

#define LED_GPIO_CR (LED_GPIO_DEV->CRH)
#define LED_RCC_ENBIT RCC_APB2ENR_IOPCEN

#define LED_GPIO_CONF_Pos GPIO_CRH_MODE13_Pos
#define LED_GPIO_CONF_Msk (0b1111 << LED_GPIO_CONF_Pos)

#define LED_GPIO_ODR (LED_GPIO_DEV->ODR)
#define LED_GPIO_ODR_BIT (GPIO_ODR_ODR13)

/* Use the above defines to setup and drive the GPIO for the LED. */
void led_setup(void);

void led_toggle(void);
void led_enable(void);
void led_disable(void);

#endif
