#include <stdio.h>
#include "motor.h"
#include <stm32f401xc.h>
#include "communication.h"
#define QUEUE_SIZE 16
#define STEPS_PER_REV  3200
#define START_ARR      4999
#define NUMBER_JOINTs 6


//waypoint buffer i need to add
waypoint queue[QUEUE_SIZE];
volatile uint8_t queue_head     = 0;
volatile uint8_t queue_tail     = 0;
volatile uint8_t queue_count    = 0;
volatile int joints_done   = 0;


uint8_t queuepush(waypoint* wp){
// so here wp enter here we have to push as head then the command should execute one after another(this can be done in quepop)
    if (queue_count >= QUEUE_SIZE) return 0;
    queue[queue_tail] = *wp; // we are assigning the object of wp to the quetail;

    queue_tail = (queue_tail + 1) & (QUEUE_SIZE - 1);

    queue_count++;
    return 1;
}
uint8_t queuepop(waypoint* wp){
// so here wp enter here we have to pop as tail then the command should execute one after another(this can be done in quepop)
if (queue_count == 0) return 0;
 *wp = queue[queue_head];

queue_head = (queue_head + 1) & (QUEUE_SIZE - 1);

queue_count--;

return 1;

}
//in exicutewaaypoint only we point to the &wp and each wp mtor will move 

void exicutewaypoint(waypoint* wp){
    joints_done = 0;
    if(int i=0;i<NUMBER_JOINTs,i++){
        movemotor(M)
    }


}







// ->so here we have create a timer struct for motors, then motors are 6 
// //1. Motor driver    → 6 motors, generic struct
// 2. Timer PWM       → step pulses for each motor
// 3. UART            → receive commands from Pi
// 4. Command parser  → extract joint angles
// 5. Motion control  → convert angles to steps, move motors
// HARDWARE NEED          SOLUTION
// ─────────────────────────────────────────────────────
// STEP pulse             Timer overflow interrupt
// Precise frequency      PSC=83 → 1MHz counter
// Speed control          ARR value
// Acceleration           ARR changes per step
// Position               steps_remaining countdown
// Direction              DIR GPIO pin
// 6 motors               Motor struct + generic functions
// Angle to steps         (angle/360) × STEPS_PER_REV
// Done detection         joints_done bitmask
// Pi communication       UART ISR + command parser





void MotorInit(Motor *m, TIM_TypeDef*   timer, GPIO_TypeDef*  step_port, uint8_t step_pin, GPIO_TypeDef*  dir_port,
     uint8_t dir_pin){
        //INTIATE THE MOTORSTRUCT 
    m->timer=timer;
    m->step_port=step_port;
    m->dir_pin=dir_pin;
    m->dir_port=dir_port;
    m->state= STOPPED;
    m->steps_remaining = 0;
    m->steps_total     = 0;
    m->start_arr       = START_ARR;
    m->target_arr      = START_ARR;
    //CONFIGURE THE GPIO TO OUTPUT
    step_port->MODER &=~(3<<(step_pin*2));
    step_port->MODER |=  (1 << (step_pin * 2));
    step_port->ODR   &= ~(1 << step_pin);
// CONFIGURE THE TIMER 
    timer->PSC = 83;
    timer->ARR = START_ARR;
    timer->EGR |= (1<<0);
    timer->SR &= ~(1<<0);
    timer->DIER |= (1<<0);
}

//so here we have have to thing 
//1. intia all the motors, and uart in the main 
//2. once uart is inita whenver the data is getting filled(uartirqhandle we have fire this with priority as well) in the comand buffer arrya then we parse it 
//3. dma also we need init so and restarrt dma as well
//in main while loop(1){once the comands are parsed and send it quespush head and tail }


void movemotor(Motor* m, int32_t angle,int32_t arr,int32_t target_arr){
if(angle>0){
    m->dir_port->ODR |=(1<<m->dir_pin);//clockwise
}
else{
    m->dir_port->ODR &=~(1<<m->dir_pin);//anticlokwiserotation
}

uint32_t steps = (uint32_t)(
        ((float)abs(angle) / 360.0f) * STEPS_PER_REV
    );
 if (steps == 0) return;

 //stepdat
       m->steps_remaining = steps;
    m->steps_total     = steps;
    m->start_arr       = start_arr;
    m->target_arr      = target_arr;
    m->state           = MOVING;
 
    // set timer
    m->timer->ARR  = start_arr;
    m->timer->CNT  = 0;
    m->timer->EGR |= (1<<0);
    m->timer->SR  &= ~(1<<0);


}

 uint32_t scurvearr(Motor*m){
    

 }

void startmotors(){


}
//here i need to push the thing into the queue fifo ,
// array i need have

void USART2_IRQHandler(void) {
    if (USART2->SR & (1<<4)) {              // IDLE flag
        volatile uint32_t dummy = USART2->DR; // clear flag

        // stop DMA
        DMA1_Stream5->CR &= ~(1<<0);

        // calculate bytes received
       bytes_received = 64 - DMA1_Stream5->NDTR;
       command_bytes[bytes_received] = '\0'; 
        waypoint wp;
      if(parsecommand((char*)command_bytes, &wp)){
        if(!queuepush(&wp)){


        }


      }

     // null terminate

    // command_buffer = 1;    
                    // tell main loop
    }
}





uint8_t parsecommand(char*cmd_buffer,waypoint* wp){
int j[6] = {0};
    int sa[6], ta[6];
    int total_time = 0;

    // initialize arrays
    for (int i = 0; i < 6; i++) {
        sa[i] = 4999;
        ta[i] = 4999;
    }

int parsed=sscanf(cmd_buffer,  "MOVE "
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

        if(parsed<1) return 0;

        for(int i=0;i<NUMBER_JOINTs;i++){
            wp->angle[i]=j[i];
            wp->start_arr[i]=sa[i];
            wp->target_arr[i]=ta[i];

        }
        wp->time_ms=(uint32_t) total_time;    
    return 1;
    
    }


