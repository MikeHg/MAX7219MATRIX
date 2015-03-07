/*
 * MAX7219constants.h
 * 
 * Copyright 2015  <pi@FamHaugkPi2>
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
 
/******************************************************************************
 ***   Defines and Constants                                                ***
 ******************************************************************************/

// IMPORTANT: User must specify the number of MAX7219 matrices here:
const int NUM_MATRICES = 8;   //Number of separate MAX7219 matrices

// Scroll & wipe directions, for use as arguments to various library functions
const int DIR_U      = 1;  // Up
const int DIR_R      = 2;  // Right
const int DIR_D      = 4;  // Down
const int DIR_L      = 8;  // Left
const int DIR_RU     = 3;  // Right & up diagonal scrolling for gfx_scroll() function only
const int DIR_RD     = 6;  // Right & down diagonal scrolling for gfx_scroll() function only
const int DIR_LU     = 9;  // Left & up diagonal scrolling for gfx_scroll() function only
const int DIR_LD     = 12; // Left & down diagonal scrolling for gfx_scroll() function only
const int DISSOLVE   = 16; // Pseudo-random fade transition for wipe_message() function only
const int GFX_OFF    = 0;  // Turn the relevant LEDs off, or omit (don't draw) the endpoint of a line
const int GFX_ON     = 1;  // Turn the relevant LEDs on, or include (draw) the endpoint of a line
const int GFX_INVERT = 2;  // Invert the state of the relevant LEDs

// MAX7219 memory register addresses according to datasheet
const char MAX7219_REG_NOOP        = 0x00;
const char MAX7219_REG_DIGIT0      = 0x01;
const char MAX7219_REG_DIGIT1      = 0x02;
const char MAX7219_REG_DIGIT2      = 0x03;
const char MAX7219_REG_DIGIT3      = 0x04;
const char MAX7219_REG_DIGIT4      = 0x05;
const char MAX7219_REG_DIGIT5      = 0x06;
const char MAX7219_REG_DIGIT6      = 0x07;
const char MAX7219_REG_DIGIT7      = 0x08;
const char MAX7219_REG_DECODEMODE  = 0x09;
const char MAX7219_REG_INTENSITY   = 0x0A;
const char MAX7219_REG_SCANLIMIT   = 0x0B;
const char MAX7219_REG_SHUTDOWN    = 0x0C;
const char MAX7219_REG_DISPLAYTEST = 0x0F;
