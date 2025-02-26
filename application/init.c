#include "main.h"
#include "init.h"
// #include "cmsis_os.h"
#include "cmsis_os2.h"

#include "board.h"

#include "plane_task.h"
#include "communicate_task.h"

osThreadId_t plane_task_id;
osThreadAttr_t plane_task_attr = {
    .name = "PLANE_TASK",
    .priority = osPriorityNormal,
    .stack_size = 512,
};

osThreadId_t communicate_task_id;
osThreadAttr_t communicate_task_attr = {
    .name = "COMMUNICATE_TASK",
    .priority = osPriorityNormal,
    .stack_size = 512,
};

void hw_init()
{
}

void sys_init()
{
    board_config();
}

void task_init()
{
    // osThreadDef(PLANE_TASK, plane_task, osPriorityNormal, 0, 512);
    // plane_task_id = osThreadCreate(osThread(PLANE_TASK), NULL);

    communicate_task_id = osThreadNew(communicate_task, NULL, &communicate_task_attr);

    plane_task_id = osThreadNew(plane_task, NULL, &plane_task_attr);
}

void services_task(void const *argument)
{
    hw_init();
    sys_init();
    task_init();

    // print log

    for (;;)
    {
        osDelay(1);
    }
}