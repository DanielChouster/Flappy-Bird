#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h" 

//#define PORTD	PIC32_R (0x860D0) 
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

/*
f) Create a new file time4io.c. Begin the file with the following three lines:
#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"
In this file, write a C function that returns the status of the toggle-switches on the board, with the
following specification.
Function prototype: int getsw( void );
Parameter: none.
Return value: The four least significant bits of the return value should contain data from switches
SW4, SW3, SW2, and SW1. SW1 corresponds to the least significant bit. All other bits of the return
value must be zero.
Notes: The function getsw will never be called before Port D has been correctly initialized. The
switches SW4 through SW1 are connected to bits 11 through 8 of Port D. 

g) In file time4io.c, add a C function that returns the current status of the push-buttons BTN2,
BTN3, and BTN4 with the following specification1
.
Function prototype: int getbtns(void);
Parameter: none.
Return value: The 3 least significant bits of the return value must contain current data from push
buttons BTN4, BTN3, and BTN2. BTN2 corresponds to the least significant bit. All other bits of
the return value must be zero.
Notes: The function getbtns will never be called before Port D has been correctly initialized. The
buttons BTN4, BTN3, and BTN2, are connected to bits 7, 6 and 5 of Port D.


Modified by Daniel Chouster during spring term 2021.

*/



int getsw (void)
{
	volatile int* portd = PORTD;
	int switch_ = *portd; //from headerfile
	switch_ = (switch_ >> 8) & 0xF; //Shift right and clear all bits except for the last 4
	return switch_;
	
}


int getbtns(void)
{
	volatile int* portd = PORTD;
	int buttons = ((*portd)>> 5) & 0x7;
	return buttons;
}
