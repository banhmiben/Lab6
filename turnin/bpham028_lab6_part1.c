/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #6  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *	Blink PB0 PB1 PB2 every second
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

enum States {Start, Init, First, Second, Third} State;

void TimerSet(unsigned long M) {
        _avr_timer_M = M;
        _avr_timer_cntcurr = _avr_timer_M;
}


void TimerOn() {

	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	
	SREG |= 0x80;
	
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	
	_avr_timer_cntcurr--; 
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
	
}

void TickBlink() {

	switch (State) {
		case(Start):
			State = Init;
			break;
		case(Init):
			State = First;
			break;
		case(First):
			State = Second;
			break;
		case(Second):
			State = Third;
			break;
		case(Third):
			State = First;
			break;
		default:
			State = Start;
			break;
	}

	switch (State) {
		case(Start):
			break;
		case(Init):
			PORTB = 0x00;
			break;
		case(First):
			PORTB = 0x01;
			break;
		case(Second):
			PORTB = 0x02;
			break;
		case(Third):
			PORTB = 0x04;
			break;
		default:
			PORTB = 0x00;
			break;
	}
}
	
int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(1000);
	TimerOn();
	State = Init;
//	unsigned char tmpB = 0x00;
    /* Insert your solution below */
    while (1) {
//	    tmpB = ~tmpB;
//	    PORTB = tmpB;
	    TickBlink();
	    while (!TimerFlag){}
	    TimerFlag = 0;
    }
}
