

#include "FreeRTOS.h"
#include "semphr.h"
#include <queue.h>
#include "error.h"
#include <stdbool.h>
#include "udp_server.h"
#include "test_spi.h"


QueueHandle_t   xQueue_spi;

 void init_spi_task(void){

	 // Create spi task

	     if (xTaskCreate(xSpi_task, "Task SPI", configMINIMAL_STACK_SIZE, NULL, TASK_SPI_PRI, NULL) != pdPASS)
	     {
	         my_assert(false, "failed to create Task SPI");
	     }


	     // create  uart queue

	         xQueue_spi = xQueueCreate(SPI_QUEUE_LENGTH, SPI_QUEUE_SIZE);

	        if (xQueue_spi == NULL)
	        {

	            my_assert(false, "failed to create spi queue");
	        }

 }

 /*****************************************************************************************************/

void xSpi_task(void *argument)
{
    cmd_to_stm_t cmd;



    for (;;)
    {
        if (xQueueReceive(  xQueue_spi, &cmd, portMAX_DELAY) == pdTRUE)
        {
            // כאן הבדיקה רצה
            // עד שהיא נגמרת, הפקודה הבאה ממתינה ב־queue
        }
    }
}
