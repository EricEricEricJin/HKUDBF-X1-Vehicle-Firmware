#ifndef __PLANE_H__
#define __PLANE_H__

#include "main.h"
#include "pid.h"
#include "servo.h"
#include "stdint.h"

typedef enum {
    PLANE_OPMODE_OFF = 0,
    PLANE_OPMODE_IDLE = 1,
    PLANE_OPMODE_DIRECT = 2,
    PLANE_OPMODE_LOCKATT = 3,
    PLANE_OPMODE_AUTOPILOT = 4,
} plane_opmode_t;

typedef enum {
    PLANE_SERVO_AILERON = 0,
    PLANE_SERVO_RUDDER_L = 1,
    PLANE_SERVO_RUDDER_R = 2,
    PLANE_SERVO_ELEVATOR = 3,
    PLANE_SERVO_MAX_NUM
};

typedef struct plane* plane_t;
typedef struct plane_param* plane_param_t;

struct plane {

    float direct_roll_coeff, direct_pitch_coeff, direct_yaw_coeff;
    float lockatt_roll_coeff, lockatt_pitch_coeff;
    float autopilot_roll_coeff, autopilot_pitch_coeff, autopilot_yaw_coeff;
    
    struct servo servos[PLANE_SERVO_MAX_NUM];
    struct pid  pid_pitch, pid_roll, pid_yaw;

    plane_opmode_t opmode;
    float stick_x, stick_y, stick_z;

    float roll, pitch, yaw;

    osMutexId_t stick_val_mutex_id, sensor_data_mutex_id, pid_param_mutex_id;
};

struct plane_param {
    struct servo_param servo_params[PLANE_SERVO_MAX_NUM]; 
    float direct_roll_coeff, direct_pitch_coeff, direct_yaw_coeff;
    float lockatt_roll_coeff, lockatt_pitch_coeff;
    float autopilot_roll_coeff, autopilot_pitch_coeff, autopilot_yaw_coeff;
    struct pid_param pid_param_pitch, pid_param_roll, pid_param_yaw;
};

typedef struct plane_info* plane_info_t;
struct plane_info {
    float deg_aileron, deg_elevator, deg_rudder;
    uint8_t opmode;
};


void plane_init(plane_t plane, plane_param_t param);

void plane_set_opmode(plane_t plane, plane_opmode_t mode);

void plane_set_stick_val(plane_t plane, float x, float y, float z);

void plane_set_sensor_data(plane_t plane, float roll, float pitch, float yaw);

void plane_calculate(plane_t plane);

void plane_get_info(plane_t plane, plane_info_t info);

void plane_update_pid_param(plane_t plane, struct pid_param pitch_param, struct pid_param roll_param);

#endif 