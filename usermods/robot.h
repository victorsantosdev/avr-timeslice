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

extern uint8_t ROBOT_timer_pwm;  	/* 10ms * 10  = 100ms*/
extern uint8_t ROBOT_timer_sonar; 	/* 10ms *7 = 70 ms*/
extern uint8_t ROBOT_timer_adc;  	/* 10ms *6 = 60 ms*/
extern uint8_t ROBOT_timer_uart; 	/* 10ms *5 = 50 ms*/


/* public functions **/
void ROBOT_init(void);
void ROBOT_task(void);

#endif /* ROBOT_H_ */
