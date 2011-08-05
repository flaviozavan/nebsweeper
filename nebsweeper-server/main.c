/*
 * NebSweeper Server - A server for NebSweeper
 * NebSweeper Server Copyright (C) 2010, 2011 Flávio Zavan
 *
 * This file is part of NebSweeper Server.
 *
 * NebSweeper Server is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NebSweeper Server is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with NebSweeper Server.  If not, see <http://www.gnu.org/licenses/>.
 *
 * flavio [AT] nebososo [DOT] com
 * http://www.nebososo.com
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifdef WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <sys/types.h>
    #include <sys/unistd.h>
#endif

#include "game.h"
#include "networking.h"

#define VERSION "1.0 RC8"

int main(int argc, char *argv[]){
    //Force line buffer
    setlinebuf(stdout);
    setlinebuf(stderr);

    //Check arguments
    if(argc < 2 || argc > 5){
        fprintf(stderr, "Usage: %s PLAYERS [MINES] [BOMBS] [PORT]\n", argv[0]);
        fprintf(stderr, "By default MINES=%u and BOMBS=%u.\n",
            DEFAULTMINES, DEFAULTBOMBS);
        fprintf(stderr, "The default port is %u.\n", DEFAULTPORT);
        exit(1);
    }

    unsigned short port = DEFAULTPORT;
    unsigned char players, i, n;
    unsigned char mines = DEFAULTMINES;
    unsigned char bombs = DEFAULTBOMBS;

    //Validate them
    players = strtoul(argv[1], NULL, 0);
    if(players < 2 || players > 6){
        fprintf(stderr, "Invalid number of players.\n");
        fprintf(stderr, "Use a number between 2 and %u.\n", MAXPLAYERS);
        exit(2);
    }

    if(argc > 2){
        if(!(mines = strtoul(argv[2], NULL, 0))){
            fprintf(stderr, "Invalid number of mines.\n");
            exit(3);
        }
    }

    if(argc > 3){
        bombs = strtoul(argv[3], NULL, 0);
    }

    if(argc == 5){
        if(!(port = strtoul(argv[4], NULL, 0))){
            fprintf(stderr, "Invalid port.\n");
            exit(3);
        }
    }

    //Start the server
    int sSock, *cSock;
    struct sockaddr_in sAddr;
    struct sockaddr_in *cAddr;
    struct data *pData;
#ifdef WIN32
    WSADATA wsaData;

    if(WSAStartup(MAKEWORD(2, 0), &wsaData)){
        fprintf(stderr, "WSAStartup() failed.\n");
        exit(4);
    }
#endif

    switch(startServer(&sSock, port, &sAddr, players)){
        //Fallthrough
        case 1:
            fprintf(stderr, "The socket could not be created.\n");
        case 2:
            fprintf(stderr, "Unable to bind.\n");
        case 3:
            fprintf(stderr, "Unable to listen on port %u.\n", port);
            exit(4);
        //No error returned
        default:
            printf("NebSweeper Server %s\n", VERSION);
            printf("Copyright (C) 2010, 2011 Flávio Zavan\n");
            printf("This program comes with ABSOLUTELY NO WARRANTY;\n");
            printf("This is free software, and you are welcome to ");
            printf("redistribute it under \ncertain conditions; ");
            printf("If you did not receive of copy of the\n");
            printf("GNU General Public License, ");
            printf("see <http://www.gnu.org/licenses/>.\n\n");
            printf("%u player, %u mine and %u bomb game created.\n",
                players, mines, bombs);
            printf("Server listening on port %u.\n", port);
            printf("Waiting for players to connect.\n");
            break;
    }

    //Allocate the needed structures
    if(!(cAddr =
        (struct sockaddr_in *) malloc(players * sizeof(struct sockaddr_in))) ||
        !(pData = (struct data *) malloc(players * sizeof(struct data))) ||
        !(cSock = (int *) malloc(players * sizeof(int)))){

        fprintf(stderr, "Out of memory.\n");
        exit(16);
    }

    //Seed
    srand(time(NULL));

    //Initialize the game
    unsigned char field[16][16];
    initialize(players, mines, bombs, pData, field);
    
    //Wait for players
    if(waitForPlayers(&sSock, cSock, cAddr, pData, players, mines, bombs)){
        fprintf(stderr, "Select failed.\n");
    }

    if(!play(cSock, pData, players, mines, field)){
        //Skip this part if the game ended because a player left
        i = 0;
        for(n = 1; n < players; n++){
            if(pData[n].score > pData[i].score){
                i = n;
            }
        }
        printf("%s won.\n", pData[i].name);
    }

    //Free the memory and sockets
    for(i = 0; i < players; i++){
        shutdown(cSock[i], 2);
#ifdef WIN32
        closesocket(cSock[i]);
#else
        close(cSock[i]);
#endif
    }

    shutdown(sSock, 2);
#ifdef WIN32
    closesocket(sSock);
#else
    close(sSock);
#endif
    free(cAddr);
    free(pData);
    free(cSock);

    return 0;
}
