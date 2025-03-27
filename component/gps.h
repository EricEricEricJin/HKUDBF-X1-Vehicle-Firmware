#ifndef __GPS_H__
#define __GPS_H__

#include "main.h"
#include "sys.h"
#include "fifo.h"

#include "cmsis_os.h"

#define GPS_RX_BUF_SIZE 512

typedef void (*gps_rx_handler_t)(uint8_t *buf, uint16_t len);
typedef void (*gps_rx_cplt_callback_t)(uint8_t *buf, uint16_t len);

typedef struct gps *gps_t;
typedef struct gps_pvt_data *gps_pvt_data_t;
typedef struct gps_frame_header *gps_frame_header_t;


struct __attribute__((__packed__)) gps_frame_header
{
    uint8_t msg_class;
    uint8_t msg_id;
    uint16_t payload_len;
};

typedef enum {
    STATE_PREAMBLE,
    STATE_HEADER,
    STATE_PAYLOAD
} gps_unpack_state_t;

struct gps_pvt_data
{
    uint32_t itow;
    uint16_t year;
    uint8_t month, day, hour, min, sec;
    uint8_t flag11;
    uint32_t t_acc;
    int32_t nano;
    uint8_t fix_type;
    uint8_t flag21;
    uint8_t flag22;
    uint8_t num_sv;
    int32_t lon, lat, height, h_msl;

    uint32_t h_acc, v_acc;
    int32_t vel_n, vel_e, vel_d, g_speed;
    int32_t head_mot;
    uint32_t s_acc, head_acc;
    uint16_t p_dop;
    uint8_t flag78;
};

struct gps
{
    fifo_s_t rx_fifo;
    uint16_t payload_len;

    uint8_t rx_buffer[GPS_RX_BUF_SIZE];
    uint16_t rx_index;

    gps_rx_handler_t rx_handler;
    gps_rx_cplt_callback_t rx_cplt_callback;

    struct gps_pvt_data pvt_data;

    gps_unpack_state_t unpack_state;
    osMutexId_t pvt_mutex_id;
};

void gps_init(gps_t gps);
void gps_unpack_fifo_data(gps_t gps);
void gps_rx_data_handle(gps_t gps, uint8_t *data, uint16_t len);

void gps_get_pvt_data(gps_t gps, gps_pvt_data_t data);

#endif 