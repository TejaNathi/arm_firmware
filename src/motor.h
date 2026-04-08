// motor.h
#ifndef MOTOR_H
#define MOTOR_H




#include <stdint.h>
#include "stm32f401xc.h"

typedef enum { MOVING, STOPPED } MotorState;

typedef struct {
    TIM_TypeDef*   timer;
    GPIO_TypeDef*  step_port;
    uint8_t        step_pin;
    GPIO_TypeDef*  dir_port;
    uint8_t        dir_pin;
    uint32_t       steps_remaining;
    uint32_t       steps_total;
    uint32_t       start_arr;
    uint32_t       target_arr;
    MotorState     state;
} Motor;

typedef struct {
 int32_t angle[6]; //this is per joint we have 6 joint
 uint32_t start_arr[6];
 uint32_t target_arr[6];
uint32_t time_ms;

} waypoint;

// function declaration
void MotorInit(Motor *m,
               TIM_TypeDef* timer,
               GPIO_TypeDef* step_port,
               uint8_t step_pin,
               GPIO_TypeDef* dir_port,
               uint8_t dir_pin);
void startmotors();
uint8_t queuepush(waypoint* wp);
uint8_t queuepop(waypoint* wp);

#endif
