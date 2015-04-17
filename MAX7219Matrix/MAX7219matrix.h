/*
 * max7219matrix.h
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
 
class MAX7219Matrix
{
	public:
	static MAX7219Matrix& instance()
    {
       static MAX7219Matrix _instance;
       return _instance;
    }
	~MAX7219Matrix();
	void wait(long);
	void brightness(char);
	void clearAll();
	void init();
	void sendLetter(int, char, const char [256][8]);
	void scrollMessageHoriz(char *, int, double, int, const char [256][8]);
	void scrollMessageVert(char *, char *, double, int, const char [256][8]);
	void displayMessage(char *, const char [256][8]);
	void reset();
	
	private:
	MAX7219Matrix();
	MAX7219Matrix( const MAX7219Matrix& );
    MAX7219Matrix & operator = (const MAX7219Matrix &);
	int SpiFd; // File descriptor of spi port
	char *TxBuffer;
	char *RxBuffer;
	char *PadString;
	int TxBufferIndex;
	int RxBufferIndex;
		
	void SendData(char, char, char);
	void SendData(char, char);
	void flush();
	void begin(int, int);
	void scrollTextOnce(char *, double, int, const char [256][8]);
};
