#include "sensor_task.h"
#include "board.h"

// #include "communicate_task.h"
// #include "protocol.h"

// todo: add BMP280 things

#include "log.h"

__NO_RETURN void sensor_task(void *args)
{

    while (1)
    {
        SET_BOARD_LED_ON();

        log_i("test");
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
