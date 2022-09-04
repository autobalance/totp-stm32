/*******

    A simple polling-based I2C driver for the STM32F103xB.
    TODO: Implement handling for all failures.

*******/

#ifndef __I2C_H__
#define __I2C_H__

#include <stm32f1xx.h>

// using I2C2 for this application
#define I2C_DEV I2C2
#define I2C_RCC_ENBIT RCC_APB1ENR_I2C2EN

#define I2C_GPIO_DEV GPIOB

#define I2C_GPIO_CR (I2C_GPIO_DEV->CRH)
#define I2C_GPIO_ENBIT RCC_APB2ENR_IOPBEN

#define I2C_GPIO_SCL_CONF_Pos GPIO_CRH_MODE10_Pos
#define I2C_GPIO_SCL_CONF_Msk (0b1111 << I2C_GPIO_SCL_CONF_Pos)
#define I2C_GPIO_SDA_CONF_Pos GPIO_CRH_MODE11_Pos
#define I2C_GPIO_SDA_CONF_Msk (0b1111 << I2C_GPIO_SDA_CONF_Pos)

#define I2C_PERIPH_CLOCK_HZ (SystemCoreClock / RCC_APB1_DIV)
#define I2C_SM_FREQ_HZ      100000U             // sm frequency = 100 kHz 
#define I2C_SM_RISE_HZ      1000000U            // sm 100khz min rise freq = 1/max rise time = 1/1000ns

typedef enum i2c_err_enum { I2C_ERR_NONE, I2C_ERR_AF } i2c_err_t;

/* Sets up the I2C peripheral corresponding the the above defines */
/* TODO: Make it flexible to setup either I2C-1 or I2C-2 */
void i2c_setup(void);

/* Write data to an I2C device with given 'addr' */
i2c_err_t i2c_write_bytes(uint8_t addr, uint8_t *data, uint32_t data_len);

/* Read data from an I2C device */
i2c_err_t i2c_read_bytes(uint8_t addr, uint8_t *data, uint32_t data_len);

#endif
