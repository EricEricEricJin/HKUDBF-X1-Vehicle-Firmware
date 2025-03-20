#include "main.h"
#include "cmsis_os.h"

#include "board.h"

#include "communicate.h"
#include "protocol.h"

#include "plane.h"
#include "sensor_task.h"

#include "log.h"

struct cmd_stick_val stick_val = {0};

struct sensor_data_decoded plane_sensor_data_decoded;

// initialize plane obj
struct plane plane;

struct plane_param plane_param = {
    .servo_params[PLANE_SERVO_AILERON] = {
        .ccr = SERVO_1_CCR, .type = SERVO_180DEG,
        .zero_deg = -5, .min_deg = -45 - 5, .max_deg = 45 - 5
    },
    .servo_params[PLANE_SERVO_RUDDER_L] = {
        .ccr = SERVO_2_CCR, .type = SERVO_180DEG,
        .zero_deg = -13, .min_deg = -45-13, .max_deg = 45-13
    },
    .servo_params[PLANE_SERVO_RUDDER_R] = {
        .ccr = SERVO_3_CCR, .type = SERVO_180DEG,
        .zero_deg = 0, .min_deg = -45, .max_deg = 45
    },
    .servo_params[PLANE_SERVO_ELEVATOR] = {
        .ccr = SERVO_4_CCR, .type = SERVO_180DEG,
        .zero_deg = 0, .min_deg = -45, .max_deg = 45
    },

    .direct_roll_coeff = 45, .direct_pitch_coeff = 45, .direct_yaw_coeff = 45,

    .lockatt_roll_coeff = 45, .lockatt_pitch_coeff = 45,  // todo

    // PID fdbk, ref, out all in degree unit

    .pid_param_pitch = {
        .p = 0.5f,
        .max_out = 45,
    },
    .pid_param_roll = {
        .p = 0.5f,
        .max_out = 45,
    }
};

void update_stick_val(uint8_t *data, uint16_t len)
{
    // SET_BOARD_LED_ON();
    memcpy(&stick_val, data, sizeof(stick_val));
    plane_set_stick_val(
        &plane, 
        stick_val.x / 32768.0f, 
        stick_val.y / 32768.0f,
        stick_val.z / 32768.0f);
}

void update_opmode(uint8_t *data, uint16_t len)
{
    plane_opmode_t opmode = (plane_opmode_t)data[0];
    log_i("set opmode = %d", opmode);
    plane_set_opmode(&plane, opmode);
}

struct communicate_recv_cmd plane_recv_cmd_table[] = {
    {CMD_ID_STICK_VAL, update_stick_val},
    {CMD_ID_SET_MODE, update_opmode}
};

__NO_RETURN void plane_task(void *args)
{
    plane_init(&plane, &plane_param);
    plane_set_opmode(&plane, PLANE_OPMODE_DIRECT);

    communicate_register_recv_cmd(plane_recv_cmd_table, sizeof(plane_recv_cmd_table) / sizeof(plane_recv_cmd_table[0]));

    // SET_BOARD_LED_ON();
    // uint32_t printtime = get_time_ms();
    while (1)
    {
        get_decoded_sensor_data(&plane_sensor_data_decoded, osWaitForever);
        
        // if (get_time_ms() - printtime > 1000)
        // {
        //     printtime = get_time_ms();
        //     log_i("roll = %f, pitch = %f, yaw = %f", plane_sensor_data_decoded.jy901_data.roll, plane_sensor_data_decoded.jy901_data.pitch, plane_sensor_data_decoded.jy901_data.yaw);
        // }
        
        plane_set_sensor_data(&plane, plane_sensor_data_decoded.jy901_data.roll, plane_sensor_data_decoded.jy901_data.pitch, plane_sensor_data_decoded.jy901_data.yaw);
        plane_calculate(&plane);
        osDelay(5);
        // SET_BOARD_LED_ON();
        // osDelay(500);
        // SET_BOARD_LED_OFF();
        // osDelay(500);
    }
}
