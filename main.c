 /*
 * main.c
 * Copyright (C) 2002-2011 Renato Jorge Caleira Nunes renato.nunes@ist.utl.pt
 * Copyright (C) 2013  David Ludovino david.ludovino@gmail.com
 * 
 * Initializes the microcontroller and modules.
 * Calls mudule's tasks in round-robin.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* Global includes */
#define _MAIN_  /* used in node.h */
#include "sysmods/node.h"

/* Global includes for AVR */
#include <avr/io.h>
#include <avr/interrupt.h>

/* Specific inlucdes for this module */
#include "sysmods/time.h"
#include "sysmods/led.h"
#include "sysmods/uart.h"
#include "sysmods/adc.h"
#include "sysmods/digitalrw.h"
#include "sysmods/pwm.h"
#include "usermods/example.h"
#include "usermods/robot.h"
#include "usermods/uart.h"
#include "usermods/LS_ATmega328.h"
#include "usermods/LS_defines.h"
#include "usermods/sensorIR.h"
#include "usermods/motores.h"
/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

/* 9600 baud */
#define UART_BAUD_RATE 9600

static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);

/* Initializes AVR microcontroller. */
void avr_init(void) {
	/* DDR - Data Direction Register.
	 * Specifies if a port should be an input 0 or an output 1.
	 * PORTA does not exist in ATmegaX8 */
	DDRB |= _BV(PB5); /* Arduino's digital outputs 8-13. Pin 5 is connected to Arduino's onboard LED. */
				 /* pins: 7, 6, and 5 as outputs: 1110 0000 */
	//DDRC = 0x00; /* Arduino's analog inputs 0-5 */
	//DDRD = 0x0E; /* Arduino's digital outputs 0-7. RX = 0 and TX = 1 0000 1110 */

	/* All outputs = 0 and pull-ups off to allow reading. */
	//PORTB = PORTC = PORTD = 0x00;
}

/*************************************************************************
Function: timer0_init
Purpose:  used to generate PWM to the motor wheels
 **************************************************************************/
void timer0_init(void) {
	MOTOR_SHIELD_DDR |= _BV(MOTOR_PWMA) | _BV(MOTOR_PWMB) | _BV(MOTOR_INA) | _BV(MOTOR_INB);
	TIMER0_PHASE_CORRECT_MAX_PWM_MODE();
	TIMER0_OC0A_CLEAR_ON_COMPARE();
	TIMER0_OC0B_CLEAR_ON_COMPARE();
	TIMER0_CLOCK_PRESCALER_OFF();
	TIMER0_COMPARE_A_CONFIGURE(0);
	TIMER0_COMPARE_B_CONFIGURE(0);
}

/*************************************************************************
Function: ADC_init
Purpose:  set up the ADC to be used in IR sensor
 **************************************************************************/
void ADC_init(void) {
	SENSOR_IR_DDR &= ~_BV(SENSOR_IR_BIT);
	//ADC in 10bits : used in IR sensor routine : without shift left or right
	ADC_REFERENCE_AREF(); 			//ADC reference in 5V
	ADC_CLOCK_PRESCALER_128();
	/* the original ADC frequency of this project was 125KHz (Prescaler = 128), thus, I changed it to sample faster, in 1MHz (Prescaler16)
	 * I have some loss in precision, working in 10 bits with a frequency bigger than 200KHz, but in this case this do not matters
	 * */
	ADC_ENABLE();
	ADC_SELECT_CHANNEL_0();
	ADC_DIGITAL_INPUT_0_DISABLE();
	ADC_START_CONVERSION();				 //I discard the first sample, which takes 25 clock cycles
	ADC_WAIT_CONVERSION_FINISH();
}


/*************************************************************************
Function: uartInit
Purpose:  initialize uart in 9600 8-N-1 standard and sets serial to stdout
 **************************************************************************/
void uartInit(void) {  /* testado: OK */
	/*
	 *  Initialize UART library, pass baudrate and AVR cpu clock
	 *  with the macro
	 *  UART_BAUD_SELECT() (normal speed mode )
	 *  or
	 *  UART_BAUD_SELECT_DOUBLE_SPEED() ( double speed mode)
	 */
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );

	// setup our stdio stream
	stdout = &mystdout;
}

/*************************************************************************
Function: buzzer_init
Purpose:  configures the pin used by buzzer, to alarm obstacles
 **************************************************************************/
void buzzer_init(void){
	ALARM_DDR |= _BV(ALARM_BIT);
}


/* Main cicle. Initializes modules and calls their tasks in round-robin. */
int main(void) {
	/* sysmods init */
	avr_init();
	TIME_init();
	LED_init(1);
	//UART_init();
	//ADC_init_10bit();
	//DIGITALRW_init();
	//PWM_init(&EXAMPLE_PWM_PIN);

	/* usermods init */
	uartInit();
	ADC_init();
	timer0_init();
	buzzer_init();
	ROBOT_init();
	enable_interrupts; /* Enables all interrupts (node.h) */
	
	while(1) {
		/* sysmods task */
		TIME_task();
		LED_task();
		//DIGITALRW_task();
		//PWM_task();
		
		/* usermods task */
		ROBOT_task();
	}
}
