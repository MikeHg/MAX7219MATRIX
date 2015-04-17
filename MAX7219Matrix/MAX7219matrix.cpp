/*
 * max7219matrix.cpp
 * 
 * Copyright 2015  <Michael Haugk>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
#include "MAX7219matrix.h"
#include "MAX7219constants.h"
#include "MAX7219fonts.h"
#include <iostream>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <math.h>
#include <string.h>
#include <cstddef>


using namespace std;

MAX7219Matrix::MAX7219Matrix() {
	TxBuffer = new char[1024]; // Buffer for TxData
	RxBuffer = new char[1024]; // Buffer for RxData
	PadString = new char[NUM_MATRICES];
	
	for(int i=0;i<(NUM_MATRICES-1);i++) PadString[i]=' ';
	PadString[NUM_MATRICES-1] = '\0';
	
	
	TxBufferIndex = 0;
	RxBufferIndex = 0;
	
	SpiFd = -1;
}

MAX7219Matrix::~MAX7219Matrix() {
	clearAll();
	
	delete[] TxBuffer;
	delete[] RxBuffer;
	delete[] PadString;
	
	if(SpiFd != -1)
	{
	  close(SpiFd); // Close SPI port
    }
}

/*
 *   In order not to have to import the wiringPi library in other parts
 *   of the code, export the delay function
 */
void MAX7219Matrix::wait(long duration) {
	delay(duration);
}

/*
 *    Send data 'data' to the MAX7219 with the 'device' no.
 *    use either the column address or a MAX7219 register for 'adr'
 */
void MAX7219Matrix::SendData(char adr, char data, char device) 
{
	// Count from top to bottom because first data which is sent is for the last device in the chain
	for (int i = NUM_MATRICES; i > 0; i--)
	{
		if ((i == device) || (device == 255))
		{
			TxBuffer[TxBufferIndex] = adr;
			TxBufferIndex++;
			TxBuffer[TxBufferIndex] = data;
			TxBufferIndex++;
		}
		else // if its not the selected device send the noop command
		{
			TxBuffer[TxBufferIndex] = MAX7219_REG_NOOP;
			TxBufferIndex++;
			TxBuffer[TxBufferIndex] = 0;
			TxBufferIndex++;
		}
	}
	
	flush();
}

/*
 *    Write the data to the physical devices
 */
void MAX7219Matrix::flush() {
	write(SpiFd, TxBuffer, TxBufferIndex); // Write the data from TxBuffer to the SPI bus...
	TxBufferIndex = 0; // ...and reset the index
}

// write to all devices (255 = Broadcast)
void MAX7219Matrix::SendData(char adr, char data)
{
	SendData(adr, data, 255); 
}

/*
 *    initialize the matrix array
 */
void MAX7219Matrix::init() {
	begin(0, 1000000); // default use channel 0 and 1MHz clock speed
	
	// Initialise all of the MAX7219 chips (see datasheet for details of registers)
    SendData(MAX7219_REG_SCANLIMIT, 7);   // show all 8 digits
    SendData(MAX7219_REG_DECODEMODE, 0);  // using a LED matrix (not a 7-Segment displays)
    SendData(MAX7219_REG_DISPLAYTEST, 0); // no display test
    clearAll();                           // ensure the whole array is blank
    brightness(3);                        // set character intensity: range: 0..15
    SendData(MAX7219_REG_SHUTDOWN, 1);    // not in shutdown mode (i.e start it up)
}

/*
 *     Initialize the wiringPi SPI library
 */
void MAX7219Matrix::begin(int channel, int speed)
{
   if ((SpiFd = wiringPiSPISetup (channel, speed)) < 0)
	{	// Open port for reading and writing
		cout << "Failed to open SPI port " << channel << "! Please try with sudo" << endl;
	}
}

/*
 *     reset the SPI channel
 */
void MAX7219Matrix::reset()
{
   if (SpiFd != -1)
   {
	   close(SpiFd); // Close SPI port
   }
   MAX7219Matrix::init();
}

/*
 *     Set the brightness of the array between 0 and 15
 */ 
void MAX7219Matrix::brightness(char intense)
{
	if(intense > 0x0F) intense = 0x0F;
	SendData(MAX7219_REG_INTENSITY, intense);
}

/*
 *     Blank all matrices
 */
void MAX7219Matrix::clearAll() {
	int col;
	
	for(col=1;col<9;col++) { 
          SendData(col,0);
    }
}

/*
 *    Send one letter to a defined matrix and use the given font
 */ 
void MAX7219Matrix::sendLetter(int matrix, char letter, const char font[256][8]) { 
	if(matrix < 1 || matrix > NUM_MATRICES) return;
	
	int col;
	for(col=1;col<9;col++) {
		SendData(col,font[letter % 0x100][col-1], matrix);
	}
}

/*
 *  Subroutine used by scroll_message_horiz(), scrolls text once across the array, starting & ending with text on the array
 */
void MAX7219Matrix::scrollTextOnce(char *text, double delayTime, int direction, const char font[256][8]) {
	
	int length = strlen(text) - NUM_MATRICES;
	int startChar, stage, col, matrix;
	char columnDataChr;
	
	if(direction == DIR_L) {
		for(startChar = 0; startChar <= length; startChar++) {
			for(stage = 0; stage < 8; stage++) {
				for(col = 0; col < 8; col++) {
					for(matrix = NUM_MATRICES; matrix > 0; matrix--) {
						if(col+stage < 8) {
							columnDataChr = font[text[startChar + NUM_MATRICES - matrix - 1] % 0x100][col+stage];
						}
						else {
							columnDataChr = font[text[startChar + NUM_MATRICES - matrix] % 0x100][col+stage-8];
						}
						// Don't use SendData since this would set the values for each matrix individually
						TxBuffer[TxBufferIndex] = col+1;
						TxBufferIndex++;
						TxBuffer[TxBufferIndex] = columnDataChr;
						TxBufferIndex++;
					}
					flush();
				}
				delay(delayTime);
			}
		}
	}
	else if(direction == DIR_R) {
		for(startChar = length; startChar > 0; startChar--) {
			for(stage = 1; stage < 9; stage++) {
				for(col = 0; col < 8; col++) {
					for(matrix = NUM_MATRICES; matrix > 0; matrix--) {
						if(col >= stage) {
							columnDataChr = font[text[startChar + NUM_MATRICES - matrix] % 0x100][col-stage];
						}
						else {
							columnDataChr = font[text[startChar + NUM_MATRICES - matrix -1] % 0x100][col-stage+8];
						}
						// Don't use SendData since this would set the values for each matrix individually
						TxBuffer[TxBufferIndex] = col+1;
						TxBufferIndex++;
						TxBuffer[TxBufferIndex] = columnDataChr;
						TxBufferIndex++;
					}
					flush();
				}
				delay(delayTime);
			}
		}
	}
}

/*
 *  Scroll a text message across the array, for a specified (repeats) number of times
 *  speed: accept values between 2.0 (very slow) and 9.0 (pretty fast)
 *  direction: DIR_L or DIR_R only; DIR_U & DIR_D will do nothing
 *  Scrolling starts with message off the RHS(DIR_L)/LHS(DIR_R) of array, and ends with message off the LHS/RHS
 */
void MAX7219Matrix::scrollMessageHoriz(char *message, int repeats, double speed, int direction, const char font[256][8]) {
	
	if(direction != DIR_L && direction != DIR_R) return;
	
	// trim message
	// this will modify the original string!
	char *localMessage = message;
	while(*localMessage == ' ' || *localMessage == '\t' || *localMessage == '\n')
            localMessage++;

    int locLen = strlen(localMessage);
    while(locLen >= 0 && 
            (localMessage[locLen - 1] == ' ' || localMessage[locLen - 1] == '\t' || localMessage[locLen - 1] == '\n'))
    {
            *(localMessage + locLen - 1) = '\0';
            locLen--;
    }
    locLen = strlen(localMessage);
    
	if(locLen < 1) return;
	
	char* paddedMessage = new char[locLen + 2 * NUM_MATRICES];
	
	if(speed<2.0) speed = 2.0;
	if(speed>9.0) speed = 9.0;
	
	double delayTime = pow(0.5, speed) * 1000.0;
	strcpy(paddedMessage,PadString);
	strcat(paddedMessage,localMessage);
	strcat(paddedMessage,PadString);
	
	if(repeats <= 0) repeats = 1;
	for(int i =0; i<repeats; i++)
	    scrollTextOnce(paddedMessage, delayTime, direction, font);
	
    clearAll();
	
	delete[] paddedMessage;
}

/*
 * Transitions vertically between two different (truncated if necessary) text messages
 * speed: 0-9 for practical purposes; speed does not have to integral
 * direction: DIR_U or DIR_D only; DIR_L & DIR_R will do nothing
 */
void MAX7219Matrix::scrollMessageVert(char *oldMessage, char *newMessage, double speed, int direction, const char font[256][8])
{
	if(speed<2.0) speed = 2.0;
	if(speed>9.0) speed = 9.0;
    double delayTime = pow(0.5, speed) * 1000.0;
    
    if(direction != DIR_U && direction != DIR_D) return;
    
    int stage, col, matrix, mLen,i;
	char columnDataChr;
	
	char localOld[9], localNew[9];
	strncpy(localOld, oldMessage, 8);
	strncpy(localNew, newMessage, 8);
	
	mLen = strlen(localOld);
	for(i=mLen;i<8;i++) {
		localOld[i] = ' ';
	}
	localOld[8] = '\0';
	
	mLen = strlen(localNew);
	for(i=mLen;i<8;i++) {
		localNew[i] = ' ';
	}
	localNew[8] = '\0';
	
    for(stage = 0; stage <= 8; stage++) {
		for(col = 0; col < 8; col++) {
			for(matrix = NUM_MATRICES; matrix > 0; matrix--) {
				
				if(direction == DIR_D) {
				    columnDataChr = font[localOld[NUM_MATRICES - matrix] % 0x100][col] << stage | font[localNew[NUM_MATRICES - matrix] % 0x100][col] >> (8 - stage);
				}
				else if(direction == DIR_U) {
					columnDataChr = font[localOld[NUM_MATRICES - matrix] % 0x100][col] >> stage | font[localNew[NUM_MATRICES - matrix] % 0x100][col] << (8 - stage);
				}
								
				// Don't use SendData since this would set the values for each matrix individually
				TxBuffer[TxBufferIndex] = col+1;
				TxBufferIndex++;
				TxBuffer[TxBufferIndex] = columnDataChr;
				TxBufferIndex++;
			}
			flush();
		}
		delay(delayTime);
	}
}

void MAX7219Matrix::displayMessage(char *message, const char font[256][8])
{
	char localMessage[9];
	strncpy(localMessage, message, 8);
	
	int col, matrix, mLen,i;
	char columnDataChr;
	
	mLen = strlen(localMessage);
	for(i=mLen;i<8;i++) {
		localMessage[i] = ' ';
	}
	localMessage[8] = '\0';
	
	for(col = 0; col < 8; col++) {
		for(matrix = NUM_MATRICES; matrix > 0; matrix--) {
		    columnDataChr = font[localMessage[NUM_MATRICES - matrix] % 0x100][col];
		    						
			// Don't use SendData since this would set the values for each matrix individually
			TxBuffer[TxBufferIndex] = col+1;
			TxBufferIndex++;
			TxBuffer[TxBufferIndex] = columnDataChr;
			TxBufferIndex++;
		}
		flush();
	}
	
}
