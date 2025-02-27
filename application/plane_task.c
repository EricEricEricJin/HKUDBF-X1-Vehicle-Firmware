#include "main.h"
#include "cmsis_os.h"

#include "board.h"

#include "communicate_task.h"
#include "protocol.h"

#include "plane.h"

struct cmd_stick_val stick_val = {0};

// initialize plane obj
struct plane plane;

struct plane_param plane_param = {
    .servo_params[PLANE_SERVO_AILERON] = {
        .ccr = SERVO_1_CCR, .type = SERVO_180DEG,
        .zero_deg = 0, .min_deg = -45, .max_deg = 45
    },
    .servo_params[PLANE_SERVO_RUDDER_L] = {
        .ccr = SERVO_2_CCR, .type = SERVO_180DEG,
        .zero_deg = 0, .min_deg = -45, .max_deg = 45
    },
    .servo_params[PLANE_SERVO_RUDDER_R] = {
        .ccr = SERVO_3_CCR, .type = SERVO_180DEG,
        .zero_deg = 0, .min_deg = -45, .max_deg = 45
    },
    .servo_params[PLANE_SERVO_ELEVATOR] = {
        .ccr = SERVO_4_CCR, .type = SERVO_180DEG,
        .zero_deg = 0, .min_deg = -45, .max_deg = 45
    },

    .direct_roll_coeff = 45, .direct_pitch_coeff = 45,

    .lockatt_roll_coeff = 0, .lockatt_pitch_coeff = 0,  // todo

    .pid_param_pitch = {
        0   // todo
    },
    .pid_param_roll = {
        0   // todo
    }
};

void update_stick_val(uint8_t *data, uint16_t len)
{
    SET_BOARD_LED_ON();
    memcpy(&stick_val, data, sizeof(stick_val));
    plane_set_stick_val(&plane, stick_val.x / 32768.0f, stick_val.y / 32768.0f);
}


struct communicate_recv_cmd plane_recv_cmd_table[] = {
    {CMD_ID_STICK_VAL, update_stick_val},
};

__NO_RETURN void plane_task(void *args)
{
    plane_init(&plane, &plane_param);
    plane_set_opmode(&plane, PLANE_OPMODE_DIRECT);

    communicate_register_recv_cmd(plane_recv_cmd_table, sizeof(plane_recv_cmd_table) / sizeof(plane_recv_cmd_table));

    while (1)
    {
        plane_calculate(&plane);
        osDelay(5);
        // SET_BOARD_LED_ON();
        // osDelay(500);
        // SET_BOARD_LED_OFF();
        // osDelay(500);
    }
}
