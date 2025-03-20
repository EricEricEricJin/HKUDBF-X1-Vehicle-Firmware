#ifndef __PLANE_TASK_H__
#define __PLANE_TASK_H__

__NO_RETURN void plane_task(void* args);

#include "protocol.h"

void get_export_servo_fdbk(servo_fdbk_t data);

#endif 