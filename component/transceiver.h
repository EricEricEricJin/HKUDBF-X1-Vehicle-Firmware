#ifndef __TRANSCEIVER_H__
#define __TRANSCEIVER_H__

// #include "usart.h"
#include "drv_uart.h"
#include "main.h"

typedef void (*trans_tx_handler_t)(uint8_t *buf, uint16_t len);
typedef uint32_t (*trans_rx_handler_t)(uint8_t *buf, uint16_t len);
typedef void (*trans_rx_cplt_callback_t)(uint8_t *buf, uint16_t len);
typedef void (*trans_cmd_callback_t)(uint16_t cmd_id, uint8_t *pdata, uint16_t len);

typedef struct transceiver *transceiver_t;

#define TRANS_RX_BUF_SIZE 512
#define TRANS_RX_SOF 0xA5
#define TRANS_RX_HEADER_SIZE sizeof(struct trans_frame_header)

#define TRANS_TX_BUF_SIZE 512
#define TRANS_TX_SOF 0xA5
#define TRANS_TX_HEADER_SIZE sizeof(struct trans_frame_header)

typedef struct trans_frame_header *trans_frame_header_t;

struct __attribute__((__packed__)) trans_frame_header
{
    uint8_t sof;
    uint8_t cmd_id;
    uint16_t payload_len; // not including CRC16!
    uint8_t crc8;
};

typedef enum
{
    STATE_SOF,
    STATE_HEADER,
    STATE_PAYLOAD // payload includes data and its CRC16
} trans_unpack_state_t;

struct transceiver
{
    fifo_s_t rx_fifo;
    uint16_t payload_len;

    uint8_t rx_buffer[TRANS_RX_BUF_SIZE];
    uint16_t rx_index;

    uint8_t tx_buffer[TRANS_TX_BUF_SIZE];
    uint16_t tx_index;

    trans_tx_handler_t tx_handler;
    trans_rx_handler_t rx_handler;
    trans_rx_cplt_callback_t rx_cplt_callback;
    trans_cmd_callback_t cmd_callback;

    trans_unpack_state_t unpack_state;
};

void transceiver_init(transceiver_t trans,
                      trans_tx_handler_t tx_handler,
                      trans_rx_cplt_callback_t rx_cplt_callback,
                      trans_cmd_callback_t cmd_callback);

void transceiver_unpack_fifo_data(transceiver_t trans);

void transceiver_rx_data_handle(transceiver_t trans, uint8_t *data, uint16_t len);

void transceiver_tx_transmit(transceiver_t trans, const uint8_t *data, uint8_t cmd_id, uint16_t len);

#endif
