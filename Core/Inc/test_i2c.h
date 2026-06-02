/*
 * i2c.h
 *
 *  Created on: 21 במאי 2026
 *      Author: david
 */

#ifndef INC_TEST_I2C_H_
#define INC_TEST_I2C_H_


#define   I2C_QUEUE_LENGTH  50
#define   I2C_QUEUE_SIZE      sizeof(cmd_to_stm_t)
#define   TASK_I2C_PRI           5


void xI2c_task(void *argument);

//extern QueueHandle_t   xQueue_i2c;
void init_i2c_task(void);

#endif /* INC_TEST_I2C_H_ */
