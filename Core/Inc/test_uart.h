/*
 * test_uart.h
 *
 *  Created on: 19 במאי 2026
 *      Author: david
 */

#ifndef INC_TEST_UART_H_
#define INC_TEST_UART_H_


#define   UART_QUEUE_LENGTH  50
#define   UART_QUEUE_SIZE      sizeof(cmd_to_stm_t)
#define   TASK_UART_PRI           5


void xUart_task(void *argument);

extern QueueHandle_t   xQueue_uart;

#endif /* INC_TEST_UART_H_ */
