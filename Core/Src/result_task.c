#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <task.h>
#include <semphr.h>
#include <timers.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <queue.h>

extern SemaphoreHandle_t udp_tx_mutex;
QueueHandle_t xQueue_result;

init_result (void){



}
