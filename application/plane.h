#ifndef __PLANE_H__
#define __PLANE_H__

#include "main.h"
#include "pid.h"
#include "servo.h"
#include "stdint.h"

typedef enum {
    PLANE_OPMODE_OFF,
    PLANE_OPMODE_IDLE,
    PLANE_OPMODE_DIRECT,
    PLANE_OPMODE_LOCKATT,
    PLANE_OPMODE_AUTOPILOT,
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
    
    struct servo servos[PLANE_SERVO_MAX_NUM];
    struct pid  pid_pitch, pid_roll;

    plane_opmode_t opmode;
    float stick_x, stick_y, stick_z;

    float roll, pitch, yaw;

    osMutexId_t stick_val_mutex_id, sensor_data_mutex_id;
};

struct plane_param {
    struct servo_param servo_params[PLANE_SERVO_MAX_NUM]; 
    float direct_roll_coeff, direct_pitch_coeff, direct_yaw_coeff;
    float lockatt_roll_coeff, lockatt_pitch_coeff;
    struct pid_param pid_param_pitch, pid_param_roll;
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

#endif 