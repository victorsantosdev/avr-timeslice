/*
 * robot.h
 *
 *  Created on: Feb 19, 2014
 *      Author: victor
 */

#ifndef ROBOT_H_
#define ROBOT_H_

/* Global includes for AVR */
#include <avr/io.h>

extern uint8_t ROBOT_timer_pwm;  /* 100ms*/
extern uint8_t ROBOT_timer_adc;  /* 10ms *6 = 60 ms*/
extern uint8_t ROBOT_timer_uart; /* 50 ms*/


/* public functions **/
void ROBOT_init(void);
void ROBOT_task(void);

#endif /* ROBOT_H_ */
