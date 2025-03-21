#include "jy901.h"
#include <string.h>
#include <stdio.h>


inline float to_float(int x)
{
    float ret = x / 32768.0f;
    return ret >= 1 ? ret - 2 : ret;
}

// #define WT16(fd, reg, value) wiringPiI2CWriteReg16((fd), (reg), (value))
// #define RD16(fd, reg, value) wiringPiI2CReadReg16((fd), (reg))

int jy901_init(jy901_t dev, uint16_t orient, uint16_t axis6)
{
    if (dev == NULL)
        return -1;

    if (i2c_check(&dev->i2c_dev) != HAL_OK)
        return -1;


    // set orient
    uint16_t value = 0x00;
    i2c_write_mem_16(&dev->i2c_dev, 0x23, value);

    // set axis9
    value = 0x00;
    i2c_write_mem_16(&dev->i2c_dev, 0x24, value);

    // todo: play with more settings

    jy901_restart(dev);
}

int jy901_restart(jy901_t dev) { 
    uint16_t value = 0xFF;
    return i2c_write_mem_16(&dev->i2c_dev, 0x00, value); 
}

int jy901_update(jy901_t dev)
{
    i2c_read_mem_16(&dev->i2c_dev, 0x34, dev->raw_data.a_x);
    i2c_read_mem_16(&dev->i2c_dev, 0x35, dev->raw_data.a_y);
    i2c_read_mem_16(&dev->i2c_dev, 0x36, dev->raw_data.a_z);

    i2c_read_mem_16(&dev->i2c_dev, 0x37, dev->raw_data.w_x);
    i2c_read_mem_16(&dev->i2c_dev, 0x38, dev->raw_data.w_y);
    i2c_read_mem_16(&dev->i2c_dev, 0x39, dev->raw_data.w_z);

    
    i2c_read_mem_16(&dev->i2c_dev, 0x3d, dev->raw_data.roll);
    i2c_read_mem_16(&dev->i2c_dev, 0x3e, dev->raw_data.pitch);
    i2c_read_mem_16(&dev->i2c_dev, 0x3f, dev->raw_data.yaw);
    
    // printf("IMU update!\n");
    return 0;
}

int jy901_decode(jy901_data_raw_t raw, jy901_data_decoded_t decoded)
{
    decoded->a_x = to_float(raw->a_x);
    decoded->a_y = to_float(raw->a_y);
    decoded->a_z = to_float(raw->a_z);

    decoded->w_x = to_float(raw->w_x);
    decoded->w_y = to_float(raw->w_y);
    decoded->w_z = to_float(raw->w_z);

    decoded->b_x = to_float(raw->b_x);
    decoded->b_y = to_float(raw->b_y);
    decoded->b_z = to_float(raw->b_z);

    decoded->roll = 180.0f * to_float(raw->roll);
    decoded->pitch = -180.0f * to_float(raw->pitch);
    decoded->yaw = 180.0f * to_float(raw->yaw);

    return 0;
}