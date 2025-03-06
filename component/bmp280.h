// Modified by Eric Jin on 2025 for DBF X1 Firmware 

/******************************************************************************
 * Copyright (C) 2024 Eric Jin <https://github.com/ericericericjin>
 *
 * Everyone is permitted to copy and distribute verbatim or modified copies
 * of this program, and changing it is allowed as long as the name is changed.
 *****************************************************************************/


#ifndef __BMP280_H__
#define __BMP280_H__

#include "drv_i2c.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct bmp280 *bmp280_t;

struct bmp280_cal
{
    uint16_t cal_t1;
    int16_t cal_t2, cal_t3;
    uint16_t cal_p1;
    int16_t cal_p2, cal_p3, cal_p4, cal_p5, cal_p6, cal_p7, cal_p8, cal_p9;
};

struct bmp280_data
{
    float temperature;
    float pressure;
};

struct bmp280
{
    struct i2c_dev i2c_dev;

    int raw_value;

    struct bmp280_cal cal;
    struct bmp280_data data;
};

int bmp280_init(bmp280_t dev);
int bmp280_update(bmp280_t dev);

#endif