
#include "FreeRTOS.h"
#include "semphr.h"
#include <queue.h>
#include "error.h"
#include <stdbool.h>
#include "udp_server.h"
#include "test_uart.h"
#include "test_results.h"
#include <string.h>

SemaphoreHandle_t uart2_tx_done_sem;
SemaphoreHandle_t uart5_rx_done_sem;

extern QueueHandle_t xQueue_results;

QueueHandle_t   xQueue_uart;

void init_uart_task(void)
{
    xQueue_uart = xQueueCreate(UART_QUEUE_LENGTH, UART_QUEUE_SIZE);

    if (xQueue_uart == NULL)
    {
        my_assert(false, "failed to create uart queue");
    }

    uart2_tx_done_sem = xSemaphoreCreateBinary();
    uart5_rx_done_sem = xSemaphoreCreateBinary();

    if (uart2_tx_done_sem == NULL)
    {
        my_assert(false, "failed to create uart2_tx_done_sem");
    }

    if (uart5_rx_done_sem == NULL)
    {
        my_assert(false, "failed to create uart5_rx_done_sem");
    }

    if (xTaskCreate(xUart_task,
                    "Task UART",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    TASK_UART_PRI,
                    NULL) != pdPASS)
    {
        my_assert(false, "failed to create Task UART");
    }
}
 /*****************************************************************************************************/

void xUart_task(void *argument)
{


	    cmd_to_stm_t cmd;
	    test_result_t result;
	    uint8_t uart5_rx_buf[BIT_PATTERN_LENGTH];
	    printf("init_uart_task called\r\n");
	    for (;;)
	    {

	    	 // printf("task uart running.....\r\n");
	        if (xQueueReceive(xQueue_uart, &cmd, 100) == pdPASS)
	        {
	            memset(&result, 0, sizeof(result));
	            memset(uart5_rx_buf, 0, sizeof(uart5_rx_buf));

	            result.test_id = cmd.test_id;
	            result.peripheral = PERIPH_UART;
	            result.status = TEST_FAIL;

	            if (cmd.bit_pattern_length == 0 ||
	                cmd.bit_pattern_length > BIT_PATTERN_LENGTH)
	            {
	            	printf("UART: send result id=%u status=%u\r\n",
	            	       result.test_id,
	            	       result.status);

	            	if (xQueueSend(xQueue_results, &result, 0) != pdTRUE)
	            	{
	            	    printf("UART: xQueueSend result FAILED\r\n");
	            	}
	                continue;
	            }

	            if (HAL_UART_Receive_DMA(&huart5,
	                                     uart5_rx_buf,
	                                     cmd.bit_pattern_length) != HAL_OK)
	            {
	            	printf("UART: send result id=%u status=%u\r\n",
	            	       result.test_id,
	            	       result.status);

	            	if (xQueueSend(xQueue_results, &result, 0) != pdTRUE)
	            	{
	            	    printf("UART: xQueueSend result FAILED\r\n");
	            	}
	                continue;
	            }

	            if (HAL_UART_Transmit_DMA(&huart2,
	                                      (uint8_t *)cmd.bit_pattern,
	                                      cmd.bit_pattern_length) != HAL_OK)
	            {
	                HAL_UART_AbortReceive(&huart5);
	                printf("UART: send result id=%u status=%u\r\n",
	                       result.test_id,
	                       result.status);

	                if (xQueueSend(xQueue_results, &result, 0) != pdTRUE)
	                {
	                    printf("UART: xQueueSend result FAILED\r\n");
	                }
	                continue;
	            }

	            if (xSemaphoreTake(uart2_tx_done_sem,
	                               pdMS_TO_TICKS(1000)) != pdTRUE)
	            {
	                HAL_UART_AbortTransmit(&huart2);
	                HAL_UART_AbortReceive(&huart5);
	                printf("UART: send result id=%u status=%u\r\n",
	                       result.test_id,
	                       result.status);

	                if (xQueueSend(xQueue_results, &result, 0) != pdTRUE)
	                {
	                    printf("UART: xQueueSend result FAILED\r\n");
	                }
	                continue;
	            }

	            if (xSemaphoreTake(uart5_rx_done_sem,
	                               pdMS_TO_TICKS(1000)) != pdTRUE)
	            {
	                HAL_UART_AbortReceive(&huart5);
	                xQueueSend(xQueue_results, &result, 0);
	                continue;
	            }

	            if (memcmp(cmd.bit_pattern,
	                       uart5_rx_buf,
	                       cmd.bit_pattern_length) == 0)
	            {
	                result.status = TEST_PASS;
	            }

	            printf("UART: send result id=%u status=%u\r\n",
	                   result.test_id,
	                   result.status);

	            if (xQueueSend(xQueue_results, &result, 0) != pdTRUE)
	            {
	                printf("UART: xQueueSend result FAILED\r\n");
	            }
	        }
	    }
}





void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (huart->Instance == USART2)
    {
        printf("TX CALLBACK\r\n");

        if (uart2_tx_done_sem != NULL)
        {
            xSemaphoreGiveFromISR(uart2_tx_done_sem,
                                  &xHigherPriorityTaskWoken);

            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (huart->Instance == UART5)
    {
        printf("RX CALLBACK\r\n");

        if (uart5_rx_done_sem != NULL)
        {
            xSemaphoreGiveFromISR(uart5_rx_done_sem,
                                  &xHigherPriorityTaskWoken);

            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}
