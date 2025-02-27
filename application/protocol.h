#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

// #include <time.h>
#include <stdint.h>

// << Set Direct, S1, S2, AP >>
#define CMD_ID_SET_MODE (0xff)

enum {
    CMD_MODE_DIRECT = 0,
    CMD_MODE_LOCKATT = 1,
    CMD_MODE_AUTOPILOT = 2
};

typedef struct cmd_set_mode* cmd_set_mode_t;
struct __attribute__((__packed__)) cmd_set_mode {uint8_t mode};

#define CMD_ID_STICK_VAL (0x02)
typedef struct cmd_stick_val* cmd_stick_val_t;
struct __attribute__((__packed__)) cmd_stick_val
{
    int16_t x;
    int16_t y;
};

#endif