/*
 * motores.c
 *
 *  Created on: Feb 19, 2014
 *      Author: victor
 */
#include "motores.h"

/*************************************************************************
	functions
 *************************************************************************/
#define MORE_PWM 200
#define LESS_PWM 170

void goBack(char flag)
{
	//printf("goBack\n");
	MOTOR_SHIELD_PORT |= _BV(MOTOR_INB) | _BV(MOTOR_INA);
	if (flag == 1) {
		TIMER0_COMPARE_A_CONFIGURE(LESS_PWM);
		TIMER0_COMPARE_B_CONFIGURE(LESS_PWM);
	}
	else  {
		TIMER0_COMPARE_A_CONFIGURE(MORE_PWM);
		TIMER0_COMPARE_B_CONFIGURE(MORE_PWM);
	}
}

void goForward(char flag)
{
	//printf("goForward\n");
	MOTOR_SHIELD_PORT &= ~(_BV(MOTOR_INB) | _BV(MOTOR_INA) );
	if (flag == 1) {
		TIMER0_COMPARE_A_CONFIGURE(LESS_PWM);
		TIMER0_COMPARE_B_CONFIGURE(LESS_PWM);
	}
	else {
		TIMER0_COMPARE_A_CONFIGURE(MORE_PWM);
		TIMER0_COMPARE_B_CONFIGURE(MORE_PWM);
	}
}

void goLeft(char flag)
{
	//printf("goRight\n");
	MOTOR_SHIELD_PORT &= ~(_BV(MOTOR_INB) | _BV(MOTOR_INA) );
	TIMER0_COMPARE_A_CONFIGURE(0);
	if (flag == 1)  TIMER0_COMPARE_B_CONFIGURE(LESS_PWM); else TIMER0_COMPARE_B_CONFIGURE(MORE_PWM);
}

void goRight(char flag)
{
	//printf("goLeft\n");
	MOTOR_SHIELD_PORT &= ~(_BV(MOTOR_INB) | _BV(MOTOR_INA) );
	if (flag == 1)  TIMER0_COMPARE_A_CONFIGURE(LESS_PWM); else TIMER0_COMPARE_A_CONFIGURE(MORE_PWM);
	TIMER0_COMPARE_B_CONFIGURE(0);
}

void stopMove(void)
{
	TIMER0_COMPARE_A_CONFIGURE(0);
	TIMER0_COMPARE_B_CONFIGURE(0);
}

void turn(void)
{
	MOTOR_SHIELD_PORT |= _BV(MOTOR_INB);
	MOTOR_SHIELD_PORT &= ~_BV(MOTOR_INA);
	TIMER0_COMPARE_A_CONFIGURE(LESS_PWM);
	TIMER0_COMPARE_B_CONFIGURE(LESS_PWM);
}
