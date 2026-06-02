#include "udp_server.h"

#include "lwip/ip_addr.h"
#include "lwip/udp.h"
#include "lwip/def.h"
#include "lwip/pbuf.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"

#include "test_uart.h"
#include "test_spi.h"
#include "test_i2c.h"
#include "test_timer.h"
#include "test_adc.h"
#include "test_results.h"

#include <string.h>
#include <stdio.h>

/* UDP TX mutex */
SemaphoreHandle_t udp_tx_mutex;

/* UDP PCB */
struct udp_pcb *udp_pcb_handle;

extern QueueHandle_t   xQueue_uart;
extern QueueHandle_t   xQueue_spi;
extern QueueHandle_t   xQueue_tmr;
extern QueueHandle_t   xQueue_i2c;
extern QueueHandle_t   xQueue_adc;
/* RX buffer */
static uint8_t udp_rx_buffer[UDP_RX_BUFFER_SIZE];
static uint16_t udp_rx_len = 0;
 volatile uint8_t udp_packet_ready = 0;

/* Last client address */
 ip_addr_t last_client_ip;
 u16_t last_client_port;

/* Private functions */
static void udp_receive_callback(void *arg, struct udp_pcb *upcb,
		struct pbuf *p, const ip_addr_t *addr, u16_t port);

/**************************************************************************************/

static void udp_receive_callback(void *arg, struct udp_pcb *upcb,
		struct pbuf *p, const ip_addr_t *addr, u16_t port) {
	if (p == NULL) {
		return;
	}

	printf("RX from %s:%u\r\n",
	       ipaddr_ntoa(addr),
	       port);
	ip_addr_copy(last_client_ip, *addr);
	last_client_port = port;

	if (p->len <= UDP_RX_BUFFER_SIZE) {
		memcpy(udp_rx_buffer, p->payload, p->len);

		udp_rx_len = p->len;
		udp_packet_ready = 1;
	} else {
		printf("UDP packet too large: %u\r\n", p->len);
	}

	pbuf_free(p);
}

/**************************************************************************************/

void UDP_Server_Init(void) {
	udp_tx_mutex = xSemaphoreCreateMutex();

	if (udp_tx_mutex == NULL) {
		printf("udp_tx_mutex create failed\r\n");
		return;
	}

	udp_pcb_handle = udp_new();

	if (udp_pcb_handle == NULL) {
		printf("udp_new failed\r\n");
		return;
	}

	if (udp_bind(udp_pcb_handle, IP_ADDR_ANY, UDP_LISTEN_PORT) != ERR_OK) {
		printf("udp_bind failed\r\n");

		udp_remove(udp_pcb_handle);
		udp_pcb_handle = NULL;

		return;
	}

	udp_recv(udp_pcb_handle, udp_receive_callback, NULL);

	printf("UDP listening on port %d\r\n", UDP_LISTEN_PORT);
}

/**************************************************************************************/

void UDP_Server_Task(void)
{
    if (udp_packet_ready)
    {
        udp_packet_ready = 0;

        if (udp_rx_len == sizeof(cmd_to_stm_t))
        {
            cmd_to_stm_t cmd;

            /* Copy from shared RX buffer to local struct */
            memcpy(&cmd, udp_rx_buffer, sizeof(cmd));

            /* Convert network order to host order */
            cmd.test_id = ntohs(cmd.test_id);
            cmd.iterations_num = ntohs(cmd.iterations_num);

            printf("Command received\r\n");
            printf("test_id=%u\r\n", ntohs(cmd.test_id));
            printf("peripheral=%u\r\n", cmd.tested_Peripheral);
            printf("iterations=%d\r\n", cmd.iterations_num);
            printf("pattern_len=%u\r\n", cmd.bit_pattern_length);
            printf("crc=%02X\r\n", cmd.cmd_crc);

            UDP_Send_Ack(&cmd);

            printf("Before Dispatch\r\n");
            fflush(stdout);

          Dispatch_Peripheral_Command(&cmd);

            printf("After Dispatch\r\n");
            fflush(stdout);
        }
        else
        {
            printf("Bad UDP packet size: %u\r\n", udp_rx_len);
        }
    }
}
/**************************************************************************************/

void UDP_Send_Ack(cmd_to_stm_t *cmd)
{
    ack_to_client_t ack;

    /* Convert back to network order before sending */
        ack.test_id = htons(cmd->test_id);
  //  ack.test_id = cmd->test_id;
    ack.status = 1;
    ack.peripheral = cmd->tested_Peripheral;

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT,
                                sizeof(ack),
                                PBUF_RAM);

    if (p == NULL)
    {
        printf("pbuf_alloc failed\r\n");
        return ;
    }

    memcpy(p->payload, &ack, sizeof(ack));

    if (xSemaphoreTake(udp_tx_mutex,
                       pdMS_TO_TICKS(100)) == pdTRUE)
    {
        printf("ACK to %s:%u\r\n",
               ipaddr_ntoa(&last_client_ip),
               last_client_port);

        printf("ACK id=%u to %s:%u\r\n",
               cmd->test_id,
               ipaddr_ntoa(&last_client_ip),
               last_client_port);

        err_t err = udp_sendto(udp_pcb_handle,
                               p,
                               &last_client_ip,
                               last_client_port);

        xSemaphoreGive(udp_tx_mutex);

        if (err != ERR_OK)
        {
            printf("ACK send failed: %d\r\n", err);
        }
    }
    else
    {
        printf("UDP TX mutex timeout\r\n");
    }

    pbuf_free(p);
}
/**************************************************************************************/

void Dispatch_Peripheral_Command(cmd_to_stm_t *cmd) {

	 BaseType_t ok = pdFAIL;

	switch (cmd->tested_Peripheral) {

	case PERIPH_TIMER:
		xQueueSend(xQueue_tmr, cmd, 0);
		break;

	case PERIPH_UART:
		   if (xQueue_uart != NULL)
		      ok = xQueueSend(xQueue_uart, cmd, 0);
		break;

	case PERIPH_SPI:
//		xQueueSend(xQueue_spi, cmd, 0);
		break;

	case PERIPH_I2C:
		xQueueSend(xQueue_i2c, cmd, 0);
		break;

	case PERIPH_ADC:
		xQueueSend(xQueue_adc, cmd, 0);
		break;

	default:
		printf("Unknown peripheral: %u\r\n", cmd->tested_Peripheral);
		break;
	}
	 printf("Queue send result=%ld\r\n", (long)ok);

}
