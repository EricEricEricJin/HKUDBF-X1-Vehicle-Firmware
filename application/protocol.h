#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

// #include <time.h>
#include <stdint.h>

// << Set Direct, S1, S2, AP >>
#define CMD_ID_SET_MODE (0xff)

// enum {
//     CMD_MODE_DIRECT = 0,
//     CMD_MODE_LOCKATT = 1,
//     CMD_MODE_AUTOPILOT = 2
// };

// typedef struct cmd_set_mode* cmd_set_mode_t;
// struct __attribute__((__packed__)) cmd_set_mode {uint8_t mode};

#define CMD_ID_STICK_VAL (0x02)
typedef struct cmd_stick_val* cmd_stick_val_t;
struct __attribute__((__packed__)) cmd_stick_val
{
    int16_t x;
    int16_t y;
    int16_t z;
};

#define CMD_ID_PID_PARAM (0xEE)

#define DATA_ID_SENSOR_EXPORT (0x03)
typedef struct sensor_data_export* sensor_data_export_t;
struct __attribute__((__packed__)) sensor_data_export
{
    int16_t a_x, a_y, a_z;
    int16_t w_x, w_y, w_z;
    int16_t b_x, b_y, b_z;
    int16_t roll, pitch, yaw;

    // uint8_t volt_bat;
    uint16_t volt_bat;

    int16_t pressure;
    int16_t temperature;

    int8_t elevator, aileron, rudder_l, rudder_r;
    uint8_t state;
    uint8_t opmode;
};

#define DATA_ID_SERVO_FDBK (0x04)
typedef struct servo_fdbk* servo_fdbk_t;
struct __attribute__((__packed__)) servo_fdbk
{
    int8_t elevator, aileron, rudder_l, rudder_r;
    uint8_t mode;
};



#endif