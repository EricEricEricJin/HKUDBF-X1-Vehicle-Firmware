#include "main.h"
#include "init.h"
// #include "cmsis_os.h"
#include "cmsis_os2.h"

#include "board.h"

#include "plane_task.h"
#include "communicate.h"
#include "sensor_task.h"

#include "sys.h"
#include "log.h"

osThreadId_t plane_task_id;
osThreadAttr_t plane_task_attr = {
    .name = "PLANE_TASK",
    .priority = osPriorityNormal,
    .stack_size = 1024,
};

osThreadId_t sensor_task_id;
osThreadAttr_t sensor_task_attr = {
    .name = "SENSOR_TASK",
    .priority = osPriorityNormal,
    .stack_size = 2048,
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
    communicate_task_init();
    
    plane_task_id = osThreadNew(plane_task, NULL, &plane_task_attr);
    sensor_task_id = osThreadNew(sensor_task, NULL, &sensor_task_attr);


    // sensor task incomplete
}

void services_task(void const *argument)
{
    hw_init();
    sys_init();
    task_init();

    // print log
    // xTaskCreate()

    for (;;)
    {
        osDelay(1);

    }
}