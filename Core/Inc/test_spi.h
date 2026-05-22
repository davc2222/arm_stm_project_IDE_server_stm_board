/*
 * test_spi.h
 *
 *  Created on: 21 במאי 2026
 *      Author: david
 */

#ifndef INC_TEST_SPI_H_
#define INC_TEST_SPI_H_

#define   SPI_QUEUE_LENGTH  50
#define   SPI_QUEUE_SIZE      sizeof(cmd_to_stm_t)
#define   TASK_SPI_PRI           5


void xSpi_task(void *argument);

extern QueueHandle_t   xQueue_spi;


#endif /* INC_TEST_SPI_H_ */
