

#include "FreeRTOS.h"
#include "semphr.h"
#include <queue.h>
#include "error.h"
#include <stdbool.h>
#include "udp_server.h"
#include "test_spi.h"
#include "test_i2c.h"
#include "test_timer.h"

QueueHandle_t   xQueue_tmr;

 void init_tmr_task(void){

	 // Create TMR task

	     if (xTaskCreate(xTmr_task, "Task Timer", configMINIMAL_STACK_SIZE, NULL, TASK_TMR_PRI, NULL) != pdPASS)
	     {
	         my_assert(false, "failed to create Task Timer");
	     }


	     // create Timer queue

	         xQueue_tmr = xQueueCreate(TMR_QUEUE_LENGTH, TMR_QUEUE_SIZE);

	        if (xQueue_tmr == NULL)
	        {

	            my_assert(false, "failed to create timer queue");
	        }

 }

 /*****************************************************************************************************/

void xTmr_task(void *argument)
{


    cmd_to_stm_t cmd;



    for (;;)
    {
        if (xQueueReceive(  xQueue_tmr, &cmd, portMAX_DELAY) == pdTRUE)
        {
            // כאן הבדיקה רצה
            // עד שהיא נגמרת, הפקודה הבאה ממתינה ב־queue
        }
    }
}
