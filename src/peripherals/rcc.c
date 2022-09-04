#include <rcc.h>

void rcc_enable_pwr_bkp(void)
{
    // enable power and backup interface clocks and enable writes to BKP
    RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;
}

void rcc_setup_prescalers(void)
{
    // assuming 72MHz system clock (see defines in 'rcc.h' to match)
    // set AHB prescaler to 1, APB2 prescaler to 1, APB1 prescaler to 2
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
}

void rcc_setup_lse(void)
{
    // enable write access to backup domain registers
    PWR->CR |= PWR_CR_DBP;

    // enable LSE crystal in BKP and wait indefinitely for it to become ready
    RCC->BDCR |= RCC_BDCR_LSEON;
    while (!(RCC->BDCR & RCC_BDCR_LSERDY));

    // disable write access to backup domain (avoid parasitic writes)
    PWR->CR &= ~PWR_CR_DBP;
}

void rcc_setup_hse(void)
{
    // enable HSE crystal and wait indefinitely for it to become ready
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));
}

void rcc_setup_pll(void)
{
    // set PLL source clock to HSE with multiplier of 9 (8MHz HSE gives 72MHz out of PLL)
    // then turn PLL on and wait indefinitely to become ready
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL);
    RCC->CFGR |= RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9;
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));
}

void rcc_setup_sysclk(void)
{
    // set system clock source to PLL and wait to become ready
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;    
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

void rcc_setup(void)
{
    // enable flash prefetch and set 2 cycle wait-state (for 72MHz system clock)
    // TODO: put this in flash.c peripheral source?
    FLASH->ACR |= FLASH_ACR_PRFTBE;
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_2;

    rcc_enable_pwr_bkp();

    rcc_setup_prescalers();

    rcc_setup_lse();
    rcc_setup_hse();
    rcc_setup_pll();

    rcc_setup_sysclk();

    // update system clock variables to reflect changes
    SystemCoreClockUpdate();
}
