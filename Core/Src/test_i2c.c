

#include "FreeRTOS.h"
#include "semphr.h"
#include <queue.h>
#include "error.h"
#include <stdbool.h>
#include "udp_server.h"
#include "test_spi.h"
#include "test_i2c.h"

QueueHandle_t   xQueue_i2c;

 void init_i2c_task(void){

	 // Create I2C task

	     if (xTaskCreate(xI2c_task, "Task I2C", configMINIMAL_STACK_SIZE, NULL, TASK_I2C_PRI, NULL) != pdPASS)
	     {
	         my_assert(false, "failed to create Task I2C");
	     }


	     // create  I2C queue

	         xQueue_i2c = xQueueCreate(I2C_QUEUE_LENGTH, I2C_QUEUE_SIZE);

	        if (xQueue_i2c == NULL)
	        {

	            my_assert(false, "failed to create I2c queue");
	        }

 }

 /*****************************************************************************************************/

void xI2c_task(void *argument)
{

    cmd_to_stm_t cmd;



    for (;;)
    {
        if (xQueueReceive(  xQueue_i2c, &cmd, portMAX_DELAY) == pdTRUE)
        {
            // כאן הבדיקה רצה
            // עד שהיא נגמרת, הפקודה הבאה ממתינה ב־queue
        }
    }
}
