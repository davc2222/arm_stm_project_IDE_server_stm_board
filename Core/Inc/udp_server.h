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


void UDP_Server_Init(void);
void UDP_Server_Task(void);
void UDP_Send_Ack(cmd_to_stm_t *cmd);

#endif
