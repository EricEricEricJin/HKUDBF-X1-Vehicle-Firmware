#include "board.h"


void board_config()
{
    SET_BOARD_LED_OFF();
    pwm_device_init();
    usart1_manage_init();
}
