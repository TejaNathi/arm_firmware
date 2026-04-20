#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdint.h>

#define UART_RX_BUF_SIZE 32U

extern volatile char command_bytes[UART_RX_BUF_SIZE];
extern volatile int command_buffer;
extern volatile uint8_t bytes_received;

void DMA1_Stream5_Init(void);
void init_USART(void);
void restart_dma_rx(void);
void init_spi(void);
void SPI1_Init(void);
void DMA2_Stream0_Init(void);
void send_response(void);

#endif
