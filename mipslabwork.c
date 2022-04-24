/* mipslabwork.c

This file written 2015 by F Lundevall
Updated 2017-04-21 by F Lundevall

This file should be changed by YOU! So you must
add comment(s) here with your name(s) and date(s):

This file modified 2017-04-31 by Ture Teknolog

Modified by Daniel Chouster during spring term 2021.

For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
#include <stdio.h>
#include <stdlib.h>

int timeoutcount = 0;


#define BIRD_W 5 //width of the bird
#define BIRD_H 4 //height of the bird
#define BIRD_FALL 3
#define BIRD_RISE 9
#define PIPE_COUNT 3 //max number of pipes in case I make them smaller
#define GAP 22 //gap between pipes
#define W_PIPE 8 //pipe width
#define PIPE_DIST 50 //distance between pipes
#define PIPE_CONSTANT 2 //distance moving each iteration
volatile int *Porte; //used for the diode
int GameStatus; //wating to start (0), game is running (1), game over (2)
int score; //global variable score
int X, Y; //coordinates of the bird, lines are drawn from top left corner
uint8_t image[512]; //(128x32)/8 buffer all here, then display, from mipslabdata
int X_pipe[PIPE_COUNT]; //array for x-coordinate
int H_pipe[PIPE_COUNT]; //array for the height of the upper part of the pipe
int r = 20; //used in random

//char textstring[] = "text, more text, and even more text!";

/* Interrupt Service Routine */
void user_isr( void ) {
	return;
}

void SetBit (int x, int y)
//set bit puts logical coordinates into pshysical
{
	if (x < 0 || y < 0) //check if the coordinates are on the display
		return;
	if (x > 127 || y > 31)
		return;
	int l = y/8; //check which row on the display (0-3)
	int b = y % 8; //bit number in the byte
	int n = 128 * l + x; //index image array f
	image[n] &= ~(1<<b); //shift with bit number, reset that bit
	//set it to 0 -> white
}

void drawRectangle(int x, int y, int w, int h) {
//draw the pipes
	int i;
	int j;
	for(i = x; i < x + w; i++) //x-coordinate, x + width
		for(j = y; j < y + h; j++) //y-coordinate, y + height
			SetBit(i, j);

}

void DrawBird(void) {
//logical coordinates
	int i;
	for(i = 0; i < 4; i++)
		SetBit(X + i, Y);
	for(i = 1; i < 5; i++)
		SetBit(X + i, Y + 1);
	for(i = 1; i < 5; i++)
		SetBit(X + i, Y + 2);
	for(i = 0; i < 4; i++)
		SetBit(X + i, Y + 3);
	/*
	----
	 ----
	 ----
	----
	*/

}

void drawPipe(int x, int h) {
	if (x == 0 && h == 0) //if there is no pipe. just in case we use more pipes
		return;

	drawRectangle(x, 0, W_PIPE, h); //draw first half
	drawRectangle(x, h + GAP, W_PIPE, 32-GAP-h); //draw second half
	//x-coordinate, y-coordinate, pipe width, height of lower pipe.

}

void drawPipes(void) {
	int i;
	for(i = 0; i  < PIPE_COUNT; i++) {
		drawPipe(X_pipe[i],H_pipe[i]); //take all pipes that are in the arrray

	}
}

void modifyPipe(void) {
	int i = 0;

	//delete first pipe if it no longer shown on the display
	if (X_pipe[0]<-W_PIPE) { //check if I should detele it
		for(i = 0; i < PIPE_COUNT -1; i++) { //move all pipes one index, second one becomes first and so on
			X_pipe[i] = X_pipe[i + 1];
			H_pipe[i] = H_pipe[i + 1];
		}

		X_pipe[PIPE_COUNT-1]=0; //mark last one as empty
		H_pipe[PIPE_COUNT-1]=0;
	}


	int i_last =-1; //will be -1 at the start of the game

	for(i = 0; i < PIPE_COUNT; i++) {

		if (X_pipe[i] > 0) //find the pipe on the farthest right
			i_last = i;
	}

	//add new pipe
	int i_new =-1; //is -1 if there are no 
	if (i_last < PIPE_COUNT-1) //check if it is the last element in the array
		i_new = i_last+1; //next will be the one after the last
	if (i_last == -1) //start of the game
		i_new = 0; //if there are no elements, i_new will be in first position. Used at the beginning of the game
	//add new
	if ((i_last==-1 || 128-X_pipe[i_last]>PIPE_DIST) && i_new!=-1) { //(if we are at the start of the game, if there is no pipe on the display or the pipe has moved more than PIPE DIST) && there is room in the array
		X_pipe[i_new]=128; //set new pipe to coordinate 128
		H_pipe[i_new]=Random_H(); //randomize height
	}


}


int Random_H() {
	//Linear congruential generator from Wiki
	//https://en.wikipedia.org/wiki/Linear_congruential_generator
	int m = 32 - GAP; //max height
	int a = 1;
	int c = 5;

	r = (a*r+c)%m;
	if(r == 0) //avoid no upper pipe
		r = 1;

	return r;
}


void cleardisplay(void) {
	int i;
	for (i = 0; i <512; i++) {
		image[i] = 0xFF; //clear screen, 1 means clear because they are inverted in display_image from mipslabfunc
	}
}

void checkforgameover(void) {
	//Check for ground
	if (Y + BIRD_H > 31) {
		GameStatus = 2; //game over

	}
	int i;
	//Check for pipe
	for(i = 0; i < PIPE_COUNT; i++) {
		if (H_pipe[i]>0) { //check if there is a pipe
			if (X + BIRD_W > X_pipe[i] && X-BIRD_W <= X_pipe[i]+W_PIPE) { //check if the x-coordinate of the bird is within the x-coordinate of the pipe
				int low=H_pipe[i]; //y coordinate lower part of the upper pipe
				int high=low+GAP; //y coordinate of the upper part of the lower pipe, upper part
				if (Y < low || Y + BIRD_H > high) {
					GameStatus = 2;
					return;
				}
			}

			if (X>X_pipe[i]+W_PIPE-PIPE_CONSTANT && X<X_pipe[i]+W_PIPE+PIPE_CONSTANT) //check if the bird has passed through the pipe
			
			{
				score++;
				if (score%5==0) { 
					
					int j = score/5-1; //bit index
				
					(*Porte)|= 1 << j; //add medal every 5

				}
			}

		}
	}

}


void gameinit (void) { //before game starts
	score = 0;
	X = 10;
	Y = 7;
	*Porte = 0; //Start from 0
	timeoutcount = 0;

	int i;
	for(i = 0; i < PIPE_COUNT; i++) { //delete all pipes
		X_pipe[i]=0;
		H_pipe[i]=0;
	}


}

void display_score(int line) {
	display_string(line, " SCORE:"); //display "SCORE:"
	char *myscore=itoaconv(score); //from mipslabfunc, receives integer and returns pointer to char array
	int i;
	for(i=7; i<16; i++) {//textbuffer -> 4x16... start from 7th symbol
		if(*myscore) { //check if it is not zero. a zero means end of string
			textbuffer[line][i] = *myscore; //print to display
			myscore++;
		} else {
			break;
		}
	}
}




/* Lab-specific initialization goes here */
void labinit( void ) { //done before start

//FROM LAB 3
	volatile int *Trise = (volatile int*) 0xbf886100; //adress to Trise
	*Trise &= 0xFFFFFF00; // bits [7:0] are set to ouput
	Porte = (volatile int*) 0xbf886110;
	*Porte = 0; //Start from 0
//	volatile int *Trisd = (volatile int *) TRISD;
//	*Trisd |= 0x00003FE0; // bits [13:5] are set to input


	int TIME_PERIOD = (80000000/256)/10; //FPS, 10 per second

	T2CON = 0x70; //Stop timer and clear control register,
	// set prescaler at 1:256, user internal clock checks. 0x70 -> bit 6-4 are 1. Page 9 PIC32 Reference Manual http://ww1.microchip.com/downloads/en/DeviceDoc/61105F.pdf
	TMR2 = 0x0; // Clear timer register, increments 1 every 256 per second, parallell with the written program
	PR2 = TIME_PERIOD; // Load period register, PIC32 clock speed is 80 MHz,
	T2CONSET = 0x8000; // Start timer


	//When TMR2 == PR2, a flag is put
	GameStatus = 0;

	return;

}

/* This function is called repetitively from the main program */
void labwork( void ) {

	int BT = 0;
	int B = getbtns();
	if (B & (1 << 2)) { //check for 4th button
		BT = 1; //set to true
	}

	if (GameStatus == 0) { //before the game starts
		//display a message on the screen
		//these two functions are from lab 3
		display_string(0, "  FLAPPY BIRD");
		display_string(1, "");
		display_string(2, " PRESS BUTTON 3");
		display_string(3, "    TO PLAY");
		display_update();
	}

	if (GameStatus == 1) { //game running
		if (IFS(0)) {
			//check if there is a flag, 10 times per sec
			IFS(0) = 0; //reset flag
			Y+= BIRD_FALL; //unconditional fall
			if (BT == 1)
				Y-= BIRD_RISE; //When the button is pressed, the bird flies higher
			modifyPipe(); //check if the pipes have passed left side of the display, delete. check how far the one on the farthest right is from the display, add one more pipe if it is more con pipe distance.
			int i;
			for(i = 0; i < PIPE_COUNT; i++) {
				if (X_pipe[i] > 0 || i == 0) //for first iteration or when if x-coordinate is > 0 , move pipes to the left
					X_pipe[i] -= PIPE_CONSTANT; //move

			}

			checkforgameover();

		}


		cleardisplay();
		DrawBird();


		drawPipes();


		display_image(0, image); //start from zero (top left corner), copy image to the screen


	}

//display message at the end
	if (GameStatus == 2) {
		display_string(0, " GAME OVER!");
		display_score(1);
		display_string(2, " PRESS BUTTON 3");
		display_string(3, " TO PLAY AGAIN");
		display_update(); //update text on the screen
	}


	if (GameStatus != 1) { //when game is not running

		if (BT == 0 && B > 0) { //check for any button except for the 4th
			gameinit();
			GameStatus = 1;

		}
	}


}
