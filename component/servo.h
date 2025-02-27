#ifndef __SERVO_H__
#define __SERVO_H__

#include "board.h"


#define SERVO_INVALID_DEG(deg, lima, limb) ((lima) >= (limb) ? \
    ((deg) > (lima) || (deg) < (limb)) : \
    ((deg) < (lima) || (deg) > (limb)))

typedef enum {
    SERVO_180DEG,
    SERVO_90DEG
} servo_type_t;

typedef enum {
    SERVO_ON,
    SERVO_OFF,
} servo_state_t;

typedef struct servo* servo_t;
typedef struct servo_param* servo_param_t;

struct servo_param
{
    volatile uint32_t* ccr;
    servo_type_t type;
    
    float zero_deg, max_deg, min_deg;
};

struct servo {
    struct servo_param param;

    servo_state_t state;
    float degree;
};


void servo_init(servo_t servo, servo_param_t param);
void servo_turn_on(servo_t servo);

void servo_turn_off(servo_t servo);
void servo_set_deg_abs(servo_t servo, float deg);
void servo_set_deg_trimmed(servo_t servo, float deg);

#endif 