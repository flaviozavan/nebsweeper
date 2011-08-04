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
#ifdef WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <sys/types.h>
    #include <sys/unistd.h>
#endif
#ifdef __APLE__
    #include <unistd.h>
    #include <select.h>
#endif

#include "game.h"
#include "networking.h"


int startServer(int *sSock, unsigned short port, struct sockaddr_in *sAddr,
    unsigned char players){
    
    unsigned char i;

    //Create the socket
    if((*sSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        return 1;
    }

    sAddr->sin_family = AF_INET;
    sAddr->sin_port = htons(port);
    sAddr->sin_addr.s_addr = htonl(INADDR_ANY);
    for(i = 0; i < sizeof(sAddr->sin_zero); i++){
        sAddr->sin_zero[i] = 0;
    }
    
    //Bind
    if(bind(*sSock, (struct sockaddr *) sAddr, sizeof(struct sockaddr_in))){
        return 2;
    }

    //Listen
    if(listen(*sSock, players)){
        return 3;
    }

    return 0;
}

int waitForPlayers(int *sSock, int *cSock, struct sockaddr_in *cAddr,
    struct data *pData, unsigned char players, unsigned char mines,
    unsigned char bombs){

    unsigned int size = sizeof(struct sockaddr_in);
    short int len;
    //255 - 64
    char buffer[191];
    unsigned char i;
    short int t;
    fd_set set;
    //Current players with names set
    unsigned char p = 0;
    //Current players
    unsigned char n = 0;
    while(p < players){
        //Build the select list
        FD_ZERO(&set);
        for(i = 0; i < players; i++){
            //Only add if it is connected
            if(pData[i].stats){
                FD_SET(cSock[i], &set);
            }
        }
        FD_SET(*sSock, &set);

        //Wait for connection and receive names
        if(!select(FD_SETSIZE, &set, NULL, NULL, NULL)){
            return 1;
        }

        if(FD_ISSET(*sSock, &set)){
            //Accept only if we're not full
            if(n < players){
                printf("New connection.\n");
                //Find a free slot
                t = -1;
                while(pData[++t].stats & 1);
                cSock[t] = accept(*sSock, (struct sockaddr *) cAddr, &size);
                //Set the connected bit
                pData[t].stats |= 1;
                n++;
            }
            else {
#ifdef WIN32
                closesocket(accept(*sSock, (struct sockaddr *) cAddr, &size));
#else
                close(accept(*sSock, (struct sockaddr *) cAddr, &size));
#endif
            }
        }
        for(i = 0; i < players; i++){
            //Skip if there's nothing on the socket
            if(!(FD_ISSET(cSock[i], &set))){
                continue;
            }

            //Receive if we can
            if(pData[i].toReceive){
                len = recv(cSock[i],
                    pData[i].buffer +
                    (pData[i].len - pData[i].toReceive) * sizeof(char),
                    pData[i].toReceive, 0);

                if(len > 0){
                    pData[i].toReceive -= len;
                    //Receive more if we didn't get the whole thing
                    if(pData[i].toReceive){
                        continue;
                    }
                }
            }
            //Discard if we have to
            else if(pData[i].toDiscard){
                len = recv(cSock[i], buffer, pData[i].toDiscard, 0);
                if(len > 0){
                    pData[i].toDiscard -= len;
                    continue;
                }
            }
            //Get the length of the new message
            else {
                len = recv(cSock[i], buffer, 1, 0);
                //Get the length if the player didn't leave
                if(len > 0){
                    //Adjust the values
                    if((unsigned char) buffer[0] > 64){
                        pData[i].len = 64;
                        pData[i].toReceive = 64;
                        pData[i].toDiscard = (unsigned char) buffer[0] - 64;
                    }
                    else {
                        pData[i].len = (unsigned char) buffer[0];
                        pData[i].toReceive = (unsigned char) buffer[0];
                    }
                    continue;
                }
            }
            //Player left, remove it
            if(len <= 0){
                if(pData[i].stats & 2){
                    printf("%s left.\n", pData[i].name);
                    p--;
                }
                else {
                    printf("Unnamed left.\n");
                }
                
                //Reset Values
                pData[i].toReceive = 0;
                pData[i].toDiscard = 0;

                //Close the socket
                shutdown(cSock[i], 2);
#ifdef WIN32
                closesocket(cSock[i]);
#else
                close(cSock[i]);
#endif
                //Tell the rest of the players
                buffer[0] = 2;
                buffer[1] = 'l';
                buffer[2] = i;
                for(t = 0; t < players; t++){
                    if(t != i && (pData[t].stats & 3) == 3){
                        send(cSock[t], buffer, 3, 0);
                    }
                }
                //Remove player
                pData[i].stats = 0;
                n--;
                continue;
            }

            //If we got down here, we're ready to parse
            
            //Read name or discard
            if(!(pData[i].stats & 2)){
                //If not yet named
                //Copy name
                if(pData[i].len > 16){
                    pData[i].len = 16;
                }
                for(t = 0; t < pData[i].len; t++){
                    pData[i].name[t] = pData[i].buffer[t];
                }
                pData[i].name[pData[i].len] = 0;
                //Update stats
                p++;
                pData[i].namelen = pData[i].len;
                pData[i].stats |= 2;
                printf("Player %u's name is %s.\n", i, pData[i].name);
                //Tell the other players
                sprintf(buffer, "0c0%s", pData[i].name);
                buffer[0] = 2 + pData[i].namelen;
                buffer[2] = i;
                for(t = 0; t < players; t++){
                    if(t != i && (pData[t].stats & 3) == 3){
                        //Not the same, connected and named
                        send(cSock[t], buffer, pData[i].namelen + 3, 0);
                    }
                }
                //Answer with all the info
                //Number of players, mines, bombs, player id, online players
                buffer[0] = 5;
                buffer[1] = players;
                buffer[2] = mines;
                buffer[3] = bombs;
                buffer[4] = i;
                buffer[5] = p;
                send(cSock[i], buffer, 6, 0);
                //Send players info
                for(t = 0; t < players; t++){
                    if(t != i && (pData[t].stats & 3) == 3){
                        sprintf(buffer, "00%s", pData[t].name);
                        buffer[0] = pData[t].namelen + 1;
                        buffer[1] = t;
                        send(cSock[i], buffer, pData[t].namelen + 2, 0);
                    }
                }
            }
            //Player's chatting, retransmit
            else if(pData[i].buffer[0] == 't'){
                //Make tBlabla into ltiBlabla
                //Where i = player id and l = length
                for(t = 1; t < pData[i].len; t++){
                    buffer[2+t] = pData[i].buffer[t];
                }
                buffer[0] = 1 + pData[i].len;
                buffer[1] = 't';
                buffer[2] = i;
                for(t = 0; t < players; t++){
                    if(pData[t].stats & 2){
                        send(cSock[t], buffer, pData[i].len + 2, 0);
                    }
                }
                //Add a zero before printing
                pData[i].buffer[pData[i].len] = 0;
                printf("%s: %s\n", pData[i].name,
                    pData[i].buffer + sizeof(char));
            }
        }
    }
    return 0;
}

int play(int *cSock, struct data *pData, unsigned char players,
    unsigned char mines, unsigned char field[16][16]){
 
    unsigned char turn = rand() % players;
    unsigned char i, n, t, e, swept;
    short int len;
    char buffer[191];
    unsigned char mBuffer[255];
    fd_set set;

    //Tell the players who`s starting
    buffer[0] = 2;
    buffer[1] = 'v';
    buffer[2] = turn;
    for(i = 0; i < players; i++){
        send(cSock[i], buffer, 3, 0);
    }
    printf("%s's turn.\n", pData[turn].name);

    //Main loop, the game only ends when all the mines are found
    while(mines){
        //Build the set
        FD_ZERO(&set);
        for(i = 0; i < players; i++){
            FD_SET(cSock[i], &set);
        }

        //Wait for something to happen
        select(FD_SETSIZE, &set, NULL, NULL, NULL);

        //Check each socket
        for(i = 0; i < players; i++){
            //Skip if there's nothing on the socket
            if(!(FD_ISSET(cSock[i], &set))){
                continue;
            }

            //Receive if we can
            if(pData[i].toReceive){
                len = recv(cSock[i],
                    pData[i].buffer +
                    (pData[i].len - pData[i].toReceive) * sizeof(char),
                    pData[i].toReceive, 0);

                if(len > 0){
                    pData[i].toReceive -= len;
                    //Receive more if we didn't get the whole thing
                    if(pData[i].toReceive){
                        continue;
                    }
                }
            }
            //Discard if we have to
            else if(pData[i].toDiscard){
                len = recv(cSock[i], buffer, pData[i].toDiscard, 0);
                if(len > 0){
                    pData[i].toDiscard -= len;
                    continue;
                }
            }
            //Get the length of the new message
            else {
                len = recv(cSock[i], buffer, 1, 0);
                //Get the length if the player didn't leave
                if(len > 0){
                    //Adjust the values
                    if((unsigned char) buffer[0] > 64){
                        pData[i].len = 64;
                        pData[i].toReceive = 64;
                        pData[i].toDiscard = (unsigned char) buffer[0] - 64;
                    }
                    else {
                        pData[i].len = (unsigned char) buffer[0];
                        pData[i].toReceive = (unsigned char) buffer[0];
                    }
                    continue;
                }
            }
            //Player left
            if(len <= 0){
                printf("%s left.\n", pData[i].name);
                //Close the socket
                shutdown(cSock[i], 2);
#ifdef WIN32
                closesocket(cSock[i]);
#else
                close(cSock[i]);
#endif
                //Tell the rest of the players
                buffer[0] = 2;
                buffer[1] = 'l';
                buffer[2] = i;
                for(n = 0; n < players; n++){
                    if(n != i){
                        send(cSock[n], buffer, 3, 0);
                    }
                }

                //End the game
                return 1;
            }

            //Parse
            if(pData[i].buffer[0] == 't'){
                //Player's chatting, retransmit
                //Make tBlabla into ltiblabla\0
                //Where i = player id and l is the length
                for(n = 1; n < pData[i].len; n++){
                    buffer[n + 2] = pData[i].buffer[n];
                }
                buffer[0] = 1 + pData[i].len;
                buffer[1] = 't';
                buffer[2] = i;
                for(n = 0; n < players; n++){
                    send(cSock[n], buffer, pData[i].len + 2, 0);
                }
                //Add a zero before printing
                pData[i].buffer[pData[i].len] = 0;
                printf("%s: %s\n", pData[i].name,
                    pData[i].buffer + sizeof(char));
            }
            //Player used a bomb
            else if(pData[i].buffer[0] == 'b' && i == turn &&
                pData[i].bombs){

                swept = bomb((pData[i].buffer[1] & 240) >> 4,
                    pData[i].buffer[1] & 15, mBuffer, &mines, pData, i, field);
                //If it was valid, tell the players
                if(swept){
                    printf("%s used a bomb at %u, %u.", pData[i].name,
                        (pData[i].buffer[1] & 240) >> 4,
                        pData[i].buffer[1] & 15);
                    pData[i].bombs--;
                    sendReveal(swept, buffer, mBuffer, cSock, pData,
                        players, field);
                    //Check if he won
                    for(t = 0; t < players; t++){
                        n = 0;
                        for(e = 0; e < players; e++){
                            if(pData[e].score + mines >= pData[t].score){
                                n++;
                            }
                        }
                        //End the game if he did
                        if(n == 1){
                            //Tell other players and stop
                            buffer[0] = 1;
                            buffer[1] = 'w';
                            for(e = 0; e < players; e++){
                                send(cSock[e], buffer, 2, 0);
                            }
                            return 0;
                        }
                    }
                    //Calculate the next turn
                    turn++;
                    turn %= players;
                    //Tell players who's next
                    buffer[0] = 2;
                    buffer[1] = 'v';
                    buffer[2] = turn;
                    for(n = 0; n < players; n++){
                        send(cSock[n], buffer, 3, 0);
                    }
                    printf("%s's turn.\n", pData[turn].name);
                }
            }
            //Player swept
            else if(pData[i].buffer[0] == 's' && i == turn){
                swept = sweep((pData[i].buffer[1] & 240) >> 4,
                    pData[i].buffer[1] & 15, mBuffer, field);
                printf("%s swept %u, %u. Value: %u. %u cell(s).\n", 
                    pData[i].name, (pData[i].buffer[1] & 240) >> 4,
                    pData[i].buffer[1] & 15,
                    field[(pData[i].buffer[1]&240)>>4][pData[i].buffer[1]&15],
                    swept);
                //If it was valid, tell the players
                if(swept){
                    sendReveal(swept, buffer, mBuffer, cSock, pData,
                        players, field);
                    //Play again if it was a mine
                    if(field[(mBuffer[0] & 240) >> 4][mBuffer[0] & 15] != 9){
                        turn++;
                        turn %= players;
                    }
                    else{
                        pData[i].score++;
                        mines--;
                        //Check if he won
                        for(t = 0; t < players; t++){
                            n = 0;
                            for(e = 0; e < players; e++){
                                if(pData[e].score + mines >= pData[t].score){
                                    n++;
                                }
                            }
                            //End the game if he did
                            if(n == 1){
                                //Tell other players and stop
                                buffer[0] = 1;
                                buffer[1] = 'w';
                                for(e = 0; e < players; e++){
                                    send(cSock[e], buffer, 2, 0);
                                }
                                return 0;
                            }
                        }
                    }
                    //Tell players who's next
                    buffer[0] = 2;
                    buffer[1] = 'v';
                    buffer[2] = turn;
                    for(n = 0; n < players; n++){
                        send(cSock[n], buffer, 3, 0);
                    }
                    printf("%s's turn.\n", pData[turn].name);
                }
            }
        }
    }

    return 0;
}

void sendReveal(unsigned char swept, char *buffer,
    unsigned char *mBuffer, int *cSock, struct data *pData,
    unsigned char players, unsigned char field[16][16]){

    unsigned char i, n;

    //Split and send
    buffer[1] = 'r';
    for(i = 0; i < swept;){
        //31 coordinates at a time
        buffer[0] = (swept - i) > 31? 63 : ((swept - i) * 2 + 1);

        for(n = 2; n < buffer[0]; n++){
            buffer[n++] = mBuffer[i];
            buffer[n] = field[(mBuffer[i] & 240) >> 4][mBuffer[i] & 15];
            i++;
        }
        for(n = 0; n < players; n++){
            send(cSock[n], buffer, buffer[0] + 1, 0);
        }
    }
}
