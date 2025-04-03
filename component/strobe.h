#ifndef __STROBE_H__
#define __STROBE_H__

#include "board.h"

typedef struct strobe* strobe_t;

enum strobe_state
{
    STROBE_DISABLE = 0,
    STROBE_ENABLE = 1,
};

struct strobe
{
    volatile uint32_t* ccr;
    uint32_t phase;
    uint32_t period;
    uint32_t ramp;
    int state;
};

void strobe_init(strobe_t strobe, volatile uint32_t* ccr, uint32_t phase, uint32_t period, uint32_t ramp);
void strobe_set(strobe_t strobe, int state);
void strobe_update(strobe_t strobe);

#endif
