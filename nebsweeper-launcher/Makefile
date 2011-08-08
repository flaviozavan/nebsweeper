CC=gcc
CFLAGS=-Wall -O2 `pkg-config --libs --cflags gtk+-2.0` -Wl,--export-dynamic

all: nebsweeper-launcher

clean:
	rm -f signals.o main.o \
        nebsweeper-launcher nebsweeper-launcher.exe

main.o: main.c

signals.o: signals.c

nebsweeper-launcher: main.o signals.o
	$(CC) $(CFLAGS) signals.c main.c -o nebsweeper-launcher
