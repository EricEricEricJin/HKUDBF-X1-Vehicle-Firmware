#include "main.h"
#include "cmsis_os.h"

#include "iwdg.h"

#include "board.h"

#include "communicate.h"
#include "protocol.h"

#include "plane.h"
#include "sensor_task.h"

#include "strobe.h"
#include "log.h"

#include "appcfg.h"



struct cmd_stick_val stick_val = {0};

struct sensor_data_decoded plane_sensor_data_decoded;

// initialize plane obj
struct plane plane;

struct plane_info plane_info;

struct strobe strobe_left = {
    .ccr = LED_LEFT_CCR,
    .phase = 0,
    .period = 1000,
    .ramp = 0,
    .state = DISABLE
};

struct strobe strobe_right = {
    .ccr = LED_RIGHT_CCR,
    .phase = 500,
    .period = 1000,
    .ramp = 0,
    .state = DISABLE
};

#define AIL_ZERO 0
#define RUD_L_ZERO (-9)
#define RUD_R_ZERO (19)
#define ELE_ZERO (-10)

struct plane_param plane_param = {
    .servo_params[PLANE_SERVO_AILERON] = {
        .ccr = SERVO_1_CCR, .type = SERVO_180DEG,
        .zero_deg = AIL_ZERO, .min_deg = -45 + AIL_ZERO, .max_deg = 45 + AIL_ZERO
    },
    .servo_params[PLANE_SERVO_RUDDER_L] = {
        .ccr = SERVO_2_CCR, .type = SERVO_180DEG,
        .zero_deg = RUD_L_ZERO, .min_deg = -45 + RUD_L_ZERO, .max_deg = 45 + RUD_L_ZERO
    },
    .servo_params[PLANE_SERVO_RUDDER_R] = {
        .ccr = SERVO_3_CCR, .type = SERVO_180DEG,
        .zero_deg = RUD_R_ZERO, .min_deg = -45 + RUD_R_ZERO, .max_deg = 45 + RUD_R_ZERO
    },
    .servo_params[PLANE_SERVO_ELEVATOR] = {
        .ccr = SERVO_4_CCR, .type = SERVO_180DEG,
        .zero_deg = ELE_ZERO, .min_deg = 45 + ELE_ZERO, .max_deg = -45 + ELE_ZERO
    },

    .direct_roll_coeff = 45, .direct_pitch_coeff = 45, .direct_yaw_coeff = 45,
    .lockatt_roll_coeff = 45, .lockatt_pitch_coeff = 45, 
    .autopilot_roll_coeff = 45, .autopilot_pitch_coeff = 45, .autopilot_yaw_coeff = 360,

    // PID fdbk, ref, out all in degree unit

    .pid_param_pitch = {
        .p = 1.0f,
        .max_out = 45,
    },
    .pid_param_roll = {
        .p = 0.5f,
        .max_out = 45,
    },
    .pid_param_yaw = {
        .p = 0.5f,
        .max_out = 45,
    },
};

void update_stick_val(uint8_t *data, uint16_t len)
{
    memcpy(&stick_val, data, sizeof(stick_val));
    plane_set_stick_val( &plane, 
        stick_val.x / 32768.0f, 
        stick_val.y / 32768.0f,
        stick_val.z / 32768.0f
    );
    // SET_BOARD_LED_TOGGLE();
}

void update_opmode(uint8_t *data, uint16_t len)
{
    plane_opmode_t opmode = (plane_opmode_t)data[0];
    log_i("set opmode = %d", opmode);
    plane_set_opmode(&plane, opmode);
}

static struct pid_param pp_pitch;
static struct pid_param pp_roll; 

void update_pid_param(uint8_t *data, uint16_t len)
{
    if (len != sizeof(float) * 12)
    {
        log_e("pid param len %d", len);
        return;
    }
    log_i("update pid param");

    float* pparr = (float*)data;
    
    pp_pitch.p = pparr[0];
    pp_pitch.i = pparr[1];
    pp_pitch.d = pparr[2];
    pp_pitch.input_max_err = pparr[3];
    pp_pitch.max_out = pparr[4];
    pp_pitch.integral_limit = pparr[5];

    log_i("1 assigned");

    pp_roll.p = pparr[6];
    pp_roll.i = pparr[7];
    pp_roll.d = pparr[8];
    pp_roll.input_max_err = pparr[9];
    pp_roll.max_out = pparr[10];
    pp_roll.integral_limit = pparr[11];
    
    log_i("2 assigned");

    plane_update_pid_param(&plane, pp_pitch, pp_roll);
    // log_i("pitch: p = %f, i = %f, roll: p = %f, i = %f", pp_pitch.p, pp_pitch.i, pp_roll.p, pp_roll.i); 
    log_i("p=%f, i=%f, d=%f, max_out=%f, integral_limit=%f, max_err=%f", pp_pitch.p, pp_pitch.i, pp_pitch.d, pp_pitch.max_out, pp_pitch.integral_limit, pp_pitch.input_max_err);
    // log_i("p=%f, i=%f, d=%f, max_out=%f, integral_limit=%f, max_err=%f", pp_roll.p, pp_roll.i, pp_roll.d, pp_roll.max_out, pp_roll.integral_limit, pp_roll.input_max_err);
}

struct communicate_recv_cmd plane_recv_cmd_table[] = {
    {CMD_ID_STICK_VAL, update_stick_val},
    {CMD_ID_SET_MODE, update_opmode},
    {CMD_ID_PID_PARAM, update_pid_param},
};


typedef enum {
    PLANE_TASK_IDLE,
    PLANE_TASK_ATTACHED,
    PLANE_TASK_STABLE,
    PLANE_TASK_TURN,
    PLANE_TASK_DROP,
} plane_task_state_t;

typedef enum {
    FLY_DIR_CW,
    FLY_DIR_CCW,
} fly_dir_t;

fly_dir_t get_fly_dir(float release_mag_hdg)
{
    if (abs(release_mag_hdg - RWY_HDG_CW) < abs(release_mag_hdg - RWY_HDG_CCW))
        return FLY_DIR_CW;
    else
        return FLY_DIR_CCW; 
};

__NO_RETURN void plane_task(void *args)
{
    plane_init(&plane, &plane_param);

    plane_set_opmode(&plane, PLANE_OPMODE_IDLE);
    #ifndef AIAA_M3
        communicate_register_recv_cmd(plane_recv_cmd_table, sizeof(plane_recv_cmd_table) / sizeof(plane_recv_cmd_table[0]));
    #endif

    SET_BUZZER_OFF();

    uint32_t buzz_toggle_time = 0;

    
    plane_task_state_t state = PLANE_TASK_IDLE;
    
    fly_dir_t fly_dir;
    int target_turned_hdg;

    uint32_t detach_time = 0;
    
    uint32_t update_time = xTaskGetTickCount();
    while (1)
    {
        // Update sensor data
        get_decoded_sensor_data(&plane_sensor_data_decoded, osWaitForever);
        plane_set_sensor_data(&plane, plane_sensor_data_decoded.jy901_data.roll, plane_sensor_data_decoded.jy901_data.pitch, plane_sensor_data_decoded.jy901_data.yaw);
        
        // State machine:
        // State Transition:
        //  IDLE -- switch pressed / --> ATTACHED
        //  ATTACHED -- switch released / --> STABLE
        //  STABLE -- stable time out (2 seconds) / --> TURN
        //  TURN -- target heading reached / --> DROP
        // Output:
        //  STABLE: rudder=0, aileron=PID(0), elevator=PID(STABLE_Y_VAL)
        //  TURN: elevator=PID(TURN_Y_VAL), rudder=PID(target_yaw), roll=0.5*rudder
        //  DROP: elevator=PID(DROP_Y_VAL), rudder=0, roll=0
        switch (state)
        {
        case PLANE_TASK_IDLE:
        {
            if (GET_DET_SW1() == DET_SW_ATTACHED)
            {
                SET_BOARD_LED_ON();
                state = PLANE_TASK_ATTACHED;
            }
        }
        break;

        case PLANE_TASK_ATTACHED:
        {
            if (GET_DET_SW1() == DET_SW_DETACHED)
            {
                // record mag heading 
                fly_dir = get_fly_dir(plane_sensor_data_decoded.jy901_data.yaw);
                target_turned_hdg = (fly_dir == FLY_DIR_CW) ? RWY_HDG_CW : RWY_HDG_CCW;
                detach_time = get_time_ms();

                plane_set_opmode(&plane, PLANE_OPMODE_LOCKATT);

                SET_BOARD_LED_OFF();
                strobe_set(&strobe_left, STROBE_ENABLE);
                strobe_set(&strobe_right, STROBE_ENABLE);

                state = PLANE_TASK_STABLE;
            }
        }
        break;

        case PLANE_TASK_STABLE:
        {
            plane_set_stick_val(&plane, 0, STABLE_Y_VAL, 0);
            if (get_time_ms() - detach_time > STABLE_TIME)
            {
                plane_set_opmode(&plane, PLANE_OPMODE_LOCKATT);
                state = PLANE_TASK_TURN;
            }
        }
        break;

        case PLANE_TASK_TURN:
        {
            float x_val, y_val, z_val;

            // rudder    
            z_val = (fly_dir == FLY_DIR_CW ? RWY_HDG_CW : RWY_HDG_CCW) / 360.0f;
            if (abs(plane_sensor_data_decoded.jy901_data.yaw - target_turned_hdg) < 20.0f)
            {
                plane_set_opmode(&plane, PLANE_OPMODE_LOCKATT);
                state = PLANE_TASK_DROP;
            }
            
            // roll
            plane_get_info(&plane, &plane_info);
            x_val = plane_info.deg_rudder / 2.0f;

            // pitch
            y_val = TURN_Y_VAL;
            plane_set_stick_val(&plane, x_val, y_val, z_val);
        }
        break;

        case PLANE_TASK_DROP:
        {
            plane_set_stick_val(&plane, 0, DROP_Y_VAL, 0);
        }
        break;
        
        default:
            break;
        }

        // All none-blocking updates
        plane_calculate(&plane);
        strobe_update(&strobe_left);
        strobe_update(&strobe_right);

        // Feed the doggy
        HAL_IWDG_Refresh(&hiwdg);

        // Delay 5ms
        vTaskDelayUntil(&update_time, 5);
    }
}

void get_export_servo_fdbk(servo_fdbk_t data)
{
    plane_get_info(&plane, &plane_info);
    data->elevator = (int8_t)plane_info.deg_elevator;
    data->aileron = (int8_t)plane_info.deg_aileron;
    data->rudder_l = (int8_t)plane_info.deg_rudder;
    data->rudder_r = (int8_t)plane_info.deg_rudder;
    data->mode = plane_info.opmode;
}
