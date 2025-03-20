#include "communicate.h"

#include "transceiver.h"
#include "usart.h"

#include "log.h"
#include "sys.h"

#include "protocol.h"
#include "sensor_task.h"

#include "log.h"

#include "plane_task.h"

#define TX_PERIOD 200

struct transceiver transceiver;

struct communicate_recv_cmd communicate_recv_cmd_table[COMM_RECV_TABLE_MAX_SIZE] = {0};

static int comm_recv_table_size;

uint32_t usart1_rx_callback(uint8_t *buf, uint16_t len)
{
    transceiver_rx_data_handle(&transceiver, buf, len);
    return 0;
}

void transceiver_cmd_callback(uint16_t cmd_id, uint8_t *data, uint16_t len)
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

// void communicate_transmit(uint8_t *data, uint8_t cmd_id, uint16_t len)
// {
//     transceiver_tx_transmit(&transceiver, data, cmd_id, len);
// }

osThreadId_t rx_task_id;
osThreadAttr_t rx_task_attr = {
    .name = "COMM_RX_TASK",
    .priority = osPriorityNormal,
    .stack_size = 1024,
};

osThreadId_t tx_task_id;
osThreadAttr_t tx_task_attr = {
    .name = "COMM_TX_TASK",
    .priority = osPriorityNormal,
    .stack_size = 1024,
};

__NO_RETURN void communicate_rx_task(void *argument)
{
    usart1_rx_callback_register(usart1_rx_callback);
    
    while (1)
    {
        transceiver_unpack_fifo_data(&transceiver);
        osDelay(1);
        // log_i("Fuck");
    }
}
__NO_RETURN void communicate_tx_task(void *argument)
{
    struct sensor_data_export tx_sensor_data;
    struct servo_fdbk servo_fdbk;
    while (1)
    {
        get_export_sensor_data(&tx_sensor_data, osWaitForever);
        get_export_servo_fdbk(&servo_fdbk);

        tx_sensor_data.aileron = servo_fdbk.aileron;
        tx_sensor_data.rudder_l = servo_fdbk.rudder_l;
        tx_sensor_data.rudder_r = servo_fdbk.rudder_r;
        tx_sensor_data.elevator = servo_fdbk.elevator;
        // todo: seperate sensor_data and servo_fdbk for clearity

        transceiver_tx_transmit(&transceiver, (uint8_t*)&tx_sensor_data, DATA_ID_SENSOR_EXPORT, sizeof(struct sensor_data_export));
        osDelay(TX_PERIOD);
    }
}

void communicate_task_init()
{
    transceiver_init(&transceiver, usart1_transmit, NULL, transceiver_cmd_callback);
    rx_task_id = osThreadNew(communicate_rx_task, NULL, &rx_task_attr);
    tx_task_id = osThreadNew(communicate_tx_task, NULL, &tx_task_attr);
}