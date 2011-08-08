CC=gcc
CFLAGS=-O2 -Wall

all: nebsweeper-curses

clean:
	rm -f networking.o game.o main.o screen.o \
        nebsweeper-curses nebsweeper-curses.exe

nebsweeper-curses: networking.o main.o screen.o
	$(CC) $(CFLAGS)   -o nebsweeper-curses main.o networking.o \
		screen.o -lncurses

windows: networking.o main.o screen.o
	$(CC) $(CFLAGS)   -o nebsweeper-curses main.o networking.o \
		screen.o -lpdcurses -lws2_32

main.o: main.c 

networking.o: networking.c 

screen.o: screen.c 

game.o: game.c 
