/*
 * test_results.h
 *
 *  Created on: 22 במאי 2026
 *      Author: david
 */

#ifndef INC_TEST_RESULTS_H_
#define INC_TEST_RESULTS_H_



extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart5;


//extern QueueHandle_t xQueue_results;


#define   RSLT_QUEUE_LENGTH  50

#define   TASK_RSLT_PRI           5

#define BIT_PATTERN_LENGTH 8

typedef struct __attribute__((packed))
{
    uint16_t test_id;
    uint8_t  peripheral;
    uint8_t  status;
    /*
        0 = FAIL
        1 = PASS
    */

} test_result_t;

#define   RSLT_QUEUE_SIZE      sizeof(test_result_t)
typedef enum
{
    TEST_FAIL = 0,
    TEST_PASS = 1
} test_status_t;



typedef enum
{
    PERIPH_TIMER = 1,
    PERIPH_UART  = 2,
    PERIPH_SPI   = 3,
    PERIPH_I2C   = 4,
    PERIPH_ADC   = 5

} peripheral_id_t;

extern struct udp_pcb *udp_pcb_handle;
extern ip_addr_t last_client_ip;
extern u16_t last_client_port;

uint8_t compute_crc8(const void *data, size_t len);
void xTask_results(void *argument);
void init_result (void);
#endif /* INC_TEST_RESULTS_H_ */
