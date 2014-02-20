/*
 * robot.c
 *
 *  Created on: Feb 19, 2014
 *      Author: victor
 */

#include "robot.h"

/* Global includes */
#include "../sysmods/node.h"

/* Global inclues for AVR */

/* Includes specific to this module */
#include "../sysmods/pwm.h"
#include "avr/io.h"
#include "LS_ATmega328.h"
#include "LS_defines.h"
#include "uart.h"
#include "motores.h"
#include "sensorIR.h"

#define READ_UART_INTERVAL  1 // 50ms
#define READ_ADC_INTERVAL   6 // 6*10ms = 60ms
#define MOTOR_INTERVAL 		1 // 100 ms


/* Global varibales ::vars **/
uint8_t ROBOT_timer_pwm;  /* 100 ms*/
uint8_t ROBOT_timer_adc;  /* 10 ms*/
uint8_t ROBOT_timer_uart; /* 50 ms*/

/* Local variables **/

#define DEBUG_PIN1 PC3
#define DEBUG_PIN2 PC4
#define DEBUG_PIN3 PC5

#define WITH_CARE 1
#define NO_CARE	  0

//only detected as obstacle if distance is below 25cm, below 10cm the sensor starts misreading
#define IR_OBSTACLE_THRESHOLD 20
#define IR_OBSTACLE_UPPER_THRESHOLD 30

void ROBOT_init(void) {
	/*configuração de pinos de debug*/
	DDRC |= (_BV(DEBUG_PIN1) | _BV(DEBUG_PIN2) | _BV(DEBUG_PIN3));
}

void ROBOT_task(void) {
	static uint8_t count_uart=0;
	static uint8_t count_pwm=0;
	static uint8_t count_adc=0;
	static uint8_t ciclosClock_motor=0;
	static char received=0;
	static int IR_obstaculo=0;
	static char obstacle_flag=0;
	static char autonomous_flag=0;
	static char flag_autonomous_obstacle=0;

	if(ROBOT_timer_uart >= READ_UART_INTERVAL) { //cada 50ms
		ROBOT_timer_uart = 0;
		if (count_uart==0) {
			set_bit(PORTC, DEBUG_PIN1);
			count_uart = 1;
			received = 0;
			if (uart_available() > 0) received = uart_getc();
			if (received == 'a') autonomous_flag = 1;
			if (received == 'q') autonomous_flag = 0;
		}
		else  {
			clr_bit(PORTC, DEBUG_PIN1);
			count_uart = 0;
		}
	}

	if(ROBOT_timer_adc >= READ_ADC_INTERVAL) { //cada 60ms
		ROBOT_timer_adc = 0;
		if(autonomous_flag == 1) {
			IR_obstaculo = 0;
			IR_obstaculo = verificaObstaculoIR();
			if (IR_obstaculo >= IR_OBSTACLE_THRESHOLD && IR_obstaculo < IR_OBSTACLE_UPPER_THRESHOLD) flag_autonomous_obstacle = 0;
			else if (IR_obstaculo >= IR_OBSTACLE_UPPER_THRESHOLD)									 flag_autonomous_obstacle = 1;
			else 																					 flag_autonomous_obstacle = 2;
		} else {
			if (count_adc==0) {
				set_bit(PORTC, DEBUG_PIN3);
				count_adc = 1;
				IR_obstaculo = 0;
				IR_obstaculo = verificaObstaculoIR();
				if (IR_obstaculo > IR_OBSTACLE_THRESHOLD) 														obstacle_flag = 0;
				else if (IR_obstaculo > IR_OBSTACLE_THRESHOLD && IR_obstaculo < IR_OBSTACLE_UPPER_THRESHOLD) 	obstacle_flag = 1;
				else if (IR_obstaculo <= IR_OBSTACLE_THRESHOLD ) 												obstacle_flag = 2;
			}
			else  {
				clr_bit(PORTC, DEBUG_PIN3);
				count_adc = 0;
			}
		}
	}

	if(ROBOT_timer_pwm >= MOTOR_INTERVAL) { //cada 100ms
		ROBOT_timer_pwm = 0;
		if (autonomous_flag == 1) {
			switch (flag_autonomous_obstacle) {
				case 0:
					goForward(WITH_CARE);
					break;
				case 1:
					goForward(NO_CARE);
					break;
				case 2:
					goBack(0);
					turn();
					break;
			}
		}
		else {
			if (ciclosClock_motor == 2) {	//duas bordas de subida depois de acionar o motor (200ms depois de acionar o motor)
				stopMove();
				ciclosClock_motor=0;
			}
			if (count_pwm==0) {
				set_bit(PORTC, DEBUG_PIN2);
				count_pwm = 1;
				if (obstacle_flag == 0 || obstacle_flag == 1) {
					switch(received) {
						case 'u':
							goForward(1);
							break;
						case 'd':
							goBack(1);
							break;
						case 'l':
							goLeft(1);
							break;
						case 'r':
							goRight(1);
							break;
						default:
							stopMove();
							break;
					}
				} else {
					if (received == 'd') {
						goBack(WITH_CARE);
					} else {
						stopMove();
						obstacleAlarm();
					}
				}
			}
			else  {
				ciclosClock_motor++;
				clr_bit(PORTC, DEBUG_PIN2);
				count_pwm = 0;
			}
		}
	}
}
