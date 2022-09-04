#include <led.h>

void led_setup(void)
{
    // enable clock for the LED's GPIO port
    RCC->APB2ENR |= LED_RCC_ENBIT;

    // configure GPIO as 50MHz max output in push-pull configuration for LED
    LED_GPIO_CR &= ~LED_GPIO_CONF_Msk;
    LED_GPIO_CR |= 0b0011 << LED_GPIO_CONF_Pos;
}

inline void led_toggle(void)
{
    LED_GPIO_ODR ^= LED_GPIO_ODR_BIT;
}

inline void led_enable(void)
{
    // open-drain, so the GPIO low turns the LED on
    LED_GPIO_ODR &= ~LED_GPIO_ODR_BIT;
}

inline void led_disable(void)
{
    LED_GPIO_ODR |= LED_GPIO_ODR_BIT;
}
