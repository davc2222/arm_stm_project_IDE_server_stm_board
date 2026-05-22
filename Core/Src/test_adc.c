

#include "FreeRTOS.h"
#include "semphr.h"
#include <queue.h>
#include "error.h"
#include <stdbool.h>
#include "udp_server.h"
#include "test_spi.h"
#include "test_i2c.h"
#include "test_timer.h"
#include "test_adc.h"
QueueHandle_t   xQueue_adc;

 void init_adc_task(void){

	 // Create ADC task

	     if (xTaskCreate(xAdc_task, "Task ADCr", configMINIMAL_STACK_SIZE, NULL, TASK_ADC_PRI, NULL) != pdPASS)
	     {
	         my_assert(false, "failed to create Task ADC");
	     }


	     // create ADC queue

	         xQueue_adc = xQueueCreate(ADC_QUEUE_LENGTH, ADC_QUEUE_SIZE);

	        if (xQueue_adc == NULL)
	        {

	            my_assert(false, "failed to create ADC queue");
	        }

 }

 /*****************************************************************************************************/

void xAdc_task(void *argument)
{
    cmd_to_stm_t cmd;



    for (;;)
    {
        if (xQueueReceive(  xQueue_adc, &cmd, portMAX_DELAY) == pdTRUE)
        {
            // כאן הבדיקה רצה
            // עד שהיא נגמרת, הפקודה הבאה ממתינה ב־queue
        }
    }
}
