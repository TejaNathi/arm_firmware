#include <stdlib.h>
#include <stdio.h>

#include "motor.h"

#define QUEUE_SIZE 16
#define STEPS_PER_REV 3200U
#define START_ARR 4999U
#define NUMBER_JOINTS 6U

static waypoint queue[QUEUE_SIZE];
static volatile uint8_t queue_head  = 0;
static volatile uint8_t queue_tail  = 0;
static volatile uint8_t queue_count = 0;

volatile int joints_done = 0;

uint8_t queuepush(const waypoint* wp)
{
    if ((wp == NULL) || (queue_count >= QUEUE_SIZE)) {
        return 0;
    }

    queue[queue_tail] = *wp;
    queue_tail = (uint8_t)((queue_tail + 1U) & (QUEUE_SIZE - 1U));
    queue_count++;
    return 1;
}

uint8_t queuepop(waypoint* wp)
{
    if ((wp == NULL) || (queue_count == 0U)) {
        return 0;
    }

    *wp = queue[queue_head];
    queue_head = (uint8_t)((queue_head + 1U) & (QUEUE_SIZE - 1U));
    queue_count--;
    return 1;
}

void executewaypoint(Motor motors[], waypoint* wp)
{
    if ((motors == NULL) || (wp == NULL)) {
        return;
    }

    joints_done = 0;
    for (uint32_t i = 0; i < NUMBER_JOINTS; i++) {
        movemotor(&motors[i], wp->angle[i], wp->start_arr[i], wp->target_arr[i]);
    }
}

void MotorInit(Motor* m,
               TIM_TypeDef* timer,
               GPIO_TypeDef* step_port,
               uint8_t step_pin,
               GPIO_TypeDef* dir_port,
               uint8_t dir_pin)
{
    if ((m == NULL) || (timer == NULL) || (step_port == NULL) || (dir_port == NULL)) {
        return;
    }

    m->timer = timer;
    m->step_port = step_port;
    m->step_pin = step_pin;
    m->dir_pin = dir_pin;
    m->dir_port = dir_port;
    m->state = STOPPED;
    m->steps_remaining = 0;
    m->steps_total = 0;
    m->start_arr = START_ARR;
    m->target_arr = START_ARR;

    step_port->MODER &= ~(3U << (step_pin * 2U));
    step_port->MODER |= (1U << (step_pin * 2U));
    step_port->ODR &= ~(1U << step_pin);

    timer->PSC = 83U;
    timer->ARR = START_ARR;
    timer->EGR |= (1U << 0U);
    timer->SR &= ~(1U << 0U);
    timer->DIER |= (1U << 0U);
}

void movemotor(Motor* m, int32_t angle, uint32_t start_arr, uint32_t target_arr)
{
    if (m == NULL) {
        return;
    }

    if (angle > 0) {
        m->dir_port->ODR |= (1U << m->dir_pin);
    } else {
        m->dir_port->ODR &= ~(1U << m->dir_pin);
    }

    uint32_t steps = (uint32_t)((((float)abs(angle)) / 360.0f) * (float)STEPS_PER_REV);
    if (steps == 0U) {
        return;
    }

    m->steps_remaining = steps;
    m->steps_total = steps;
    m->start_arr = start_arr;
    m->target_arr = target_arr;
    m->state = MOVING;

    m->timer->ARR = start_arr;
    m->timer->CNT = 0;
    m->timer->EGR |= (1U << 0U);
    m->timer->SR &= ~(1U << 0U);
}

void startmotors(void)
{
    // placeholder for synchronized start logic
}

uint8_t parsecommand(char* cmd_buffer, waypoint* wp)
{
    if ((cmd_buffer == NULL) || (wp == NULL)) {
        return 0;
    }

    int j[6] = {0};
    int sa[6] = {4999, 4999, 4999, 4999, 4999, 4999};
    int ta[6] = {4999, 4999, 4999, 4999, 4999, 4999};
    int total_time = 0;

    int parsed = sscanf(cmd_buffer,
                        "MOVE "
                        "J1 %d SA1 %d TA1 %d "
                        "J2 %d SA2 %d TA2 %d "
                        "J3 %d SA3 %d TA3 %d "
                        "J4 %d SA4 %d TA4 %d "
                        "J5 %d SA5 %d TA5 %d "
                        "J6 %d SA6 %d TA6 %d "
                        "T%d",
                        &j[0], &sa[0], &ta[0],
                        &j[1], &sa[1], &ta[1],
                        &j[2], &sa[2], &ta[2],
                        &j[3], &sa[3], &ta[3],
                        &j[4], &sa[4], &ta[4],
                        &j[5], &sa[5], &ta[5],
                        &total_time);

    if (parsed != 19) {
        return 0;
    }

    for (uint32_t i = 0; i < NUMBER_JOINTS; i++) {
        wp->angle[i] = j[i];
        wp->start_arr[i] = (uint32_t)sa[i];
        wp->target_arr[i] = (uint32_t)ta[i];
    }
    wp->time_ms = (uint32_t)total_time;

    return 1;
}
