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
        osDelay(500);
        SET_BOARD_LED_OFF();
        osDelay(500);
    }
}



