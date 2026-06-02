/*
 * test_tiner.h
 *
 *  Created on: 21 במאי 2026
 *      Author: david
 */

#ifndef INC_TEST_TIMER_H_
#define INC_TEST_TIMER_H_

#define   TMR_QUEUE_LENGTH  50
#define   TMR_QUEUE_SIZE      sizeof(cmd_to_stm_t)
#define   TASK_TMR_PRI           5

void xTmr_task(void *argument);

//extern QueueHandle_t   xQueue_tmr;
void init_tmr_task(void);

#endif /* INC_TEST_TIMER_H_ */
