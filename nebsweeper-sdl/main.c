/*
 * NebSweeper SDL - SDL Client for NebSweeper
 * NebSweeper SDL Copyright (C) 2010, 2011 Flávio Zavan
 *
 * This file is part of NebSweeper SDL.
 *
 * NebSweeper SDL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NebSweeper SDL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with NebSweeper SDL.  If not, see <http://www.gnu.org/licenses/>.
 *
 * flavio [AT] nebososo [DOT] com
 * http://www.nebososo.com
*/

#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#ifdef __APPLE__
    #include <unistd.h>
    #include <select.h>
#endif

#ifdef WIN32

#define _WIN32_WINNT 0x501

    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <sys/unistd.h>
#endif

#include "defs.h"
#include "screen.h"
#include "networking.h"

int main(int argc, char *argv[]){
    //Check arguments
    if(argc < 2 || argc > 4){
        fprintf(stderr, "Usage: %s NAME [SERVERIP] [PORT]\n", argv[0]);
        fprintf(stderr, "By default SERVERIP=%s and PORT=%u.\n",
            DEFAULTIP, DEFAULTPORT);
        exit(1);
    }
    
    unsigned short port = DEFAULTPORT;
    //p for current players
    unsigned char i, n, players, bombs, id, mines, p;
    int sock;
    struct data *pData = NULL;

#ifdef WIN32
    WSADATA wsaData;

    if(WSAStartup(MAKEWORD(2, 0), &wsaData)){
        fprintf(stderr, "WSAStartup() failed.\n");
        exit(10);
    }
#endif

    //Validate port
    if(argc == 4 && !(port = strtoul(argv[3], NULL, 0))){
        fprintf(stderr, "Invalid port.\n");
        exit(2);
    }

    //Check if it's a 7 bit name
    for(i = 0; argv[1][i]; i++){
        if(argv[1][i] < 32 || argv[1][i] > 127){
            fprintf(stderr,
                "Invalid name, only 7-bit characters are allowed.\n");
            exit(9);
        }
    }

    //Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO)){
        fprintf(stderr, "Unable to initialize SDL.\n");
        exit(3);
    }
    SDL_EnableUNICODE(1);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    //Initialize the screen
    SDL_Surface *screen;
    if(!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_SWSURFACE))){
        SDL_Quit();
        fprintf(stderr, "Could not create screen.\n");
        exit(10);
    }
    SDL_WM_SetCaption("NebSweeper SDL", NULL);

    //Load data
    SDL_Surface *tilesT, *fontT, *tiles, *font;
    if(!(tilesT = IMG_Load(TILES))){
        SDL_Quit();
        fprintf(stderr, "Could not load the tiles.\n");
        exit(11);
    }
    if(!(fontT = IMG_Load(FONT))){
        SDL_Quit();
        fprintf(stderr, "Could not load the font.\n");
        exit(12);
    }
    //Convert
    font = SDL_DisplayFormat(fontT);
    tiles = SDL_DisplayFormat(tilesT);
    SDL_FreeSurface(tilesT);
    SDL_FreeSurface(fontT);
    //Set the font and tiles alfa
    SDL_SetColorKey(font, SDL_SRCCOLORKEY | SDL_RLEACCEL,
        SDL_MapRGB(font->format, MAGENTA));
    SDL_SetColorKey(tiles, SDL_SRCCOLORKEY,
        SDL_MapRGB(tiles->format, MAGENTA));

    //Draw the basic interface
    initializeScreen(screen, tiles, font);
    addString(screen, font, "Connecting...", SDL_MapRGB(screen->format, WHITE));
    SDL_Flip(screen);

    //Try to connect
    i = startSocket(&sock, (argc >= 3)? (argv[2]) : (DEFAULTIP), port);
    if(i){
        SDL_Quit();
        switch(i){
            case 1:
                fprintf(stderr, "The socket could not be created.\n");
                exit(4);

            case 2:
                fprintf(stderr, "Unable to resolv %s.\n",
                    argc >= 3? argv[2] : DEFAULTIP);
                exit(5);

            default:
                fprintf(stderr, "Unable to connect to %s.\n",
                    argc >= 3? argv[2] : DEFAULTIP);
                exit(6);
        }
    }

    //Send name and retrieve basic info from the server
    p = exchangeBasics(sock, &players, &bombs, &mines, &id,
        argv[1], &pData);

    //Lost connection
    if(!p){
        SDL_Quit();
        fprintf(stderr, "Lost connection to the server.\n");
        exit(7);
    }

    //Copy own name
    for(i = 0; argv[1][i] && i < 16; i++){
        pData[id].name[i] = argv[1][i];
    }
    pData[id].name[i] = 0;
    //Update own stats
    pData[id].stats = 1;

    //Initialize the field
    unsigned char field[16][16];

    for(i = 0; i < 16; i++){
        for(n = 0; n < 16; n++){
            field[i][n] = 10;
        }
    }

    //Initialize the board
    addString(screen, font, "Connected...", SDL_MapRGB(screen->format, WHITE));
    initializeBoard(screen, font, pData, bombs, id, mines, players);
    SDL_Flip(screen);

    //Play
    p = game(screen, tiles, font, sock, id, players,
            bombs, mines, pData, field);

    shutdown(sock, 2);
#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif

    if(!p){
        showWinner(screen, font, pData, players);
    }

    //Close SDL
    SDL_FreeSurface(tiles);
    SDL_FreeSurface(font);
    SDL_FreeSurface(screen);
    SDL_Quit();

    if(p == 1){
        fprintf(stderr, "Lost connection to the server.\n");
    }
    
    //Free the memory
    free(pData);

    return (p == 2)? (13) : (0);
}
