#include <stdint.h>
#include "motor.h"


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
    MotorInit(&motor[0],TIM2,GPIOA,2,GPIOB,3);
    MotorInit(&motor[1],TIM3,GPIOA,3,GPIOB,4);
    MotorInit(&motor[2],TIM4,GPIOA,4,GPIOB,5);
    





    result = add(local_a, local_b);
    increment(&counter);
    counter += lookup[idx & 0x3U];

    while(1)
    {
        total += result;
        private_count++;
    }

    return 0;
}
