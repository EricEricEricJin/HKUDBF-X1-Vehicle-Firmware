#include "sensor_task.h"
#include "board.h"
#include "bmp280.h"
#include "jy901.h"

// #include "communicate_task.h"
// #include "protocol.h"

// todo: add BMP280 things

#include "log.h"

struct bmp280 bmp280 = {
    .i2c_dev = {
        .i2c = &hi2c2,
        .dev_addr = 0x77 << 1,
    }
};

struct jy901 jy901 = {
    .i2c_dev = {
        .i2c = &hi2c1,
        .dev_addr = 0x50 << 1,
    }
};

struct jy901_data_decoded jy901_data;

__NO_RETURN void sensor_task(void *args)
{
    // Initialize BMP280
    osDelay(500);
    int ret = bmp280_init(&bmp280);
    log_i("bmp280 init = %d", ret);
    
    osDelay(500);
    
    // Initialize JY901
    osDelay(500);
    ret = jy901_init(&jy901, 0, 0);
    log_i("jy901 init = %d", ret);
    
    while (1)
    {
        SET_BOARD_LED_ON();
        
        jy901_update(&jy901);
        jy901_decode(&jy901.raw_data, &jy901_data);
        log_i("pitch=%f, roll=%f, yaw=%f", jy901_data.pitch * 180, jy901_data.roll * 180, jy901_data.yaw * 180);
        
        bmp280_update(&bmp280);
        log_i("temp=%f, pressure=%f", bmp280.data.temperature, bmp280.data.pressure);


        // usart3_transmit((uint8_t *)"test\n", 5);
        // char log_str[128];
        // int len = 0;
        // len += snprintf(&log_str[len], 128 - len, "\r\n");
        // len += snprintf(&log_str[len], 128 - len, "[%d.%03d]", (int)1000 / 1000, (int)1000 % 1000);
        // // len += snprintf(&log_str[len], 128 - len, "[%s][%s]", "log", LOG_LEVEL_TAGS[4]);
        // // len += log_printf_to_buffer(&log_str[len], 128 - len, "test");
        // // usart3_transmit((uint8_t *)(uint8_t *)log_str, len);

        osDelay(500);
        SET_BOARD_LED_OFF();
        osDelay(500);
    }
}
