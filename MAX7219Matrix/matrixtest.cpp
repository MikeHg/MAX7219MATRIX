/*
 * matrixtest.cpp
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


#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "MAX7219matrix.h"
#include "MAX7219fonts.h"
#include "MAX7219constants.h"

using namespace std;

int main(int argc, char **argv)
{	
	double speedVal = 3.0;
	int pos = -1;
	
	for(int i=0;i<argc;i++) 
	{
		if(strcmp (argv[i],"-speed") == 0)
		{
			pos = i;
			pos++;
		}
	}
	
	if(pos < argc)
	{
		speedVal = atof(argv[pos]);
		if(speedVal < 0.5) speedVal = 0.5;
		if(speedVal > 90.0) speedVal = 90.0;
	}
	
	MAX7219Matrix::instance().init();
	
	MAX7219Matrix::instance().sendLetter(0,'P',CP437_FONT);
	MAX7219Matrix::instance().sendLetter(1,'H',CP437_FONT);
	MAX7219Matrix::instance().sendLetter(2,'G',CP437_FONT);
	MAX7219Matrix::instance().sendLetter(3,'F',CP437_FONT);
	MAX7219Matrix::instance().sendLetter(4,'E',CP437_FONT);
	MAX7219Matrix::instance().sendLetter(5,'D',CP437_FONT);
	MAX7219Matrix::instance().sendLetter(6,'C',CP437_FONT);
	MAX7219Matrix::instance().sendLetter(7,'B',CP437_FONT);
	MAX7219Matrix::instance().sendLetter(8,'A',CP437_FONT);
	MAX7219Matrix::instance().sendLetter(9,'P',CP437_FONT);
	
	MAX7219Matrix::instance().wait(2000);
	
	MAX7219Matrix::instance().clearAll();
	
	char message[] = "    Hallo Welt, und weiter geht's! Und noch ein Satz.    ";
	
	MAX7219Matrix::instance().scrollMessageHoriz(message, 2, speedVal, DIR_L, CP437_FONT);
	
	MAX7219Matrix::instance().wait(2000);
	
	char oldMessage[] ="Hello";
	char newMessage[] ="World";
	
	MAX7219Matrix::instance().displayMessage(oldMessage, CP437_FONT);
	MAX7219Matrix::instance().wait(2000);
	MAX7219Matrix::instance().scrollMessageVert(oldMessage, newMessage, speedVal, DIR_U, CP437_FONT);
	
	MAX7219Matrix::instance().wait(5000);
	
	MAX7219Matrix::instance().clearAll();
	
	return 0;
}

