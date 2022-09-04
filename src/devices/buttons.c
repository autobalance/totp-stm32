#include <buttons.h>
#include <systick.h> // use tick counter for debounce logic

// use 100ms for debounce time (also accounts for reaction time of releasing button)
#define DEBOUNCE_TIME 150

void buttons_setup(void)
{
    // enable clock for each buttons GPIO port
    RCC->APB2ENR |= BUTTON_UP_RCC_ENBIT | BUTTON_DOWN_RCC_ENBIT;

    // configure each button as input with pull-down
    BUTTON_UP_GPIO_CR &= ~BUTTON_UP_GPIO_CONF_Msk;
    BUTTON_UP_GPIO_CR |= 0b1000 << BUTTON_UP_GPIO_CONF_Pos;
    BUTTON_UP_ODR &= ~BUTTON_UP_ODR_BIT; // use pull-down

    BUTTON_DOWN_GPIO_CR &= ~BUTTON_DOWN_GPIO_CONF_Msk;
    BUTTON_DOWN_GPIO_CR |= 0b1000 << BUTTON_DOWN_GPIO_CONF_Pos;
    BUTTON_DOWN_ODR &= ~BUTTON_DOWN_ODR_BIT;
}

bool buttons_poll_up(void)
{
    static uint32_t last_tick = 0;

    uint32_t curr_tick = systick_get_tick();
    if (curr_tick - last_tick >= DEBOUNCE_TIME)
    {
        last_tick = curr_tick;

        if (BUTTON_UP_IDR & BUTTON_UP_IDR_BIT)
        {
            return true;
        }
    }

    return false;
}

bool buttons_poll_down(void)
{
    static uint32_t last_tick = 0;

    uint32_t curr_tick = systick_get_tick();
    if (curr_tick - last_tick >= DEBOUNCE_TIME)
    {
        last_tick = curr_tick;

        if (BUTTON_DOWN_IDR & BUTTON_DOWN_IDR_BIT)
        {
            return true;
        }
    }

    return false;
}
