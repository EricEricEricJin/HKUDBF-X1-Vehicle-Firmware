#include "servo.h"


inline uint32_t servo_deg_to_pw(servo_type_t type, float deg)
{
    switch (type)
    {
    case SERVO_180DEG:
        deg = deg > 90 ? 90 : (deg < -90 ? -90 : deg);
        return 1400 + 1000 * (deg / 90); // 400us to 2400us
    default:
        break;
    }
    return 0;
}


void servo_init(servo_t servo, servo_param_t param)
{
    servo->param.ccr = param->ccr;
    servo->param.type = param->type;
    servo->param.zero_deg = param->zero_deg;
    servo->param.min_deg = param->min_deg;
    servo->param.max_deg = param->max_deg;

    pwm_set_width(servo->param.ccr, 0);
    servo->state = SERVO_OFF;
}

void servo_turn_on(servo_t servo)
{
    if (servo->state == SERVO_ON)
        return;
    
    pwm_set_width(servo->param.ccr, servo_deg_to_pw(SERVO_180DEG, servo->param.zero_deg));
    servo->state = SERVO_ON;
    servo->degree = servo->param.zero_deg;
}

void servo_turn_off(servo_t servo)
{
    pwm_set_width(servo->param.ccr, 0);
    servo->state = SERVO_OFF;
}

void servo_set_deg_abs(servo_t servo, float deg)
{
    if (servo->state != SERVO_ON)
        return;
    
    pwm_set_width(servo->param.ccr, servo_deg_to_pw(servo->param.type, deg));
    servo->degree = deg;
}

void servo_set_deg_trimmed(servo_t servo, float deg)
{
    if (servo->state != SERVO_ON)
        return;
    
    float deg_abs = servo->param.zero_deg + (servo->param.max_deg > servo->param.min_deg ? deg : -deg);
    if (SERVO_INVALID_DEG(deg_abs, servo->param.max_deg, servo->param.min_deg))
        return;

    pwm_set_width(servo->param.ccr, servo_deg_to_pw(servo->param.type, deg_abs));
    servo->degree = deg_abs;
}

float servo_get_deg_abs(servo_t servo)
{
    return servo->degree;
}

float servo_get_deg_trimmed(servo_t servo)
{
    return servo->degree - servo->param.zero_deg;
}
