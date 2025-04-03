#ifndef __DRV_IO_H__
#define __DRV_IO_H__

#include "stm32g431xx.h"
#include "main.h"

#define SET_BOARD_LED_ON() HAL_GPIO_WritePin(BOARD_LED_GPIO_Port, BOARD_LED_Pin, GPIO_PIN_SET) 
#define SET_BOARD_LED_OFF() HAL_GPIO_WritePin(BOARD_LED_GPIO_Port, BOARD_LED_Pin, GPIO_PIN_RESET)
#define SET_BOARD_LED_TOGGLE() HAL_GPIO_TogglePin(BOARD_LED_GPIO_Port, BOARD_LED_Pin)
#define GET_BOARD_BUTTON() HAL_GPIO_ReadPin(BOARD_BUTTON_GPIO_Port, BOARD_BUTTON_Pin)

#define SET_BUZZER_ON() HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET)
#define SET_BUZZER_TOGGLE() HAL_GPIO_TogglePin(BUZZER_GPIO_Port, BUZZER_Pin)
#define SET_BUZZER_OFF() HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET)


enum {
    DET_SW_DETACHED = GPIO_PIN_RESET,
    DET_SW_ATTACHED = GPIO_PIN_SET,
};

#define GET_DET_SW1() HAL_GPIO_ReadPin(DET_SW1_GPIO_Port, DET_SW1_Pin)

#define SERVO_1_CCR (&TIM2->CCR1)
#define SERVO_2_CCR (&TIM2->CCR2)
#define SERVO_3_CCR (&TIM2->CCR3)
#define SERVO_4_CCR (&TIM2->CCR4)

// #define LED_LL_CCR (&TIM3->CCR1)
#define LED_LEFT_CCR (&TIM3->CCR2)
// #define LED_RL_CCR (&TIM3->CCR3)
#define LED_RIGHT_CCR (&TIM3->CCR3)

void pwm_device_init();
void pwm_set_value(volatile uint32_t* ccr_ptr, uint32_t value);
void pwm_set_width(volatile uint32_t* ccr_ptr, uint32_t width_us);

void adc_init();
void adc_convert();
uint16_t adc_get_mv();

#endif
