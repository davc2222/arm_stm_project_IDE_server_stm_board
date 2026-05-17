#include "udp_server.h"
#include "lwip/ip_addr.h"
#include "cmsis_os.h"
#include <string.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "semphr.h"


SemaphoreHandle_t udp_tx_mutex;

static struct udp_pcb *udp_pcb_handle;

static uint8_t udp_rx_buffer[UDP_RX_BUFFER_SIZE];
static uint16_t udp_rx_len = 0;

static volatile uint8_t udp_packet_ready = 0;

static ip_addr_t last_client_ip;
static u16_t last_client_port;


static void udp_receive_callback(void *arg,
                                 struct udp_pcb *upcb,
                                 struct pbuf *p,
                                 const ip_addr_t *addr,
                                 u16_t port)
{


	ip_addr_copy(last_client_ip, *addr);
	last_client_port = port;


    if (p == NULL)
        return;

    if (p->len <= UDP_RX_BUFFER_SIZE)
    {
        memcpy(udp_rx_buffer, p->payload, p->len);

        udp_rx_len = p->len;

        udp_packet_ready = 1;

        printf("UDP RX %d bytes\r\n", p->len);
    }

    pbuf_free(p);
}

/**************************************************************************************/

void UDP_Server_Init(void)
{
    udp_pcb_handle = udp_new();

    if (udp_pcb_handle == NULL)
    {
        printf("udp_new failed\r\n");
        return;
    }

    if (udp_bind(udp_pcb_handle,
                 IP_ADDR_ANY,
                 UDP_LISTEN_PORT) != ERR_OK)
    {
        printf("udp_bind failed\r\n");

        udp_remove(udp_pcb_handle);

        udp_pcb_handle = NULL;

        return;
    }

    udp_recv(udp_pcb_handle,
             udp_receive_callback,
             NULL);

    printf("UDP listening on port %d\r\n",
           UDP_LISTEN_PORT);
}

/*********************************************************************************/

void UDP_Server_Task(void)
{
    if (udp_packet_ready)
    {
        udp_packet_ready = 0;

        if (udp_rx_len == sizeof(cmd_to_stm_t))
        {
            cmd_to_stm_t *cmd = (cmd_to_stm_t *)udp_rx_buffer;

            printf("Command received\r\n");
            printf("test_id=%u\r\n", cmd->test_id);
            printf("peripheral=%u\r\n", cmd->tested_Peripheral);
            printf("iterations=%d\r\n", cmd->iterations_num);
            printf("pattern_len=%u\r\n", cmd->bit_pattern_length);
            printf("crc=%02X\r\n", cmd->cmd_crc);

            UDP_Send_Ack(cmd);
        }
    }

}

/********************************************************************************/
 void UDP_Send_Ack(cmd_to_stm_t *cmd)
{
    ack_to_client_t ack;

    ack.test_id = cmd->test_id;
    ack.status = 1;
    ack.peripheral = cmd->tested_Peripheral;

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, sizeof(ack), PBUF_RAM);

    if (p == NULL)
    {
        printf("pbuf_alloc failed\r\n");
        return;
    }

    memcpy(p->payload, &ack, sizeof(ack));

    if (xSemaphoreTake(udp_tx_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
    {
        udp_sendto(udp_pcb_handle, p, &last_client_ip, last_client_port);
        xSemaphoreGive(udp_tx_mutex);
    }

    pbuf_free(p);
}
