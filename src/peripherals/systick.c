#include <systick.h>

volatile uint32_t ticks_ms = 0;

void SysTick_Handler(void)
{
    ticks_ms++;
}

void systick_setup(void)
{
    // trigger SysTick_Handler every millisecond
    NVIC_EnableIRQ(SysTick_IRQn);
    SysTick_Config(SystemCoreClock / 1000);
}

void systick_delay_ms(uint32_t ms)
{
    if (ms == 0) return;

    volatile uint32_t start = ticks_ms;

    while ((ticks_ms - start) < ms) __WFI();
}

uint32_t systick_get_tick(void)
{
    return ticks_ms;
}
