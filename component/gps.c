#include "gps.h"

#define PREAMBLE0 0xB5
#define PREAMBLE1 0x62

#define PREAMBLE_LEN 2
#define CHECKSUM_LEN 2

#define PVT_MSG_CLASS 0x01
#define PVT_MSG_ID 0x07

#define HEADER_SIZE sizeof(struct gps_frame_header)

const osMutexAttr_t gps_pvt_mutex_attr = {
    .name = "GPS_PVT_MUTEX",
};

void gps_init(gps_t gps)
{
    fifo_s_init(&gps->rx_fifo, gps->rx_buffer, GPS_RX_BUF_SIZE);
    gps->payload_len = 0;
    gps->rx_index = 0;
    gps->pvt_mutex_id = osMutexNew(&gps_pvt_mutex_attr);
    gps->unpack_state = STATE_PREAMBLE;
}

void gps_unpack_fifo_data(gps_t gps)
{
    fifo_s_t* p_fifo = &gps->rx_fifo;
    uint8_t* rx_buffer = gps->rx_buffer;
    uint8_t byte;
    
    while (fifo_s_used(p_fifo))
    {
        byte = fifo_s_get(p_fifo);
        switch (gps->unpack_state)
        {
        
        case STATE_PREAMBLE:
        {
            rx_buffer[gps->rx_index++] = byte;
            if (gps->rx_index == PREAMBLE_LEN && rx_buffer[0] == PREAMBLE0 && rx_buffer[1] == PREAMBLE1)
                gps->unpack_state = STATE_HEADER;
            else
                gps->rx_index = 0;
        }
        break;

        case STATE_HEADER:
        {
            rx_buffer[gps->rx_index++] = byte;
            if (gps->rx_index == HEADER_SIZE)
            {
                gps_frame_header_t header = (gps_frame_header_t)(&rx_buffer[PREAMBLE_LEN]);
                
                if (header->msg_class == PVT_MSG_CLASS && header->msg_id == PVT_MSG_ID)
                {
                    gps->payload_len = header->payload_len;
                    gps->unpack_state = STATE_PAYLOAD;
                }
                else
                {
                    gps->rx_index = 0;
                    gps->unpack_state = STATE_PREAMBLE;
                }
            }
        }
        break;

        case STATE_PAYLOAD:
        {
            rx_buffer[gps->rx_index++] = byte;
            if (gps->rx_index == (HEADER_SIZE + gps->payload_len + CHECKSUM_LEN))
            {
                uint8_t ck_a, ck_b;
                ck_a = rx_buffer[gps->rx_index - 2];
                ck_b = rx_buffer[gps->rx_index - 1];

                uint8_t calc_a = 0;
                uint8_t calc_b = 0;
                for (int i = PREAMBLE_LEN; i < PREAMBLE_LEN + gps->payload_len + HEADER_SIZE; i++)
                {
                    calc_a = (calc_a + rx_buffer[i]) & 0xff;
                    calc_b = (calc_b + calc_a) & 0xff;
                }

                if (calc_a == ck_a && calc_b == ck_b)
                {
                    osMutexAcquire(gps->pvt_mutex_id, osWaitForever);
                    memcpy(&gps->pvt_data, &rx_buffer[PREAMBLE_LEN + HEADER_SIZE], gps->payload_len);
                    osMutexRelease(gps->pvt_mutex_id);
                }
                
                gps->rx_index = 0;
                gps->unpack_state = STATE_PREAMBLE;
            
            }
        }
        break;

        default:
        {
            gps->unpack_state = STATE_PREAMBLE;
            gps->rx_index = 0;
        }
        break;
        }
    }
    
}

void gps_rx_data_handle(gps_t gps, uint8_t *data, uint16_t len)
{
    fifo_s_puts(&gps->rx_fifo, (char *)data, len);
}

void gps_get_pvt_data(gps_t gps, gps_pvt_data_t data)
{
    osMutexAcquire(gps->pvt_mutex_id, osWaitForever);
    memcpy(data, &gps->pvt_data, sizeof(struct gps_pvt_data));
    osMutexRelease(gps->pvt_mutex_id);
}
