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
#include "max7219matrix.h"
#include "MAX7219fonts.h"
#include "MAX7219constants.h"

using namespace std;

int main(int argc, char **argv)
{
	MAX7219Matrix m7219;
	
	m7219.init();
	m7219.sendLetter(1,'P',CP437_FONT);
	m7219.sendLetter(1,'A',CP437_FONT);
	m7219.sendLetter(2,'B',CP437_FONT);
	m7219.sendLetter(3,'C',CP437_FONT);
	m7219.sendLetter(4,'D',CP437_FONT);
	m7219.sendLetter(5,'E',CP437_FONT);
	m7219.sendLetter(6,'F',CP437_FONT);
	m7219.sendLetter(7,'G',CP437_FONT);
	m7219.sendLetter(8,'H',CP437_FONT);
	m7219.sendLetter(9,'P',CP437_FONT);
	
	m7219.wait(5000);
	
	m7219.clearAll();
	
	char message[] = "Hallo Welt, und weiter geht's! Und noch ein Satz.";
	
	m7219.scrollMessageHoriz(message, 0, 3.0, DIR_L, CP437_FONT, 0);
	
	m7219.wait(10000);
	
	m7219.clearAll();
	
	return 0;
}

