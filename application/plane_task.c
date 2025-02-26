#include "main.h"

#include "board.h"
#include "cmsis_os.h"

#include "communicate_task.h"

#include "protocol.h"

int32_t plane_set_att(uint8_t *data, uint16_t len)
{
    // try loopback
    communicate_transmit(data, CMD_SET_ATT, len);
}

struct communicate_recv_cmd plane_recv_cmd_table[] = {
    {CMD_SET_ATT, plane_set_att},
};

__NO_RETURN void plane_task(void *args)
{

    communicate_register_recv_cmd(plane_recv_cmd_table, sizeof(plane_recv_cmd_table) / sizeof(plane_recv_cmd_table));

    while (1)
    {
        osDelay(5);
        // SET_BOARD_LED_ON();
        // osDelay(500);
        // SET_BOARD_LED_OFF();
        // osDelay(500);
    }
}
