#ifndef __SENSOR_TASK_H__
#define __SENSOR_TASK_H__

#include "main.h"
#include "cmsis_os.h"

#include "jy901.h"
#include "bmp280.h"
#include "protocol.h"

typedef struct sensor_data_decoded* sensor_data_decoded_t;

struct sensor_data_decoded
{
    struct jy901_data_decoded jy901_data;
    struct bmp280_data bmp280_data;
};

__NO_RETURN void sensor_task(void* args);

void get_decoded_sensor_data(sensor_data_decoded_t data, uint32_t timeout);
void get_export_sensor_data(sensor_data_export_t data, uint32_t timeout);


#endif
