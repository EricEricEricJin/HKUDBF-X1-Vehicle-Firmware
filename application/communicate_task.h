#ifndef __COMMUNICATE_TASK_H__
#define __COMMUNICATE_TASK_H__

#include "stdint.h"

#define COMM_RECV_TABLE_MAX_SIZE 10

typedef int32_t (*recv_handle_callback_t)(uint8_t *buff, uint16_t len);

typedef struct communicate_recv_cmd *communicate_recv_cmd_t;
struct communicate_recv_cmd
{
    uint16_t cmd;
    recv_handle_callback_t recv_callback;
};



void communicate_task(void *argument);

void communicate_register_recv_cmd(communicate_recv_cmd_t table, int n);

void communicate_transmit(uint8_t *data, uint8_t cmd_id, uint16_t len);

#endif 
