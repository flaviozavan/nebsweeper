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

#ifdef __APPLE__
    #include <unistd.h>
    //#include <select.h>
#endif

#ifdef WIN32

#define _WIN32_WINNT 0x501

    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <sys/types.h>
    #include <sys/unistd.h>
    #include <netdb.h>
#endif

#include <SDL/SDL.h>
#include "defs.h"
#include "networking.h"
#include "screen.h"

int startSocket(int *sock, const char *host, unsigned short port){

    unsigned char i;
    struct addrinfo *result;
    struct sockaddr_in sAddr;
    
    //Initialize the socket
    if((*sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        return 1;
    }

    //Resolve
    if(getaddrinfo(host, NULL, NULL, &result)){
        return 2;
    }

    //Copy the ip to sAddr
    sAddr.sin_addr.s_addr =
        ((struct sockaddr_in *) result->ai_addr)->sin_addr.s_addr;
    //Free the memory
    freeaddrinfo(result);

    //Set the rest of the structure
    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons(port);
    for(i = 0; i < sizeof(sAddr.sin_zero); i++){
        sAddr.sin_zero[i] = 0;
    }

    //Connect
    if(connect(*sock, (struct sockaddr *) &sAddr, sizeof(sAddr))){
        return 3;
    }

    return 0;
}

int exchangeBasics(int sock, unsigned char *players, unsigned char *bombs,
    unsigned char *mines, unsigned char *id, char *name, struct data **pData){

    //Return 0 for failure, or the number of players including you

    char buffer[17];
    unsigned char i, n, t, p, len;

    //Measure the name and copy to the buffer
    for(i = 0; name[i]; i++){
        buffer[i+1] = name[i];
    }
    buffer[0] = i;

    //Send the name
    if(send(sock, buffer, i + 1, 0) <= 0){
        return 0;
    }

    //Receive data
    for(i = 0; i < 6; i += len){
        len = recv(sock, buffer + i * sizeof(char), 6 - i, 0);
        if(len <= 0){
            return 0;
        }
    }

    //Set the data
    *players = buffer[1];
    *mines = buffer[2];
    *bombs = buffer[3];
    *id = buffer[4];
    p = buffer[5];

    //Allocate the structs
    if(!(*pData = (struct data *) malloc(sizeof(struct data) * *players))){
        return 0;
    }

    //Reset the stats, score and the null byte
    for(i = 0; i < *players; i++){
        (*pData)[i].stats = 0;
        (*pData)[i].score = 0;
        (*pData)[i].name[16] = 0;
    }

    //Receive names
    //i = 1 because p includes you
    for(i = 1; i < p; i++){
        //Get the length
        len = recv(sock, buffer, 1, 0);
        if(len <= 0){
            return 0;
        }
        //Get the data
        for(n = 0; n < buffer[0]; n += len){
            len = recv(sock, buffer + (1 + n) * sizeof(char),
                buffer[0] - n, 0);
            if(len <= 0){
                return 0;
            }
        }
        t = buffer[1];
        //Copy name
        for(n = 2; n < buffer[0] + 1; n++){
            (*pData)[t].name[n - 2] = buffer[n];
        }
        (*pData)[t].name[n - 2] = 0;

        //Update the stats
        (*pData)[t].stats = 1;
    }

    return p;
}

int game(SDL_Surface *screen, SDL_Surface *tiles, SDL_Surface *font,
    int sock, unsigned char id, unsigned char players,
    unsigned char bombs, unsigned char mines, struct data *pData,
    unsigned char field[16][16]){

    unsigned char i, toReceive, toDiscard, chatLen;
    int y, x;
    unsigned char msgLen, len, isChatting, turn, bombing;
    SDL_Event event;
    //255-64+1
    char buffer[192];
    //65 to fit a 0 at the end before printing
    char chatBuffer[65];
    char line[81];

    fd_set set;

    //Initialize what we have to
    isChatting = 0;
    toReceive = 0;
    toDiscard = 0;
    chatLen = 2;
    bombing = 0;
    msgLen = 0;
    turn = players + 1;
    chatBuffer[1] = 't';
    y = 7;
    x = 7;

    struct timeval timeVal;
    timeVal.tv_sec = 0;
    timeVal.tv_usec = 10000;

    //Main loop
    while(mines){
        //Build the set
        FD_ZERO(&set);
        FD_SET(sock, &set);

        //Check for input
        if(SDL_PollEvent(&event)){
            //User quit
            if(event.type == SDL_QUIT){
                return 2;
            }
            //Mouse moving
            else if(turn == id && event.type == SDL_MOUSEMOTION){
                updateCursor(screen, tiles, &y, &x, bombing, bombing,
                    event.motion.y / TILE_HEIGHT,
                    (event.motion.x - BOARD_WIDTH) / TILE_WIDTH);
                
                SDL_Flip(screen);
            }
            //Click
            else if(event.type == SDL_MOUSEBUTTONDOWN && turn == id){
                //Right click
                if(event.button.button == SDL_BUTTON_RIGHT && bombs){
                    updateCursor(screen, tiles, &y, &x, bombing, !bombing,
                        event.motion.y / TILE_HEIGHT,
                        (event.motion.x - BOARD_WIDTH) / TILE_WIDTH);

                    bombing ^= 1;

                    SDL_Flip(screen);
                }
                //Always update
                else {
                    updateCursor(screen, tiles, &y, &x, bombing, bombing,
                        event.motion.y / TILE_HEIGHT,
                        (event.motion.x - BOARD_WIDTH) / TILE_WIDTH);

                    //Only work if it's hovering the field
                    if(event.button.button == SDL_BUTTON_LEFT &&
                        y >= 0 && y <= 15 && x >= 0 && x <= 15){

                        //Sweep
                        if(!bombing){
                            buffer[0] = 2;
                            buffer[1] = 's';
                            buffer[2] = (y << 4) | (x & 15);
                            send(sock, buffer, 3, 0);

                            clearCursor(screen, tiles, y, x, bombing);

                            SDL_Flip(screen);
                        }
                        else {
                            buffer[0] = 2;
                            buffer[1] = 'b';
                            buffer[2] = (y << 4) | (x & 15);
                            send(sock, buffer, 3, 0);

                            sprintf(line, "Bombs: %3d", --bombs);
                            printString(screen, font, line, id * 2 + 1,
                                0, getPlayerColor(screen, turn));

                            clearCursor(screen, tiles, y, x, bombing);

                            bombing = 0;

                            SDL_Flip(screen);
                        }
                    }
                }
            }
            else if(event.type == SDL_KEYDOWN &&
                event.key.state == SDL_PRESSED){

                //Send the message if the handler says it's OK
                if(keyboardHandler(screen, font, &chatLen, &isChatting,
                    id, chatBuffer, &event)){

                    //Set the size of the message and send it
                    chatBuffer[0] = chatLen - 1;
                    send(sock, chatBuffer, chatLen, 0);
                    //Cleanup
                    isChatting = 0;
                    chatLen = 2;
                    SDL_Flip(screen);
                }
                SDL_Flip(screen);
            }
        }

        //Check the socket and sleep a bit
        switch(select(FD_SETSIZE, &set, NULL, NULL, &timeVal)){
            case -1:
                return 1;

            case 0:
                continue;
        }


        //Receive data
        if(FD_ISSET(sock, &set)){
            //Receive if we can
            if(toReceive){
                len = recv(sock, buffer + (msgLen - toReceive) * sizeof(char),
                    toReceive, 0);

                if(len > 0){
                    toReceive -= len;
                    if(toReceive){
                        continue;
                    }
                }
            }
            //Discard if we have to
            else if(toDiscard){
                len = recv(sock, buffer, toDiscard, 0);
                if(len > 0){
                    toDiscard -= len;
                    continue;
                }
            }
            //Get the length of the new message
            else {
                len = recv(sock, buffer, 1, 0);
                //Get the length if we didn't lose connection
                if(len > 0){
                    //Adjust the values
                    if((unsigned char) buffer[0] > 64){
                        msgLen = 64;
                        toReceive = 64;
                        toDiscard = (unsigned char) buffer[0] - 64;
                    }
                    else {
                        msgLen = (unsigned char) buffer[0];
                        toReceive = (unsigned char) buffer[0];
                    }
                    continue;
                }
            }
            //Lost connection
            if(len <= 0){
                return 1;
            }

            //If we got down here, we're ready to parse

            //Chat
            if(buffer[0] == 't'){
                //Null terminate
                buffer[msgLen] = 0;

                sprintf(line, "%s: %s",
                    pData[(int) buffer[1]].name, buffer + 2);

                addString(screen, font,
                    line, getPlayerColor(screen, (int) buffer[1]));

                SDL_Flip(screen);
            }
            //New player
            else if(buffer[0] == 'c'){
                //Copy name
                for(i = 2; i < msgLen; i++){
                    //Make it 7-bit
                    if(buffer[i] < 32 || buffer[i] > 126){
                        buffer[i] = '?';
                    }
                    pData[(int) buffer[1]].name[i - 2] = buffer[i];
                }

                //Null terminate
                pData[(int) buffer[1]].name[i-2] = 0;

                //Update the stats
                pData[(int) buffer[1]].stats = 1;

                //Add player to the left panel
                addPlayer(screen, font,
                    pData[(int) buffer[1]].name, (unsigned char) buffer[1]);

                //Print
                sprintf(line, "%s joined the game.",
                    pData[(int) buffer[1]].name);
                addString(screen, font, line,
                    getPlayerColor(screen, (unsigned char) buffer[1]));

                SDL_Flip(screen);
            }
            //Player left
            else if(buffer[0] == 'l'){
                //Update the stats
                pData[(int) buffer[1]].stats = 0;

                //Remove from the window
                deletePlayer(screen, font, (unsigned char) buffer[1]);

                //Print
                sprintf(line, "%s left the game.",
                    pData[(int) buffer[1]].name);
                addString(screen, font, line,
                    getPlayerColor(screen, (unsigned char) buffer[1]));

                SDL_Flip(screen);
            }
            //New turn
            else if(buffer[0] == 'v'){
                //Update the stats
                turn = buffer[1];

                //Remove from the window
                sprintf(line, "%s's turn.",
                    pData[(int) buffer[1]].name);
                addString(screen, font, line,
                    getPlayerColor(screen, (unsigned char) buffer[1]));

                //Hide the cursor

                bombing = 0;

                SDL_Flip(screen);
            }
            //Player won
            else if(buffer[0] == 'w'){
                return 0;
            }
            //Reveal
            else if(buffer[0] == 'r'){
                for(i = 1; i < msgLen; i += 2){
                    //Update the matrix
                    field[(buffer[i] & 240) >> 4][buffer[i] & 15] =
                        buffer[i+1];

                    //Update the screen
                    setFieldValue(screen, tiles, (buffer[i] & 240) >> 4,
                        buffer[i] & 15, buffer[i+1], turn);

                    //Set the player and update the score
                    if(buffer[i+1] > 8){
                        field[(buffer[i] & 240) >> 4][buffer[i] & 15] |=
                            turn << 4;

                        pData[turn].score++;
                        
                        sprintf(line, "%3d", --mines);
                        printString(screen, font, line,
                            BOARD_HEIGHT/FONT_HEIGHT - 1, 0,
                            SDL_MapRGB(screen->format, RED));
                    }
                }
                //Update the score on the screen
                sprintf(line, "%3d", pData[turn].score);
                printString(screen, font, line, turn * 2,
                    BOARD_WIDTH/FONT_WIDTH - 4, getPlayerColor(screen, turn));

                SDL_Flip(screen);
            }
        }
    }

    return 0;
}
