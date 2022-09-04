/*******

    Driver for buttons to be used to cycle through TOTP codes.

*******/

#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include <stm32f1xx.h>
#include <stdbool.h>


#define BUTTON_UP_GPIO_DEV GPIOA

#define BUTTON_UP_GPIO_CR (BUTTON_UP_GPIO_DEV->CRL)
#define BUTTON_UP_RCC_ENBIT RCC_APB2ENR_IOPAEN

#define BUTTON_UP_GPIO_CONF_Pos GPIO_CRL_MODE0_Pos
#define BUTTON_UP_GPIO_CONF_Msk (0b1111 << BUTTON_UP_GPIO_CONF_Pos)

#define BUTTON_UP_ODR (BUTTON_UP_GPIO_DEV->ODR)
#define BUTTON_UP_ODR_BIT (GPIO_ODR_ODR0)
#define BUTTON_UP_IDR (BUTTON_UP_GPIO_DEV->IDR)
#define BUTTON_UP_IDR_BIT (GPIO_IDR_IDR0)



#define BUTTON_DOWN_GPIO_DEV GPIOA

#define BUTTON_DOWN_GPIO_CR (BUTTON_DOWN_GPIO_DEV->CRL)
#define BUTTON_DOWN_RCC_ENBIT RCC_APB2ENR_IOPAEN

#define BUTTON_DOWN_GPIO_CONF_Pos GPIO_CRL_MODE1_Pos
#define BUTTON_DOWN_GPIO_CONF_Msk (0b1111 << BUTTON_DOWN_GPIO_CONF_Pos)

#define BUTTON_DOWN_ODR (BUTTON_DOWN_GPIO_DEV->ODR)
#define BUTTON_DOWN_ODR_BIT (GPIO_ODR_ODR1)
#define BUTTON_DOWN_IDR (BUTTON_DOWN_GPIO_DEV->IDR)
#define BUTTON_DOWN_IDR_BIT (GPIO_IDR_IDR1)


void buttons_setup(void);

/* Return 'true' if button is pressed, 'false' otherwise. */
bool buttons_poll_up(void);
bool buttons_poll_down(void);

#endif
