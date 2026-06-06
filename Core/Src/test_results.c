#include <FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <task.h>
#include <semphr.h>
#include <timers.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <queue.h>

#include "lwip/udp.h"
#include "lwip/ip_addr.h"

#include "main.h"
#include "error.h"
#include "test_results.h"
#include "udp_server.h"

extern SemaphoreHandle_t udp_tx_mutex;

QueueHandle_t xQueue_results = NULL;

void init_result(void)
{
    xQueue_results = xQueueCreate(RSLT_QUEUE_LENGTH, RSLT_QUEUE_SIZE);

    if (xQueue_results == NULL)
    {
        my_assert(false, "failed to create results queue");
    }

    if (xTaskCreate(xTask_results,
                    "Task Results",
                    configMINIMAL_STACK_SIZE,
                    NULL,
                    TASK_RSLT_PRI,
                    NULL) != pdPASS)
    {
        my_assert(false, "failed to create Task Results");
    }
}

void xTask_results(void *argument)
{
    test_result_t result;

    for (;;)
    {
        if (xQueueReceive(xQueue_results,
                          &result,
                          portMAX_DELAY) == pdTRUE)
        {
            printf("RESULT TASK: received from queue\r\n");

            result.packet_type = PACKET_TYPE_RESULT;
            result.test_id = htons(result.test_id);

            struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT,
                                        sizeof(test_result_t),
                                        PBUF_RAM);

            if (p == NULL)
            {
                printf("Result pbuf alloc failed\r\n");
                continue;
            }

            memcpy(p->payload,
                   &result,
                   sizeof(test_result_t));

            if (xSemaphoreTake(udp_tx_mutex,
                               pdMS_TO_TICKS(100)) == pdTRUE)
            {
                printf("RESULT send type=%u id=%u size=%u\r\n",
                       result.packet_type,
                       ntohs(result.test_id),
                       (unsigned int)sizeof(test_result_t));

                err_t err = udp_sendto(udp_pcb_handle,
                                       p,
                                       &last_client_ip,
                                       last_client_port);

                if (err == ERR_OK)
                {
                    printf("RESULT UDP send OK to %s:%u\r\n",
                           ipaddr_ntoa(&last_client_ip),
                           last_client_port);
                }
                else
                {
                    printf("RESULT send failed: %d\r\n", err);
                }

                xSemaphoreGive(udp_tx_mutex);
            }
            else
            {
                printf("UDP TX mutex timeout while sending RESULT\r\n");
            }

            pbuf_free(p);
        }
    }
}

/*******************************************************************************/
uint8_t compute_crc8(const void *data, size_t len)
{
    const uint8_t *p = (const uint8_t *)data;
    uint8_t crc = 0;

    for (size_t i = 0; i < len; i++)
    {
        crc ^= p[i];

        for (int j = 0; j < 8; j++)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }

    return crc;
}
