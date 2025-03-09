#ifndef __JY901_H__
#define __JY901_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "drv_i2c.h"

enum {
    JY901_ORIENT_H,
    JY901_ORIENT_V,
};

enum {
    JY901_AXIS_6,
    JY901_AXIS_9,
};

typedef struct jy901* jy901_t;
typedef struct jy901_data_decoded* jy901_data_decoded_t;
typedef struct jy901_data_raw* jy901_data_raw_t;

// Currently the basic JY901
// Update to JY901B later to access GPS and Barometer (?)
struct jy901_data_raw
{
    uint16_t a_x, a_y, a_z;
    uint16_t w_x, w_y, w_z;
    uint16_t b_x, b_y, b_z;
    uint16_t roll, pitch, yaw;
};

struct jy901_data_decoded
{
    float a_x, a_y, a_z;
    float w_x, w_y, w_z;
    float b_x, b_y, b_z;
    float roll, pitch, yaw;
};

struct jy901
{
    struct i2c_dev i2c_dev;

    struct jy901_data_raw raw_data;

};

int jy901_init(jy901_t dev, uint16_t orient, uint16_t axis6);
int jy901_restart(jy901_t dev);
int jy901_update(jy901_t dev);
int jy901_decode(jy901_data_raw_t raw, jy901_data_decoded_t decoded);

#endif