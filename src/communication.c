#include <stdint.h>
#include "motor.h"
#include "stm32f4xx.h"
//#include "motor.c"
#define NSS 4
#define PA2 2
#define PA3 3
#define SCK 5
#define MOSI 6
#define MISO 6
//working on this working wiht merge and all

volatile char command_bytes[32];
volatile int command_buffer;
volatile uint8_t bytes_received=0;
volatile uint8_t imu_data[13];
volatile uint8_t spi_tx_buf[13] = {
    0x80 | 0x3B,  // read request, register 0x3B
    0x00, 0x00, 0x00, 0x00,   // dummy bytes
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00    // 12 dummy bytes
};

void DMA1_Stream5_Init(void) {
    RCC->AHB1ENR |= (1<<21);           
 // disable stream
    while (DMA1_Stream5->CR & (1<<0)); // wait until disabled

    DMA1->HIFCR |= (0x3F<<6);         // clear Stream 5 flags

    DMA1_Stream5->PAR  = (uint32_t)&USART2->DR;
    DMA1_Stream5->M0AR = (uint32_t)command_bytes;
    DMA1_Stream5->NDTR = 32;

    DMA1_Stream5->CR = 0;
    DMA1_Stream5->CR |= (4<<25);       // channel 4
    DMA1_Stream5->CR |= (1<<16);       // priority high
    DMA1_Stream5->CR |= (0<<6);        // peripheral to memory
    DMA1_Stream5->CR |= (1<<10);       // MINC
    DMA1_Stream5->CR |= (1<<4);        // TCIE

    NVIC_EnableIRQ(DMA1_Stream5_IRQn);

    DMA1_Stream5->CR |= (1<<0);        // enable
}


void init_USART(void){
    RCC->APB1ENR |= (1<<17);   
    RCC->AHB1ENR |= (1<<0);   
    GPIOA->MODER &= ~(3 << (PA2*2));
    GPIOA->MODER |=  (2 << (PA2*2));
    GPIOA->MODER &= ~(3 << (PA3*2));
    GPIOA->MODER |=  (2 << (PA3*2));
    GPIOA->AFR[0] |= (7 << (PA2*4));
    GPIOA->AFR[0] |= (7 << (PA3*4));

    USART2->BRR = (22<<4) | 13;
    USART2->CR1 |= (1<<3);           // TE
    USART2->CR1 |= (1<<2);          
    USART2->CR3 |= (1<<6);   
    NVIC_EnableIRQ(USART2_IRQn)  ;      // DMAR ← new line
    USART2->CR1 |= (1<<13); 
};


void restart_dma_rx(void) {
    DMA1_Stream5->CR &= ~(1<<0);     
    while (DMA1_Stream5->CR & (1<<0));
    DMA1->HIFCR |= (1<<11);       
    DMA1_Stream5->NDTR = 32;        
    DMA1_Stream5->M0AR = (uint32_t)command_bytes;
    DMA1_Stream5->CR |= (1<<0); 
}

void init_spi(){

}


void USART2_IRQHandler(void) {
    if (USART2->SR & (1<<4)) {              // IDLE flag
        volatile uint32_t dummy = USART2->DR; // clear flag
        (void)dummy;

        // stop DMA
        DMA1_Stream5->CR &= ~(1<<0);

        // calculate bytes received
        bytes_received = (uint8_t)(32U - DMA1_Stream5->NDTR);
        if (bytes_received >= sizeof(command_bytes)) {
            bytes_received = (uint8_t)(sizeof(command_bytes) - 1U);
        }
        command_bytes[bytes_received] = '\0';
        command_buffer = 1;
                    // tell main loop
    }
}


void send_response(){



}

void SPI1_Init(void) {
    // 1. Enable clocks
    RCC->APB2ENR |= (1<<12);        // SPI1 clock
    RCC->AHB1ENR |= (1<<0);         // GPIOA clock

    // 2. PA4 NSS → output
    GPIOA->MODER &= ~(3<<(NSS*2));
    GPIOA->MODER |=  (1<<(NSS*2));
    GPIOA->ODR   |=  (1<<NSS);      // NSS starts HIGH

    // 3. PA5 SCK, PA6 MISO, PA7 MOSI → alternate function AF5
    GPIOA->MODER &= ~(3<<(SCK*2));
    GPIOA->MODER |=  (2<<(SCK*2));
    GPIOA->MODER &= ~(3<<(MISO*2));
    GPIOA->MODER |=  (2<<(MISO*2));
    GPIOA->MODER &= ~(3<<(MOSI*2));
    GPIOA->MODER |=  (2<<(MOSI*2));

    GPIOA->AFR[0] |= (5<<(SCK*4));
    GPIOA->AFR[0] |= (5<<(MISO*4));
    GPIOA->AFR[0] |= (5<<(MOSI*4));

    // 4. Configure CR1
    SPI1->CR1 = 0;                  // clear first
    SPI1->CR1 |= (3<<3);            // BR = fPCLK/16
    SPI1->CR1 |= (1<<10);            // SSM
    SPI1->CR1 |= (1<<8);            // SSI
    SPI1->CR1 |= (1<<2);            // MSTR

    // 5. Enable DMA
    SPI1->CR2 |= (1<<0);            // RXDMAEN
    SPI1->CR2 |= (1<<1);            // TXDMAEN

    // 6. Enable SPI last
    SPI1->CR1 |= (1<<6);            // SPE
}

void DMA2_Stream0_Init(){
     RCC->AHB1ENR |=(1<<22);//     RCC     → AHB1ENR bit 22 (DMA2)
        DMA2_Stream0 ->PAR=&SPI1->DR ;                  // PAR     → &SPI1->DR
// M0AR    → imu_data[]
// NDTR    → 12 (6 axes × 2 bytes)
// CR      → channel 3, peripheral to memory, MINC, circular, TCIE
// NVIC    → DMA2_Stream0_IRQn
// EN      → last
}
