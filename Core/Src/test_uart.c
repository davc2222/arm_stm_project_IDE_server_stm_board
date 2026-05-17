
#include "FreeRTOS.h"
#include "semphr.h"
#include <queue.h>


QueueHandle_t uart_queue;


void UartTask(void *argument)
{
    cmd_to_stm_t cmd;

    for (;;)
    {
        if (xQueueReceive(uart_queue, &cmd, portMAX_DELAY) == pdTRUE)
        {
            // כאן הבדיקה רצה
            // עד שהיא נגמרת, הפקודה הבאה ממתינה ב־queue
        }
    }
}
