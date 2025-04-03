#include "strobe.h"

#define STROBE_MAX UINT16_MAX


uint16_t strobe_value(uint32_t delta_time, uint32_t period, uint32_t ramp)
{

    // if (delta_time < period / 2)
    //     return STROBE_MAX;
    // else
    //     return 0;

    int32_t value;

    if (delta_time < ramp / 2)
        value = (0.5f + delta_time / (float)ramp) * STROBE_MAX;
    else if (delta_time < period / 2 - ramp / 2)
        value = STROBE_MAX;
    else if (delta_time < period / 2 + ramp / 2)
        value = (0.5f + (period / 2 - delta_time) / (float)ramp) * STROBE_MAX;
    else if (delta_time < period - ramp / 2)
        value = 0;
    else
        value = (0.5f + (delta_time - period) / (float)ramp) * STROBE_MAX;
    
    VAL_LIMIT(value, 0, STROBE_MAX);
    return (uint16_t)value;
};

void strobe_init(strobe_t strobe, volatile uint32_t* ccr, uint32_t phase, uint32_t period, uint32_t ramp)
{
    strobe->phase = phase;
    strobe->state = STROBE_DISABLE;
    strobe->ccr = ccr;
    strobe->period = period;
    strobe->ramp = ramp;
}

void strobe_set(strobe_t strobe, int state)
{
    strobe->state = state;
}

void strobe_update(strobe_t strobe)
{
    if (strobe->state == STROBE_ENABLE)
        pwm_set_value(strobe->ccr, strobe_value((get_time_ms() - strobe->phase) % strobe->period, strobe->period, strobe->ramp));
    else
        pwm_set_width(strobe->ccr, 0); 
}
