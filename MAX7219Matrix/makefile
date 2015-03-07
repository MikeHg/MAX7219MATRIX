# 
CC=g++
# 
CFLAGS=-Wall -lwiringPi

all: MAX7219matrix
	ls -lh --color=auto

run: MAX7219matrix
	ls -lh --color=auto
	./matrixtest -speed 3.0

MAX7219matrix: MAX7219matrix.cpp matrixtest.cpp
	$(CC) $(CFLAGS) MAX7219matrix.cpp matrixtest.cpp -o matrixtest

clean:
	rm -rf *o matrixtest
	ls -lh --color=auto
