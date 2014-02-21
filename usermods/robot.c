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
#include "ultrasom.h"

#define READ_UART_INTERVAL 	 	1 // 50ms
#define READ_ADC_INTERVAL   	6 // 6*10ms = 60ms
#define READ_SONAR_INTERVAL  	7 // 7*10ms = 70ms
#define MOTOR_INTERVAL 			1 // 100 ms


/* Global varibales ::vars **/
uint8_t ROBOT_timer_pwm;   /* 100 ms*/
uint8_t ROBOT_timer_sonar;  /* 70 ms*/
uint8_t ROBOT_timer_adc;    /* 60 ms*/
uint8_t ROBOT_timer_uart;   /* 50 ms*/

/* Local variables **/

#define DEBUG_PIN1 PC3
#define DEBUG_PIN2 PC4
#define DEBUG_PIN3 PC5

#define WITH_CARE 1
#define NO_CARE	  0

//only detected as obstacle if distance is below 25cm, below 10cm the sensor starts misreading
#define IR_OBSTACLE_THRESHOLD 20
#define IR_OBSTACLE_UPPER_THRESHOLD 30
#define SONAR_OBSTACLE_THRESHOLD 20
#define SONAR_OBSTACLE_UPPER_THRESHOLD 30

void ROBOT_init(void) {
	/*configuração de pinos de debug*/
	//DDRC |= (_BV(DEBUG_PIN1) | _BV(DEBUG_PIN2) | _BV(DEBUG_PIN3));
}

void ROBOT_task(void) {
	static uint8_t count_uart=0;
	static uint8_t count_pwm=0;
	static uint8_t count_adc=0;
	static uint8_t count_sonar=0;
	static uint8_t ciclosClock_motor=0;
	static char received=0;
	static char IR_obstaculo=0;
	static unsigned int SONAR_obstaculo=0;
	static char obstacle_flag_sonar=0;
	static char obstacle_flag_ir=0;
	static char obstacle_flag=0;
	static char autonomous_flag=0;
	static char flag_autonomous_obstacle_ir=0;
	static char flag_autonomous_obstacle_sonar=0;
	static char flag_autonomous_obstacle=0;


	if(ROBOT_timer_uart >= READ_UART_INTERVAL) { //cada 50ms
		ROBOT_timer_uart = 0;
		if (count_uart==0) {
			//set_bit(PORTC, DEBUG_PIN1);
			count_uart = 1;
			received = 0;
			if (uart_available() > 0) {
				received = uart_getc();
				if (received == 'a') autonomous_flag = 1;
				if (received == 'q') autonomous_flag = 0;
			}
		}
		else  {
			//clr_bit(PORTC, DEBUG_PIN1);
			count_uart = 0;
		}
	}

	if(ROBOT_timer_adc >= READ_ADC_INTERVAL) { //cada 60ms
		ROBOT_timer_adc = 0;
		if(autonomous_flag == 1) {
			IR_obstaculo = 0;
			IR_obstaculo = verificaObstaculoIR();
			if (IR_obstaculo >= IR_OBSTACLE_THRESHOLD && IR_obstaculo < IR_OBSTACLE_UPPER_THRESHOLD) flag_autonomous_obstacle_ir = 0;
			else if (IR_obstaculo >= IR_OBSTACLE_UPPER_THRESHOLD)									 flag_autonomous_obstacle_ir = 1;
			else 																					 flag_autonomous_obstacle_ir = 2;
		} else {
			if (count_adc==0) {
				//set_bit(PORTC, DEBUG_PIN3);
				count_adc = 1;
				IR_obstaculo = 0;
				IR_obstaculo = verificaObstaculoIR();
				printf("distancia_ir: %d\n", IR_obstaculo);
				if (IR_obstaculo > IR_OBSTACLE_THRESHOLD) 														obstacle_flag_ir = 0;
				else if (IR_obstaculo > IR_OBSTACLE_THRESHOLD && IR_obstaculo < IR_OBSTACLE_UPPER_THRESHOLD) 	obstacle_flag_ir = 1;
				else if (IR_obstaculo <= IR_OBSTACLE_THRESHOLD ) 												obstacle_flag_ir = 2;
			}
			else  {
				//clr_bit(PORTC, DEBUG_PIN3);
				count_adc = 0;
			}
		}
	}

	if(ROBOT_timer_sonar >= READ_SONAR_INTERVAL) { //cada 70ms
		ROBOT_timer_sonar = 0;
		if(autonomous_flag == 1) {
			SONAR_obstaculo = 0;
			SONAR_obstaculo = get_UltrasoundData();
			if (SONAR_obstaculo >= SONAR_OBSTACLE_THRESHOLD && SONAR_obstaculo < SONAR_OBSTACLE_UPPER_THRESHOLD) flag_autonomous_obstacle_sonar = 0;
			else if (SONAR_obstaculo >= SONAR_OBSTACLE_UPPER_THRESHOLD)									 		 flag_autonomous_obstacle_sonar = 1;
			else 																					 			 flag_autonomous_obstacle_sonar = 2;
			flag_autonomous_obstacle = flag_autonomous_obstacle_ir | flag_autonomous_obstacle_sonar;
		} else {
			if (count_sonar==0) {
				//set_bit(PORTC, DEBUG_PIN3);
				count_sonar = 1;
				SONAR_obstaculo = 0;
				SONAR_obstaculo = get_UltrasoundData();
				printf("distancia_sonar: %d\n", SONAR_obstaculo);
				if (SONAR_obstaculo > SONAR_OBSTACLE_THRESHOLD) 															obstacle_flag_sonar = 0;
				else if (SONAR_obstaculo > SONAR_OBSTACLE_THRESHOLD && SONAR_obstaculo < SONAR_OBSTACLE_UPPER_THRESHOLD) 	obstacle_flag_sonar = 1;
				else if (SONAR_obstaculo <= SONAR_OBSTACLE_THRESHOLD ) 														obstacle_flag_sonar = 2;
				obstacle_flag = obstacle_flag_ir | obstacle_flag_sonar;
			}
			else  {
				//clr_bit(PORTC, DEBUG_PIN3);
				count_sonar = 0;
			}
		}
		if( (flag_autonomous_obstacle & 0b10) == 0b10) flag_autonomous_obstacle = 2;
		else if ( (flag_autonomous_obstacle & 0b01) == 0b01) flag_autonomous_obstacle = 1;
		else flag_autonomous_obstacle = 0;

		if( (obstacle_flag & 0b10) == 0b10) obstacle_flag = 2;
		else if ( (obstacle_flag & 0b01) == 0b01) obstacle_flag = 1;
		else obstacle_flag = 0;
	}

	if(ROBOT_timer_pwm >= MOTOR_INTERVAL) { //cada 100ms
		ROBOT_timer_pwm = 0;

		if (autonomous_flag == 1) {
			switch (flag_autonomous_obstacle) {
				case 0:
					clr_bit(ALARM_PORT_RED, ALARM_BIT_RED);
					set_bit(ALARM_PORT_GREEN, ALARM_BIT_GREEN);
					goForward(WITH_CARE);
					break;
				case 1:
					clr_bit(ALARM_PORT_RED, ALARM_BIT_RED);
					set_bit(ALARM_PORT_GREEN, ALARM_BIT_GREEN);
					goForward(NO_CARE);
					break;
				case 2:
					goBack(0);
					turn();
					set_bit(ALARM_PORT_RED, ALARM_BIT_RED);
					clr_bit(ALARM_PORT_GREEN, ALARM_BIT_GREEN);
					break;
			}
		}
		else {
			if (ciclosClock_motor == 2) {	//duas bordas de subida depois de acionar o motor (200ms depois de acionar o motor)
				stopMove();
				ciclosClock_motor=0;
			}
			if (count_pwm==0) {
				//set_bit(PORTC, DEBUG_PIN2);
				count_pwm = 1;
				if (obstacle_flag == 0 || obstacle_flag == 1) {
					clr_bit(ALARM_PORT_RED, ALARM_BIT_RED);
					set_bit(ALARM_PORT_GREEN, ALARM_BIT_GREEN);
					switch(received) {
						case 'u':
							goForward(obstacle_flag);
							break;
						case 'd':
							goBack(obstacle_flag);
							break;
						case 'l':
							goLeft(obstacle_flag);
							break;
						case 'r':
							goRight(obstacle_flag);
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
						set_bit(ALARM_PORT_RED, ALARM_BIT_RED);
						clr_bit(ALARM_PORT_GREEN, ALARM_BIT_GREEN);
					}
				}
			}
			else  {
				ciclosClock_motor++;
				//clr_bit(PORTC, DEBUG_PIN2);
				count_pwm = 0;
			}
		}
	}
}
