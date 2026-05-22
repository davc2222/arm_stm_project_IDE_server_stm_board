/*
 * test_adc.h
 *
 *  Created on: 22 במאי 2026
 *      Author: david
 */

#ifndef INC_TEST_ADC_H_
#define INC_TEST_ADC_H_

#define   ADC_QUEUE_LENGTH  50
#define   ADC_QUEUE_SIZE      sizeof(cmd_to_stm_t)
#define   TASK_ADC_PRI           5

void xAdc_task(void *argument);

extern QueueHandle_t   xQueue_adc;



#endif /* INC_TEST_ADC_H_ */
