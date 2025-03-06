#ifndef __DRV_I2C_H__
#define __DRV_I2C_H__

#include "i2c.h"

#define I2C_MEM_WRITE_TIMEOUT (10)
#define I2C_MEM_READ_TIMEOUT (10)
#define I2C_DEV_CHECK_TIMEOUT (100)

typedef struct i2c_dev *i2c_dev_t;

struct i2c_dev
{
    I2C_HandleTypeDef *i2c;
    uint16_t dev_addr;
};

#define i2c_check(i2c_dev) \
    HAL_I2C_IsDeviceReady((i2c_dev)->i2c, (i2c_dev)->dev_addr, 1, I2C_DEV_CHECK_TIMEOUT)

#define i2c_write_mem_16(i2c_dev, mem_addr, value) \
    HAL_I2C_Mem_Write((i2c_dev)->i2c, (i2c_dev)->dev_addr, (mem_addr), I2C_MEMADD_SIZE_8BIT, (uint8_t *)&(value), 2, I2C_MEM_WRITE_TIMEOUT)

#define i2c_read_mem_16(i2c_dev, mem_addr, value) \
    HAL_I2C_Mem_Read((i2c_dev)->i2c, (i2c_dev)->dev_addr, (mem_addr), I2C_MEMADD_SIZE_8BIT, (uint8_t *)&(value), 2, I2C_MEM_READ_TIMEOUT)

#define i2c_write_mem_8(i2c_dev, mem_addr, value) \
    HAL_I2C_Mem_Write((i2c_dev)->i2c, (i2c_dev)->dev_addr, (mem_addr), I2C_MEMADD_SIZE_8BIT, (uint8_t *)&(value), 1, I2C_MEM_WRITE_TIMEOUT)

#define i2c_read_mem_8(i2c_dev, mem_addr, value) \
    HAL_I2C_Mem_Read((i2c_dev)->i2c, (i2c_dev)->dev_addr, (mem_addr), I2C_MEMADD_SIZE_8BIT, (uint8_t *)&(value), 1, I2C_MEM_READ_TIMEOUT)

#define i2c_read_mem_20(i2c_dev, mem_addr, value)                                                                                              \
    do                                                                                                                                         \
    {                                                                                                                                          \
        HAL_I2C_Mem_Read((i2c_dev)->i2c, (i2c_dev)->dev_addr, (mem_addr), I2C_MEMADD_SIZE_8BIT, (uint8_t *)&(value), 3, I2C_MEM_READ_TIMEOUT); \
        (value) >>= 4;                                                                                                                         \
    } while (0)

#endif
