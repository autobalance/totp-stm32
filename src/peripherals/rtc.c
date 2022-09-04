#include <rcc.h>
#include <rtc.h>

void rtc_setup(void)
{
    // don't setup RTC if already initialized
    // needed to preserve RTC during system resets
    if (RTC_INIT_REG == RTC_INIT_MAGIC)
    {
        return;
    }

    // enable write-access to backup domain registers
    PWR->CR |= PWR_CR_DBP;

    // use LSE crystal as RTC clock
    RCC->BDCR |= RCC_BDCR_RTCSEL_LSE;
    RCC->BDCR |= RCC_BDCR_RTCEN;

    // wait for RTC registers to synchronize after reset
    RTC->CRL &= ~RTC_CRL_RSF;
    while (!(RTC->CRL & RTC_CRL_RSF));

    // wait for RTC registers write-access and go into conf. mode
    while (!(RTC->CRL & RTC_CRL_RTOFF));
    RTC->CRL = RTC_CRL_CNF;

    // configure prescaler to have RTC count seconds
    uint32_t lse_psc_val = LSE_VALUE - 1;
    RTC->PRLH = (lse_psc_val << 16) & 0xffff0000;
    RTC->PRLL = (lse_psc_val <<  0) & 0x0000ffff;

    // exit conf. mode and wait for registers to be updated
    RTC->CRL &= ~RTC_CRL_CNF;
    while (!(RTC->CRL & RTC_CRL_RTOFF));

    // enable seconds-based interrupts
    RTC->CRH |= RTC_CRH_SECIE;

    // don't enable IRQ line here, let user application do it when ready
    //NVIC_EnableIRQ(RTC_IRQn);

    // set a backup register to flag RTC in BKP domain is setup
    RTC_INIT_REG = RTC_INIT_MAGIC;

    // disable write-access to backup domain registers
    PWR->CR &= ~PWR_CR_DBP;
}

void rtc_set_time(uint32_t time, bool force)
{
    // return if time was set prior and it's not being forced to change
    if ((RTC_TIMESET_REG == RTC_TIMESET_MAGIC) && !force)
    {
        return;
    }

    // enable write-access to backup domain registers
    PWR->CR |= PWR_CR_DBP;

    // wait for RTC registers write-access and go into conf. mode
    while (!(RTC->CRL & RTC_CRL_RTOFF));
    RTC->CRL = RTC_CRL_CNF;

    RTC->CNTH = (time >> 16) & 0xffff;
    RTC->CNTL = (time >>  0) & 0xffff;

    // exit conf. mode and wait for registers to be updated
    RTC->CRL &= ~RTC_CRL_CNF;
    while (!(RTC->CRL & RTC_CRL_RTOFF));

    // set a backup register to flag RTC counter is set
    RTC_TIMESET_REG = RTC_TIMESET_MAGIC;

    // disable write-access to backup domain registers
    PWR->CR &= ~PWR_CR_DBP;
}

uint32_t rtc_get_time(void)
{
    return (((uint32_t) RTC->CNTH) << 16) | (((uint32_t) RTC->CNTL) & 0xffff);
}
