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

#include "game.h"

void initialize(unsigned char players, unsigned char mines,
    unsigned char bombs, struct data *pData, unsigned char field[16][16]){

    unsigned char i, n, t, y, x;
    unsigned char around[8][2] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, 
        {1, 0}, {1, -1}, {0, -1}, {-1, -1}};

    
    //Initialize the data array
    for(i = 0; i < players; i++){
        pData[i].stats = 0;
        pData[i].bombs = bombs;
        pData[i].score = 0;
        pData[i].toReceive = 0;
        pData[i].toDiscard = 0;
    }

    //Clear the field
    for(i = 0; i < 16; i++){
        for(n = 0; n < 16; n++){
            field[i][n] = 0;
        }
    }

    //Place mines
    for(i = 0; i < mines; i++){
        y = (rand() % 16);
        x = (rand() % 16);
        while(field[y][x]){
            if(++x > 15){
                x = 0;
                if(++y > 15){
                    y = 0;
                }
            }
        }
        field[y][x] = 9;
    }

    //Place numbers
    for(y = 0; y < 16; y++){
        for(x = 0; x < 16; x++){
            if(!field[y][x]){
                for(i = 0; i < 8; i++){
                    n = y + around[i][0];
                    t = x + around[i][1];
                    //n and t are unsigned char, so 0 - 1 > 16
                    if(n < 16 && t < 16){
                        field[y][x] += (field[n][t] == 9);
                    }
                }
            }
        }
    }
    //Set the most significant bit for the entire field
    //It means it is not yet revealed
    for(i = 0; i < 16; i++){
        for(n = 0; n < 16; n++){
            field[i][n] |= 128;
        }
    }
}

int sweep(unsigned char y, unsigned char x, unsigned char *mBuffer,
    unsigned char field[16][16]){

    unsigned char i, m, n, t;
    //l == last element in lStack
    short int l;
    unsigned char lStack[255];
    unsigned char around[8][2] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, 
        {1, 0}, {1, -1}, {0, -1}, {-1, -1}};

    //Check if it's already revealed
    if(!(field[y][x] & 128)){
        return 0;
    }

    //Reveal it
    //Push the first element
    lStack[0] = (y << 4) | x;
    l = 0;
    m = 0;
    field[y][x] &= 127;

    //Start popping
    while(l >= 0){
        x = lStack[l] & 15;
        y = (lStack[l] & 240) >> 4;
        mBuffer[m++] = lStack[l--];
        //If it's a zero, reveal more
        if(field[y][x]){
            continue;
        }
        //Reveal around if it's a zero
        for(i = 0; i < 8; i++){
            n = y + around[i][0];
            t = x + around[i][1];
            //n and t are unsigned char, so 0 - 1 > 16
            if(n < 16 && t < 16 && field[n][t] & 128){
                //If the square exists and is not revealed,
                //push it into the stack
                lStack[++l] = (n << 4) | t;
                //mark it
                field[n][t] &= 127;
            }
        }
    }

    return m;
}

int bomb(unsigned char y, unsigned char x, unsigned char *mBuffer,
    unsigned char *mines, struct data *pData, unsigned char p,
    unsigned char field[16][16]){

    unsigned char m, i, n, s;
    
    //Check if it is valid
    if(y > 13 || x > 13){
        return 0;
    }

    m = 0;

    //Sweep all 9 squares and return the buffer
    for(i = 0; i < 3; i++){
        for(n = 0; n < 3; n++){
            s = sweep(y+i, x+n, mBuffer + m*sizeof(unsigned char), field);
            //Update score
            if(field[(mBuffer[m] & 240) >> 4][mBuffer[m] & 15] == 9){
                pData[p].score++;
                (*mines)--;
            }
            m += s;
        }
    }

    return m;
}
