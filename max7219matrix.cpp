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
#include "max7219matrix.h"
#include "MAX7219constants.h"
#include "MAX7219fonts.h"
#include <iostream>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <math.h>
#include <string.h>


using namespace std;

/******************************************************************************
***   Defines and Constants                                                 ***
******************************************************************************/

MAX7219Matrix::MAX7219Matrix() {
	TxBuffer = new char[1024]; // Buffer for TxData
	RxBuffer = new char[1024]; // Buffer for RxData
	
	TxBufferIndex = 0;
	RxBufferIndex = 0;
}

MAX7219Matrix::~MAX7219Matrix() {
	clearAll();
	
	delete[] TxBuffer;
	delete[] RxBuffer;
	
	close(SpiFd); // Close SPI port
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
    //gfx_set_all(GFX_OFF);               // clear the graphics buffer
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
 *  Subroutine used by scroll_message_horiz(), scrolls text once across the array, starting & ending with test on the array
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
		for(startChar = length-1; startChar > -1; startChar--) {
		}
	}
	
	/*
	start_range = []
    if direction == DIR_L:
        start_range = range(length)
    elif direction == DIR_R:
        start_range = range(length-1, -1, -1)
    for start_char in start_range:
        for stage in range(8):
            for col in range(8):
                column_data = []
                for matrix in range(NUM_MATRICES-1, -1, -1):
                    if direction == DIR_L:
                        this_char = font[ord(text[start_char + NUM_MATRICES - matrix - 1])]
                        next_char = font[ord(text[start_char + NUM_MATRICES - matrix])]
                        if col+stage < 8:
                            column_data += [col+1, this_char[col+stage]]
                        else:
                            column_data += [col+1, next_char[col+stage-8]]
                    elif direction == DIR_R:
                        this_char = font[ord(text[start_char + NUM_MATRICES - matrix])]
                        next_char = font[ord(text[start_char + NUM_MATRICES - matrix - 1])]
                        if col >= stage:
                            column_data += [col+1, this_char[col-stage]]
                        else:
                            column_data += [col+1, next_char[col-stage+8]]
                send_bytes(column_data)
            time.sleep(delay)
	
	*/
}

/*
 *  Scroll a text message across the array, for a specified (repeats) number of times
 *  repeats=0 gives indefinite scrolling until script is interrupted
 *  speed: 0-9 for practical purposes; speed does not have to integral
 *  direction: DIR_L or DIR_R only; DIR_U & DIR_D will do nothing
 *  finish: True/False - True ensures array is clear at end, False ends with the last column of the last character of message
 *           still displayed on the array - this is included for completeness but rarely likely to be required in practice
 *  Scrolling starts with message off the RHS(DIR_L)/LHS(DIR_R) of array, and ends with message off the LHS/RHS
 *  If repeats>1, add space(s) at end of 'message' to separate the end of message & start of its repea
 */
void MAX7219Matrix::scrollMessageHoriz(char *message, int repeats, double speed, int direction, const char font[256][8], int finish) {
	
	double delayTime = pow(0.5, speed) * 1000.0;
	scrollTextOnce(message, delayTime, direction, font);
	
	/*
    if repeats <= 0:
        indef = True
    else:
        indef = False
        repeats = int(repeats)
        
    if len(message) < NUM_MATRICES:
        message = trim(message)
    # Repeatedly scroll the whole message (initially 'front-padded' with blanks) until the last char appears
    scroll_text = ""
    if direction == DIR_L:
        scroll_text = PAD_STRING + message
    elif direction == DIR_R:
        scroll_text = message + PAD_STRING
    counter = repeats
    while (counter > 0) or indef:
        scroll_text_once(scroll_text, delay, direction, font)
        # After the first scroll, replace the blank 'front-padding' with the start of the same message
        if counter == repeats:
            if direction == DIR_L:
                scroll_text = message[-NUM_MATRICES:] + message
            elif direction == DIR_R:
                scroll_text = message + message[:NUM_MATRICES]
        counter -= 1
    # To finish, 'end-pad' the message with blanks and scroll the end of the message off the array
    if direction == DIR_L:
        scroll_text = message[-NUM_MATRICES:] + PAD_STRING
    elif direction == DIR_R:
        scroll_text = PAD_STRING + message[:NUM_MATRICES]
    scroll_text_once(scroll_text, delay, direction, font)
    # Above algorithm leaves the last column of the last character displayed on the array, so optionally erase it
    if finish:
        clear_all()
    */
}

