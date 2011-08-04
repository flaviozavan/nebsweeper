CC=gcc
CFLAGS=-O2 -Wall

all: nebsweeper-sdl

clean:
	rm -f networking.o game.o main.o screen.o nebsweeper-sdl{,.exe}

nebsweeper-sdl: main.o networking.o screen.o
	$(CC) $(CFLAGS)   -o nebsweeper-sdl main.o networking.o screen.o \
        -lSDL -lSDL_image

windows: networking.o main.o screen.o
	$(CC) $(CFLAGS)   -o nebsweeper-curses main.o networking.o \
		screen.o -lpdcurses -lws2_32

main.o: main.c 

networking.o: networking.c

screen.o: screen.c
