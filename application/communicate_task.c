#include "communicate_task.h"

#include "transceiver.h"
#include "usart.h"

struct transceiver transceiver;

struct communicate_recv_cmd communicate_recv_cmd_table[COMM_RECV_TABLE_MAX_SIZE] = {0};

static int comm_recv_table_size;

uint32_t usart1_rx_callback(uint8_t *buf, uint16_t len)
{
    transceiver_rx_data_handle(&transceiver, buf, len);
    return 0;
}

void transceiver_cmd_callback(uint16_t cmd_id, uint8_t* data, uint16_t len)
{
    for (int i = 0; i < comm_recv_table_size; i++)
    {
        if (communicate_recv_cmd_table[i].cmd == cmd_id)
        {
            communicate_recv_cmd_table[i].recv_callback(data, len);
            break;
        }
    }
}

void communicate_register_recv_cmd(communicate_recv_cmd_t table, int n)
{
    memcpy(communicate_recv_cmd_table, table, n * sizeof(struct communicate_recv_cmd));
    comm_recv_table_size = n;
}

void communicate_transmit(uint8_t *data, uint8_t cmd_id, uint16_t len)
{
    transceiver_tx_transmit(&transceiver, data, cmd_id, len);
}

__NO_RETURN void communicate_task(void *argument)
{
    transceiver_init(&transceiver, usart1_transmit, NULL, transceiver_cmd_callback);
    usart1_rx_callback_register(usart1_rx_callback);

    while (1)
    {
        transceiver_unpack_fifo_data(&transceiver);
        osDelay(1);
    }
}


