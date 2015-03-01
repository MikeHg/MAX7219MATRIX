# 
CC=g++
# 
CFLAGS=-Wall -lwiringPi

all: MAX7219matrix
	ls -lh --color=auto

run: MAX7219matrix
	ls -lh --color=auto
	./matrixtest

MAX7219matrix: max7219matrix.cpp matrixtest.cpp
	$(CC) $(CFLAGS) max7219matrix.cpp matrixtest.cpp -o matrixtest

clean:
	rm -rf *o matrixtest
	ls -lh --color=auto
