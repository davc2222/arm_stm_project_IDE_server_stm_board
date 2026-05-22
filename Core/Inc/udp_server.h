#ifndef UDP_SERVER_H
#define UDP_SERVER_H




#include "lwip/udp.h"
#include <stdint.h>

#define UDP_LISTEN_PORT 1234
//#define UDP_RX_BUFFER_SIZE 64

#define UDP_RX_BUFFER_SIZE sizeof(cmd_to_stm_t)
typedef struct __attribute__((packed))
{
    uint16_t test_id;
    uint8_t tested_Peripheral;
    int16_t iterations_num;
    uint8_t bit_pattern_length;
    char bit_pattern[8];
    uint8_t cmd_crc;
} cmd_to_stm_t;



typedef struct __attribute__((packed))
{
    uint16_t test_id;
    uint8_t status;      // 1 = received
    uint8_t peripheral;
} ack_to_client_t;





typedef struct __attribute__((packed))
{
    uint16_t test_id;
    uint8_t peripheral;
    uint8_t status;
    /*
        0 = FAIL
        1 = PASS
    */

} test_result_t;


typedef enum
{
    PERIPH_TIMER = 1,
    PERIPH_UART  = 2,
    PERIPH_SPI   = 3,
    PERIPH_I2C   = 4,
    PERIPH_ADC   = 5

} peripheral_id_t;
void UDP_Server_Init(void);
void UDP_Server_Task(void);
void UDP_Send_Ack(cmd_to_stm_t *cmd);

#endif
