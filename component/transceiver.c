/**
 * Modified based on RoboMaster RoboRTS-Firmware referee_system.c
 */

#include "transceiver.h"

#include "fifo.h"
#include "crc8.h"
#include "crc16.h"

#include "board.h"

void transceiver_init(transceiver_t trans,
                     trans_tx_handler_t tx_handler,
                     trans_rx_cplt_callback_t rx_cplt_callback,
                     trans_cmd_callback_t cmd_callback)
{
    // initialize fifo
    fifo_s_init(&trans->rx_fifo, trans->rx_buffer, TRANS_RX_BUF_SIZE);
    trans->payload_len = 0;
    trans->rx_index = 0;

    trans->tx_handler = tx_handler;
    trans->rx_cplt_callback = rx_cplt_callback;
    trans->cmd_callback = cmd_callback;

    trans->unpack_state = STATE_SOF;

    // SET_BOARD_LED_OFF();
}

void data_unpack(transceiver_t trans, trans_frame_header_t header_ptr, uint8_t* payload_ptr)
{
    trans->cmd_callback(header_ptr->cmd_id, payload_ptr, header_ptr->payload_len);
}

void transceiver_unpack_fifo_data(transceiver_t trans)
{
    fifo_s_t *p_fifo = &trans->rx_fifo;
    uint8_t byte;

    while (fifo_s_used(p_fifo))
    {
        // SET_BOARD_LED_ON();
        byte = fifo_s_get(p_fifo);
        
        switch (trans->unpack_state)
        {
        case STATE_SOF:
        {
            if (byte == TRANS_RX_SOF)
            {
                trans->unpack_state = STATE_HEADER;
                trans->rx_buffer[trans->rx_index++] = byte;
            }
            else
            {
                trans->rx_index = 0;
            }
        }
        break;

        case STATE_HEADER:
        {
            trans->rx_buffer[trans->rx_index++] = byte;
            if (trans->rx_index == TRANS_RX_HEADER_SIZE)
            {
                if (crc8(trans->rx_buffer, TRANS_RX_HEADER_SIZE - 1) == byte)
                {
                    trans->payload_len = ((trans_frame_header_t)trans->rx_buffer)->payload_len;
                    trans->unpack_state = STATE_PAYLOAD;
                }
                else
                {
                    trans->rx_index = 0;
                    trans->unpack_state = STATE_SOF;
                }
            }

        }
        break;

        case STATE_PAYLOAD:
        {
            trans->rx_buffer[trans->rx_index++] = byte;
                
            if (trans->rx_index == (TRANS_RX_HEADER_SIZE + trans->payload_len + 2))
            {
                uint16_t crc16_recv = *((uint16_t*)&(trans->rx_buffer[trans->rx_index - 2]));
                
                if (crc16(&trans->rx_buffer[TRANS_RX_HEADER_SIZE], trans->rx_index - 2 - TRANS_RX_HEADER_SIZE) == crc16_recv)
                {
                    data_unpack(trans, (trans_frame_header_t)trans->rx_buffer, &trans->rx_buffer[TRANS_RX_HEADER_SIZE]);
                }

                trans->unpack_state = STATE_SOF;
                trans->rx_index = 0;
            }
        }
        break;

        default:
        {
            trans->unpack_state = STATE_SOF;
            trans->rx_index = 0;
        }
        break;
        }
    }
}

void transceiver_rx_data_handle(transceiver_t trans, uint8_t *data, uint16_t len)
{
    fifo_s_puts(&trans->rx_fifo, (char *)data, len);

    if (trans->rx_cplt_callback != NULL)
    {
        trans->rx_cplt_callback(data, len);
    }
}

void transceiver_tx_transmit(transceiver_t trans, const uint8_t *data, uint8_t cmd_id, uint16_t len)
{
    // write header
    trans->tx_index = 0;
    trans_frame_header_t header = (trans_frame_header_t)trans->tx_buffer;
    header->sof = TRANS_TX_SOF;
    header->cmd_id = cmd_id;
    header->payload_len = len;
    header->crc8 = crc8((uint8_t*)header, TRANS_TX_HEADER_SIZE - 1);
    trans->tx_index += TRANS_TX_HEADER_SIZE;

    // copy data
    memcpy(&trans->tx_buffer[trans->tx_index], data, len);
    trans->tx_index += len;

    // write crc16
    uint16_t crc16_val = crc16(data, len);
    *((uint16_t*)&trans->tx_buffer[trans->tx_index]) = crc16_val;
    trans->tx_index += sizeof(uint16_t);

    // transmit
    if (trans->tx_handler != NULL)
    {
        trans->tx_handler(trans->tx_buffer, trans->tx_index);
    }
}