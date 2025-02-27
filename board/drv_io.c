#include "drv_io.h"
#include "tim.h"

void pwm_device_init(void)
{
    // TIM2 for servos
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);

    // TIM3 for LEDs
    // HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    // HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    // HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
    // HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
}


void pwm_set_value(uint32_t* ccr_ptr, uint32_t value)
{
    *ccr_ptr = value;
}

void pwm_set_width(uint32_t* ccr_ptr, uint32_t width_us)
{
    pwm_set_value(ccr_ptr, width_us);   // pre-scaled freq = 1MHz
}

