#include "board.h"
#include "sys.h"
#include "tim.h"

void board_config()
{
    SET_BOARD_LED_OFF();
    pwm_device_init();
    usart1_manage_init();
    usart3_manage_init();
}

uint32_t get_time_us(void)
{
    return TIM17->CNT;
}

uint32_t get_time_ms(void)
{
    return HAL_GetTick();
}

float get_time_ms_us(void)
{
    return get_time_ms() + get_time_us() / 1000.0f;
}