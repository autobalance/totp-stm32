#include <i2c.h>
#include <rcc.h>

#define I2C_WRITE 0
#define I2C_READ  1

void i2c_setup(void)
{
    // enable I2C peripheral clocks and interface (B10=SCL2, B11=SDA2)
    RCC->APB1ENR |= I2C_RCC_ENBIT;
    RCC->APB2ENR |= I2C_GPIO_ENBIT;

    // configure GPIO as 50MHz max output in open-drain configuration for I2C
    I2C_GPIO_CR &= ~(I2C_GPIO_SCL_CONF_Msk | I2C_GPIO_SDA_CONF_Msk);
    I2C_GPIO_CR |= (0b1111 << I2C_GPIO_SCL_CONF_Pos) | (0b1111 << I2C_GPIO_SDA_CONF_Pos);

    // program frequency of APB1 in MHz as necessary
    I2C_DEV->CR2 = (I2C_PERIPH_CLOCK_HZ / 1000000U) << I2C_CR2_FREQ_Pos;

    // choose sm mode
    I2C_DEV->CCR = 0;

    // from ref. manual in sm mode with 50% duty cycle (T_high == T_low):
    // T_high = CCR * T_PCKL1 ---> CCR = T_high / T_PCLK1  = (T_cycle / 2) / T_PCKL1
    // compute using frequencies to avoid rounding during division
    I2C_DEV->CCR = I2C_PERIPH_CLOCK_HZ / (2 * I2C_SM_FREQ_HZ);

    // from ref. manual, and again compute using frequencies to avoid rounding
    I2C_DEV->TRISE = I2C_PERIPH_CLOCK_HZ / I2C_SM_RISE_HZ + 1;

    // enable I2C peripheral
    I2C_DEV->CR1 |= I2C_CR1_PE;
}

/* not for general use yet as it doesn't return when errors occur (e.g. ACK failure) */
i2c_err_t i2c_write_bytes(uint8_t addr, uint8_t *data, uint32_t data_len)
{
    if ((data_len == 0) || !data)
    {
        return I2C_ERR_NONE;
    }

    // send START condition
    I2C_DEV->CR1 |= I2C_CR1_START;

    // clear event EV5 (read SR1 to check SB bit and write I2C address to DR)
    while (!(I2C_DEV->SR1 & I2C_SR1_SB));
    I2C_DEV->DR = (addr << 1) | I2C_WRITE;  // LSB of address indiciates tx/rx on I2C

    // clear event EV6 (read SR1 to check ADDR bit then read SR2)
    while (!(I2C_DEV->SR1 & I2C_SR1_ADDR))
    {
        // return error when no ACK
        if (I2C_DEV->SR1 & I2C_SR1_AF)
        {
            I2C_DEV->CR1 &= ~I2C_CR1_SWRST; // TODO: release the bus before reset
            return I2C_ERR_AF;
        }
    }
    I2C_DEV->SR2;

    // process and clear events EV8, EV8_1
    for (uint32_t i = 0; i < data_len; i++)
    {
        // on events EV8 and EV8_1, check TxE before putting data on DR
        if (!(I2C_DEV->SR1 & I2C_SR1_TXE))
        {
            continue;
        }

        // if system was too slow, BTF may have been set before putting next data
        // so clear BTF as necessary before putting data on DR
        if (I2C_DEV->SR1 & I2C_SR1_BTF) I2C_DEV->SR1;

        I2C_DEV->DR = data[i];
    }

    // clear event EV8_2 (wait for BTF, then program STOP)
    while (!(I2C_DEV->SR1 & I2C_SR1_BTF));
    I2C_DEV->CR1 |= I2C_CR1_STOP;

    return I2C_ERR_NONE;
}

i2c_err_t i2c_read_one_byte(uint8_t addr, uint8_t *data)
{
    // send ACK when bytes received
    I2C_DEV->CR1 |= I2C_CR1_ACK;

    // send START condition
    I2C_DEV->CR1 |= I2C_CR1_START;

    // clear event EV5 (read SR1 to check SB bit and write I2C address to DR)
    while (!(I2C_DEV->SR1 & I2C_SR1_SB));
    I2C_DEV->DR = (addr << 1) | I2C_READ;  // LSB of address indiciates tx/rx on I2C

    // clear event EV6_3 (read SR1 until ADDR=1, clear ACK, read SR2 for ADDR=0, then STOP)
    while (!(I2C_DEV->SR1 & I2C_SR1_ADDR))
    {
        // return error when no ACK
        if (I2C_DEV->SR1 & I2C_SR1_AF)
        {
            I2C_DEV->CR1 &= ~I2C_CR1_SWRST; // TODO: release the bus before reset
            return I2C_ERR_AF;
        }
    }
    I2C_DEV->CR1 &= ~I2C_CR1_ACK;
    I2C_DEV->SR2;
    I2C_DEV->CR1 |= I2C_CR1_STOP;

    // clear event EV7 (wait for RXNE, then read DR)
    while (!(I2C_DEV->SR1 & I2C_SR1_RXNE));
    *data = I2C_DEV->DR;

    return I2C_ERR_NONE;
}

i2c_err_t i2c_read_two_bytes(uint8_t addr, uint8_t *data)
{
    // send ACK when bytes received
    I2C_DEV->CR1 |= I2C_CR1_POS | I2C_CR1_ACK;

    // send START condition
    I2C_DEV->CR1 |= I2C_CR1_START;

    // clear event EV5 (read SR1 to check SB bit and write I2C address to DR)
    while (!(I2C_DEV->SR1 & I2C_SR1_SB));
    I2C_DEV->DR = (addr << 1) | I2C_READ;  // LSB of address indiciates tx/rx on I2C

    // clear event EV6 (read SR1 until ADDR=1, read SR2 for ADDR=0)
    while (!(I2C_DEV->SR1 & I2C_SR1_ADDR))
    {
        // return error when no ACK
        if (I2C_DEV->SR1 & I2C_SR1_AF)
        {
            I2C_DEV->CR1 &= ~I2C_CR1_SWRST; // TODO: release the bus before reset
            return I2C_ERR_AF;
        }
    }
    I2C_DEV->SR2;

    // clear event EV6_1 (disable ACK right after clear ADDR)
    I2C_DEV->CR1 &= ~I2C_CR1_ACK;

    // clear event EV7_3 (wait for BTF, program STOP, then read DR twice)
    while (!(I2C_DEV->SR1 & I2C_SR1_BTF));
    I2C_DEV->CR1 |= I2C_CR1_STOP;

    data[0] = I2C_DEV->DR;
    data[1] = I2C_DEV->DR;

    return I2C_ERR_NONE;
}

i2c_err_t i2c_read_bytes(uint8_t addr, uint8_t *data, uint32_t data_len)
{
    if ((data_len == 0) || !data)
    {
        return I2C_ERR_NONE;
    }
    else if (data_len == 1)
    {
        return i2c_read_one_byte(addr, data);
    }
    else if (data_len == 2)
    {
        return i2c_read_two_bytes(addr, data);
    }

    // send ACK when bytes received
    I2C_DEV->CR1 |= I2C_CR1_ACK;

    // send START condition
    I2C_DEV->CR1 |= I2C_CR1_START;

    // clear event EV5 (read SR1 to check SB bit and write I2C address to DR)
    while (!(I2C_DEV->SR1 & I2C_SR1_SB));
    I2C_DEV->DR = (addr << 1) | I2C_READ;  // LSB of address indiciates tx/rx on I2C

    // clear event EV6 (read SR1 to check ADDR bit then read SR2)
    while (!(I2C_DEV->SR1 & I2C_SR1_ADDR))
    {
        // return error when no ACK
        if (I2C_DEV->SR1 & I2C_SR1_AF)
        {
            I2C_DEV->CR1 &= ~I2C_CR1_SWRST; // TODO: release the bus before reset
            return I2C_ERR_AF;
        }
    }
    I2C_DEV->SR2;

    // process and clear event EV7 until 3 bytes remain to be received
    for (uint32_t i = 0; i < data_len - 3; i++)
    {
        // check RxNE for data available in DR
        if (!(I2C_DEV->SR1 & I2C_SR1_RXNE))
        {
            continue;
        }

        data[i] = I2C_DEV->DR;
    }

    // clear event EV7_2 (wait for BTF, set ACK=0, read 3rd last byte, set STOP, read 2nd last byte)
    while (!(I2C_DEV->SR1 & I2C_SR1_BTF));
    I2C_DEV->CR1 &= ~I2C_CR1_ACK;
    data[data_len - 3] = I2C_DEV->DR;
    I2C_DEV->CR1 |= I2C_CR1_STOP;
    data[data_len - 2] = I2C_DEV->DR;

    // clear event EV7 (wait for RxNE, then read DR)
    while (!(I2C_DEV->SR1 & I2C_SR1_RXNE));
    data[data_len - 1] = I2C_DEV->DR;

    return I2C_ERR_NONE;
}
