
#include "FreeRTOS.h"
#include "semphr.h"
#include <queue.h>
#include "error.h"
#include <stdbool.h>
#include "udp_server.h"
#include "test_uart.h"


QueueHandle_t   xQueue_uart;

 void init_uart_task(void){

	 // Create uart task

	     if (xTaskCreate(xUart_task, "Task UART", configMINIMAL_STACK_SIZE, NULL, TASK_UART_PRI, NULL) != pdPASS)
	     {
	         my_assert(false, "failed to create Task UART");
	     }


	     // create  uart queue

	        xQueue_uart = xQueueCreate(UART_QUEUE_LENGTH, UART_QUEUE_SIZE);

	        if (xQueue_uart == NULL)
	        {

	            my_assert(false, "failed to create uart queue");
	        }

 }

 /*****************************************************************************************************/

void xUart_task(void *argument)
{
    cmd_to_stm_t cmd;



    for (;;)
    {
        if (xQueueReceive(  xQueue_uart, &cmd, portMAX_DELAY) == pdTRUE)
        {
            // כאן הבדיקה רצה
            // עד שהיא נגמרת, הפקודה הבאה ממתינה ב־queue
        }
    }
}
