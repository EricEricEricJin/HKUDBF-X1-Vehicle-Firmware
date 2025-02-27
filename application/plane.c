#include "plane.h"
#include "my_math.h"

const osMutexAttr_t stick_val_mutex_attr = {
    .name = "STICK_VAL_MUTEX",
};

void plane_init(plane_t plane, plane_param_t param)
{
    // copy needed param to member 
    plane->direct_roll_coeff = param->direct_roll_coeff;
    plane->direct_pitch_coeff = param->direct_pitch_coeff;
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

}

void plane_set_opmode(plane_t plane, plane_opmode_t mode)
{
    plane->opmode = mode;
}

void plane_set_stick_val(plane_t plane, float x, float y)
{   
    VAL_LIMIT(x, -1.0f, 1.0f);
    VAL_LIMIT(y, -1.0f, 1.0f);
 
    osMutexAcquire(plane->stick_val_mutex_id, osWaitForever);
    plane->stick_x = x;
    plane->stick_y = y;
    osMutexRelease(plane->stick_val_mutex_id);
}

void plane_calculate(plane_t plane)
{
    osMutexAcquire(plane->stick_val_mutex_id, osWaitForever);
    float stick_x = plane->stick_x;
    float stick_y = plane->stick_y;
    osMutexRelease(plane->stick_val_mutex_id);

    if (plane->opmode == PLANE_OPMODE_OFF)
    {
        for(int i = 0; i < PLANE_SERVO_MAX_NUM; i++)
            servo_turn_off(&plane->servos[i]);
    }
    else 
    {
        for(int i = 0; i < PLANE_SERVO_MAX_NUM; i++)
           servo_turn_on(&plane->servos[i]);

        switch (plane->opmode)
        {
        case PLANE_OPMODE_IDLE:
        {
            for(int i = 0; i < PLANE_SERVO_MAX_NUM; i++)
                servo_set_deg_trimmed(&plane->servos[i], 0);
        }
        break;

        case PLANE_OPMODE_DIRECT:
        {
            
            float deg_aileron = plane->direct_roll_coeff * stick_x;
            float deg_elevator = plane->direct_pitch_coeff * stick_y;
            float deg_rudder = 0.0f;

            servo_set_deg_trimmed(&plane->servos[PLANE_SERVO_AILERON], deg_aileron);
            servo_set_deg_trimmed(&plane->servos[PLANE_SERVO_ELEVATOR], deg_elevator);
            servo_set_deg_trimmed(&plane->servos[PLANE_SERVO_RUDDER_L], deg_rudder);
            servo_set_deg_trimmed(&plane->servos[PLANE_SERVO_RUDDER_R], deg_rudder);
        }
        break;

        case PLANE_OPMODE_LOCKATT:
        {
            // get sensor
            // todo
        }
        break;
        
        case PLANE_OPMODE_AUTOPILOT:
        {

        }
        break;

        default:
            break;
        }

    }    
}