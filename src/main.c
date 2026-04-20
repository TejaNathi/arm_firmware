#include <stdint.h>
#include "motor.h"
#include "communication.h"

volatile uint32_t counter = 10;
volatile uint32_t total;
static volatile uint32_t private_count = 5;
const uint32_t lookup[4] = {1, 2, 3, 4};
volatile uint32_t idx = 0;

static void increment(volatile uint32_t *val)
{
    (*val)++;
}

uint32_t add(uint32_t a, uint32_t b)
{
    return a + b;
}

int main(void)
{
    uint32_t local_a = 3;
    uint32_t local_b = 4;
    uint32_t result;
    Motor motor[6] = {0};

    MotorInit(&motor[0], TIM2, GPIOA, 2, GPIOB, 3);
    MotorInit(&motor[1], TIM3, GPIOA, 3, GPIOB, 4);
    MotorInit(&motor[2], TIM4, GPIOA, 4, GPIOB, 5);

    init_USART();
    DMA1_Stream5_Init();

    result = add(local_a, local_b);
    increment(&counter);
    counter += lookup[idx & 0x3U];

    while (1) {
        if (command_buffer) {
            waypoint wp = {0};
            waypoint exec_wp = {0};

            if (parsecommand((char *)command_bytes, &wp)) {
                (void)queuepush(&wp);
                if (queuepop(&exec_wp)) {
                    executewaypoint(motor, &exec_wp);
                }
            }

            command_buffer = 0;
            restart_dma_rx();
        }

        total += result;
        private_count++;
    }

    return 0;
}
