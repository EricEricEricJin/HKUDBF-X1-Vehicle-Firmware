#include "plane.h"
#include "my_math.h"

const osMutexAttr_t stick_val_mutex_attr = {
    .name = "STICK_VAL_MUTEX",
};

const osMutexAttr_t sensor_data_mutex_attr = {
    .name = "SENSOR_DATA_MUTEX",
};

void plane_init(plane_t plane, plane_param_t param)
{
    // copy needed param to member 
    plane->direct_roll_coeff = param->direct_roll_coeff;
    plane->direct_pitch_coeff = param->direct_pitch_coeff;
    plane->direct_yaw_coeff = param->direct_yaw_coeff;
    plane->lockatt_roll_coeff = param->lockatt_roll_coeff;
    plane->lockatt_pitch_coeff = param->lockatt_pitch_coeff;
    
    // initialize servos
    for (int i = 0; i < PLANE_SERVO_MAX_NUM; i++)
        servo_init(&plane->servos[i], &param->servo_params[i]);

    // initialize PID
    pid_struct_init(
        &plane->pid_pitch, 
        param->pid_param_pitch.max_out,
        param->pid_param_pitch.integral_limit,
        param->pid_param_pitch.p,
        param->pid_param_pitch.i,
        param->pid_param_pitch.d
    );
    pid_struct_init(
        &plane->pid_roll, 
        param->pid_param_roll.max_out,
        param->pid_param_roll.integral_limit,
        param->pid_param_roll.p,
        param->pid_param_roll.i,
        param->pid_param_roll.d
    );
        
    plane->opmode = PLANE_OPMODE_OFF;

    // Create the lock for stick_val
    plane->stick_val_mutex_id = osMutexNew(&stick_val_mutex_attr);
    plane->sensor_data_mutex_id = osMutexNew(&sensor_data_mutex_attr);
}

void plane_set_opmode(plane_t plane, plane_opmode_t mode)
{
    plane->opmode = mode;
}

void plane_set_stick_val(plane_t plane, float x, float y, float z)
{
    VAL_LIMIT(x, -1.0f, 1.0f);
    VAL_LIMIT(y, -1.0f, 1.0f);
    VAL_LIMIT(z, -1.0f, 1.0f);

    osMutexAcquire(plane->stick_val_mutex_id, osWaitForever);
    plane->stick_x = x;
    plane->stick_y = y;
    plane->stick_z = z;
    osMutexRelease(plane->stick_val_mutex_id);
}

void plane_set_sensor_data(plane_t plane, float roll, float pitch, float yaw)
{
    osMutexAcquire(plane->sensor_data_mutex_id, osWaitForever);
    plane->roll = roll;
    plane->pitch = pitch;
    plane->yaw = yaw;
    osMutexRelease(plane->sensor_data_mutex_id);
}

void plane_calculate(plane_t plane)
{
    
    if (plane->opmode == PLANE_OPMODE_OFF)
    {
        for(int i = 0; i < PLANE_SERVO_MAX_NUM; i++)
            servo_turn_off(&plane->servos[i]);
    }
    else 
    {
        osMutexAcquire(plane->stick_val_mutex_id, osWaitForever);
        float stick_x = plane->stick_x;
        float stick_y = plane->stick_y;
        float stick_z = plane->stick_z;
        osMutexRelease(plane->stick_val_mutex_id);

        float deg_aileron = 0;
        float deg_elevator = 0;
        float deg_rudder = 0;

        for(int i = 0; i < PLANE_SERVO_MAX_NUM; i++)
           servo_turn_on(&plane->servos[i]);

        switch (plane->opmode)
        {
        case PLANE_OPMODE_IDLE:
        {
            deg_aileron = 0;
            deg_elevator = 0;
            deg_rudder = 0;
        }
        break;

        case PLANE_OPMODE_DIRECT:
        {
            deg_aileron = plane->direct_roll_coeff * stick_x;
            deg_elevator = plane->direct_pitch_coeff * stick_y;
            deg_rudder = plane->direct_yaw_coeff * stick_z;
        }
        break;

        case PLANE_OPMODE_LOCKATT:
        {
            // get sensor
            osMutexAcquire(plane->sensor_data_mutex_id, osWaitForever);
            float roll = plane->roll;
            float pitch = plane->pitch;
            float yaw = plane->yaw;
            osMutexRelease(plane->sensor_data_mutex_id);

            float target_roll = plane->lockatt_roll_coeff * stick_x;
            float target_pitch = plane->lockatt_pitch_coeff * stick_y;

            // calculate PID
            deg_aileron = pid_calculate(&plane->pid_roll, roll, target_roll);
            deg_elevator = pid_calculate(&plane->pid_pitch, pitch, target_pitch);
            deg_rudder = plane->direct_yaw_coeff * stick_z;
        }
        break;
        
        case PLANE_OPMODE_AUTOPILOT:
        {
            // todo
        }
        break;

        default:
            break;
        }

        servo_set_deg_trimmed(&plane->servos[PLANE_SERVO_AILERON], deg_aileron);
        servo_set_deg_trimmed(&plane->servos[PLANE_SERVO_ELEVATOR], deg_elevator);
        servo_set_deg_trimmed(&plane->servos[PLANE_SERVO_RUDDER_L], deg_rudder);
        servo_set_deg_trimmed(&plane->servos[PLANE_SERVO_RUDDER_R], deg_rudder);

    }    
}
