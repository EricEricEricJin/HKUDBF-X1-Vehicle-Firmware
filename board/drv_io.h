#ifndef __DRV_IO_H__
#define __DRV_IO_H__

#include "stm32g431xx.h"
#include "main.h"

#define SET_BOARD_LED_ON() HAL_GPIO_WritePin(BOARD_LED_GPIO_Port, BOARD_LED_Pin, GPIO_PIN_SET) 
#define SET_BOARD_LED_OFF() HAL_GPIO_WritePin(BOARD_LED_GPIO_Port, BOARD_LED_Pin, GPIO_PIN_RESET)
#define GET_BOARD_BUTTON() HAL_GPIO_ReadPin(BOARD_BUTTON_GPIO_Port, BOARD_BUTTON_Pin)

#define SERVO_1_CCR (&TIM2->CCR1)
#define SERVO_2_CCR (&TIM2->CCR2)
#define SERVO_3_CCR (&TIM2->CCR3)
#define SERVO_4_CCR (&TIM2->CCR4)

#define LED_LL_CCR (&TIM3->CCR1)
#define LED_LR_CCR (&TIM3->CCR2)
#define LED_RL_CCR (&TIM3->CCR3)
#define LED_RR_CCR (&TIM3->CCR4)

void pwm_device_init();
void pwm_set_value(uint32_t* ccr_ptr, uint32_t value);
void pwm_set_width(uint32_t* ccr_ptr, uint32_t width_us);

#endif
