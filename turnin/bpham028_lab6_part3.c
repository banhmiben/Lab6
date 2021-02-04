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

enum States {Start, Init, WaitPress ,Dec, DecHold, DecRelease, Inc, IncHold, IncRelease, Reset} State;

unsigned char tmpA = 0x00;
unsigned char cnt = 0x00;

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

void Tick() {

	tmpA = ~PINA;

	switch(State) {
		case(Start):
			State = Init;
			break;
		case(Init):
			State = WaitPress;
			break;
		case(WaitPress):
			cnt = 0x00;
			if (tmpA == 0x01) {
				State = Inc;
			} else if (tmpA == 0x02) {
				State = Dec;
			} else if (tmpA == 0x03) {
				State = Reset;
			} break;
		case(Inc):
			if (tmpA == 0x01) {
				State = IncHold;
			} else {
				State = IncRelease;
			} break;
		case (IncHold):
			if (tmpA == 0x01) {
				State = IncHold;
			} else {
				State = IncRelease;
			}
		case(IncRelease):
			State = WaitPress;
			break;
		case(Dec):
			if (tmpA == 0x02) {
				State = DecHold;
			} else {
				State = DecRelease;
			} break;
		case(DecRelease):
			State = WaitPress;
			break;
		case (Reset):
			if (tmpA == 0x03) {
				State = Reset;
			} else {
				State = WaitPress;
			} break;
		default:
			State = Start;
			break;
	}

	switch(State) {
		case(Start):
			break;
		case(Init):
			break;
		case(WaitPress):
			break;
		case(Inc):
			if (PORTC < 0x09) {
				++PORTC;
			} break;
		case (IncHold):
			++cnt;
			if ((PORTC < 0x09) && (cnt == 0x0A)) {
				++PORTC;
			} break;
		case (IncRelease):
			break;
		case (Dec):
			if (PORTC > 0) {
				--PORTC;
			} break;
		case (DecHold):
			++cnt;
			if ((PORTC > 0x00) && (cnt == 0x0A)) {
				--PORTC;
			} break;
		case(DecRelease):
			break;
		case(Reset):
			PORTC = 0x00;
			break;
		default:
			break;
	}
}

int main(void) {

	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	TimerSet(100);
	TimerOn();
	State = Start;
	PORTC = 0x07;
	while(1) {
		Tick();
		while (!TimerFlag){}
		TimerFlag = 0;
	}

}
