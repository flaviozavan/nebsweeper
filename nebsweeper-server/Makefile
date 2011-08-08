CC=gcc
CFLAGS=-O2 -Wall

all: nebsweeper-server

clean:
	rm -f networking.o game.o main.o \
        nebsweeper-server nebsweeper-server.exe

nebsweeper-server: networking.o game.o main.o
	$(CC) $(CFLAGS)   -o nebsweeper-server networking.o main.o game.o

windows: networking.o game.o main.o
	$(CC) $(CFLAGS)   -o nebsweeper-server networking.o main.o game.o -lws2_32

main.o: main.c 

networking.o: networking.c 

game.o: game.c 
