/*
 * NebSweeper Curses - A server for NebSweeper
 * NebSweeper Curses Copyright (C) 2010, 2011 Flávio Zavan
 *
 * This file is part of NebSweeper Curses.
 *
 * NebSweeper Curses is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NebSweeper Curses is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with NebSweeper Curses.  If not, see <http://www.gnu.org/licenses/>.
 *
 * flavio [AT] nebososo [DOT] com
 * http://www.nebososo.com
*/

#include <curses.h>
#include "defs.h"
#include "screen.h"

const char VALUE[11] = {' ', '1', '2', '3', '4', '5', '6', '7', '8', 'M', '#'};


int initializeScreen(WINDOW **left, WINDOW **middle, WINDOW **right,
    WINDOW **bottom, WINDOW **textBox){

    //Allocate the memory
    if(!(*left = newwin(17, 21, 0, 0)) ||
        !(*middle = newwin(17, COLS - 43, 0, 21)) ||
        !(*right = newwin(17, 21, 0, COLS - 22)) ||
        !(*bottom = newwin(LINES - 18, COLS, 17, 0)) ||
        !(*textBox = newwin(1, COLS, LINES-1, 0))){

        return 1;
    }

    //Enable scroll where we need it
    scrollok(*bottom, TRUE);
    //Enable arrow keys
    keypad(stdscr, TRUE);

    //Nodelay on windows because its select() sucks
#ifdef WIN32
    nodelay(stdscr, TRUE);
#endif

    //Make pairs if colors are available
    if(has_colors()){
        start_color();

        init_pair(1, COLOR_YELLOW, COLOR_YELLOW);
        init_pair(2, COLOR_BLUE, COLOR_BLACK);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
        init_pair(4, COLOR_WHITE, COLOR_BLACK);
        init_pair(5, COLOR_YELLOW, COLOR_BLACK);
        init_pair(6, COLOR_CYAN, COLOR_BLACK);
        init_pair(7, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(8, COLOR_RED, COLOR_BLACK);
        init_pair(9, COLOR_BLACK, COLOR_WHITE);
        init_pair(10, COLOR_WHITE, COLOR_BLACK);
        init_pair(11, COLOR_WHITE, COLOR_YELLOW);
        init_pair(12, COLOR_RED, COLOR_BLUE);
        init_pair(13, COLOR_RED, COLOR_GREEN);
        init_pair(14, COLOR_RED, COLOR_WHITE);
        init_pair(15, COLOR_RED, COLOR_YELLOW);
        init_pair(16, COLOR_RED, COLOR_CYAN);
        init_pair(17, COLOR_RED, COLOR_MAGENTA);
        init_pair(18, COLOR_WHITE, COLOR_CYAN);
    }

    return 0;
}

void drawMiddle(WINDOW *middle, unsigned char field[16][16]){
    unsigned int i, n;

    //Borders
    wborder(middle, ' ', ' ', ' ', '-', ' ', ' ', '-', '-');
    
    //The field stats completely unrevealed
    wcolor_set(middle, 11, NULL);

    for(i = 0; i < 16; i++){
        //Reset the cursor
        wmove(middle, i, ((COLS-42)/2)-8);
        for(n = 0; n < 16; n++){
            waddch(middle, '#');
        }
    }
}

void drawLeft(WINDOW *left, unsigned char players, unsigned char id,
    unsigned char bombs, unsigned char mines, struct data *pData){

    unsigned int i;

    //Add borders
    wborder(left, ' ', '|', ' ', '-', ' ', '|', '-', '+');

    //Write all the information we have so far
    for(i = 0; i < players; i++){
        //Set the appropriate color
        wcolor_set(left, i + 2 , NULL);

        if(pData[i].stats){
            mvwprintw(left, 2*i, 0, "%s", pData[i].name);
            mvwprintw(left, 2*i, 17, "%3d", pData[i].score);

            //Print the bombs you have
            if(i == id){
                mvwprintw(left, 2*i + 1, 0, "          Bombs: %3d", bombs);
            }
        }
        else {
            mvwprintw(left, 2*i, 0, "--------Open--------");
        }
    }

    //Print mines
    wcolor_set(left, 8, NULL);
    mvwprintw(left, 15, 0, "%3d/%3d mines", mines, mines);
}

void drawRight(WINDOW *right){
    //Borders
    wborder(right, '|', ' ', ' ', '-', '|', ' ', '+', '-');

    //Red
    wcolor_set(right, 8, NULL);

    mvwprintw(right, 0, 1, "NebSweeper Curses");
    mvwprintw(right, 1, 1, "%s", VERSION);
    mvwprintw(right, 2, 1, "Copyright 201{0,1}");
    mvwprintw(right, 3, 1, "Flavio Zavan");

    //Yelow
    wcolor_set(right, 5, NULL);
    
    mvwprintw(right, 5, 1, "Licensed under the");
    mvwprintw(right, 6, 1, "GNU/GPL version 3");

    //Green
    wcolor_set(right, 3, NULL);

    mvwprintw(right, 11, 1, "Keys:");
    mvwprintw(right, 12, 1, "Arrows: Move Cursor");
    mvwprintw(right, 13, 1, "Space: Sweep/Bomb");
    mvwprintw(right, 14, 1, "B: Toggle Bomb");
    mvwprintw(right, 15, 1, "Return: Chat");
}

int keyboardHandler(WINDOW *middle, WINDOW *textBox,
    unsigned char *chatLen, unsigned char *isChatting, unsigned char bombs,
    unsigned char *bombing, unsigned char *y, unsigned char *x,
    unsigned char id, unsigned char turn, unsigned char field[16][16],
    char *chatBuffer){

    int c;
    unsigned char i, n, v;
    
    //Read from the buffer
    c = getch();

#ifdef WIN32
    if(c == ERR){
        return 4;
    }
#endif

    //Process
    switch(c){
        case KEY_LEFT:
            //Move the cursor if it's our turn
            if(turn != id || *isChatting){
                break;
            }

            if(!(*bombing)){
                //Fix the old spot
                setValue(middle, *y, *x, field[*y][*x] & 15,
                    (field[*y][*x] & 240) >> 4);

                //Find an appropriate position
                do{
                    if(*x > 0){
                        (*x)--;
                    }
                    else {
                        *x = 15;
                        if(*y > 0){
                            (*y)--;
                        }
                        else {
                            *y = 15;
                        }
                    }
                }while(field[*y][*x] != 10);
            }
            //Bomb
            else{
                //Fix the old spot
                for(i = 0; i < 3; i++){
                    for(n = 0; n < 3; n++){
                    setValue(middle, *y + i, *x +n, field[*y + i][*x +n] & 15,
                        (field[*y + i][*x + n] & 240) >> 4);
                    }
                }
                //Find 9 good cells
                v = 0;
                do{
                    //Move cursor
                    if(*x > 0){
                        (*x)--;
                    }
                    else {
                        *x = 13;
                        if(*y > 0){
                            (*y)--;
                        }
                        else {
                            *y = 13;
                        }
                    }
                    v = 0;
                    //Calculate how many good cells
                    for(i = 0; i < 3; i++){
                        for(n = 0; n < 3; n++){
                            v += (field[*y + i][*x + n] == 10);
                        }
                    }
                }while(!v);
            }

            //Redraw cursor
            startCursor(middle, y, x, *bombing, field);

            wrefresh(middle);
            break;

        case KEY_RIGHT:
            //Move the cursor if it's our turn
            if(turn != id || *isChatting){
                break;
            }

            if(!(*bombing)){
                //Fix the old spot
                setValue(middle, *y, *x, field[*y][*x] & 15,
                    (field[*y][*x] & 240) >> 4);

                //Find an appropriate position
                do{
                    if(*x < 15){
                        (*x)++;
                    }
                    else {
                        *x = 0;
                        if(*y < 15){
                            (*y)++;
                        }
                        else {
                            *y = 0;
                        }
                    }
                }while(field[*y][*x] != 10);
            }
            //Bomb
            else{
                //Fix the old spot
                for(i = 0; i < 3; i++){
                    for(n = 0; n < 3; n++){
                    setValue(middle, *y + i, *x +n, field[*y + i][*x +n] & 15,
                        (field[*y + i][*x + n] & 240) >> 4);
                    }
                }
                //Find 9 good cells
                do{
                    //Move the cursor
                    if(*x < 13){
                        (*x)++;
                    }
                    else {
                        *x = 0;
                        if(*y < 13){
                            (*y)++;
                        }
                        else {
                            *y = 0;
                        }
                    }
                    v = 0;
                    //Calculate how many good cells
                    for(i = 0; i < 3; i++){
                        for(n = 0; n < 3; n++){
                            v += (field[*y + i][*x + n] == 10);
                        }
                    }
                }while(!v);
            }

            startCursor(middle, y, x, *bombing, field);

            wrefresh(middle);
            break;

        case KEY_DOWN:
            //Move the cursor if it's our turn
            if(turn != id || *isChatting){
                break;
            }

            if(!(*bombing)){
                //Fix the old spot
                setValue(middle, *y, *x, field[*y][*x] & 15,
                    (field[*y][*x] & 240) >> 4);

                //Find an appropriate position
                do{
                    if(*y < 15){
                        (*y)++;
                    }
                    else {
                        *y = 0;
                        if(*x < 15){
                            (*x)++;
                        }
                        else {
                            *x = 0;
                        }
                    }
                }while(field[*y][*x] != 10);
            }
            //Bomb
            else{
                //Fix the old spot
                for(i = 0; i < 3; i++){
                    for(n = 0; n < 3; n++){
                    setValue(middle, *y + i, *x +n, field[*y + i][*x +n] & 15,
                        (field[*y + i][*x + n] & 240) >> 4);
                    }
                }
                //Find 9 good cells
                do{
                    //Move the cursor
                    if(*y < 13){
                        (*y)++;
                    }
                    else {
                        *y = 0;
                        if(*x < 13){
                            (*x)++;
                        }
                        else {
                            *x = 0;
                        }
                    }
                    v = 0;
                    //Calculate how many good cells
                    for(i = 0; i < 3; i++){
                        for(n = 0; n < 3; n++){
                            v += (field[*y + i][*x + n] == 10);
                        }
                    }
                }while(!v);
            }
                
            startCursor(middle, y, x, *bombing, field);

            wrefresh(middle);
            break;

        case KEY_UP:
            //Move the cursor if it's our turn
            if(turn != id || *isChatting){
                break;
            }

            if(!(*bombing)){
                //Fix the old spot
                setValue(middle, *y, *x, field[*y][*x] & 15,
                    (field[*y][*x] & 240) >> 4);

                //Find an appropriate position
                do{
                    if(*y > 0){
                        (*y)--;
                    }
                    else {
                        *y = 15;
                        if(*x > 0){
                            (*x)--;
                        }
                        else {
                            *x = 15;
                        }
                    }
                }while(field[*y][*x] != 10);
            }
            //Bomb
            else{
                //Fix the old spot
                for(i = 0; i < 3; i++){
                    for(n = 0; n < 3; n++){
                    setValue(middle, *y + i, *x +n, field[*y + i][*x +n] & 15,
                        (field[*y + i][*x + n] & 240) >> 4);
                    }
                }
                //Find 9 good cells
                do{
                    //Move the cursor
                    if(*y > 0){
                        (*y)--;
                    }
                    else {
                        *y = 13;
                        if(*x > 0){
                            (*x)--;
                        }
                        else {
                            *x = 13;
                        }
                    }
                    v = 0;
                    //Calculate how many good cells
                    for(i = 0; i < 3; i++){
                        for(n = 0; n < 3; n++){
                            v += (field[*y + i][*x + n] == 10);
                        }
                    }
                }while(!v);
            }

            startCursor(middle, y, x, *bombing, field);

            wrefresh(middle);
            break;

        //Enter
        case 10:
            if(*isChatting){
                //Send if we have stuff
                if(*chatLen > 2){
                    return 1;
                }
                *isChatting = 0;
                mvwhline(textBox, 0, 0, ' ', COLS-18);
                *chatLen = 2;
            }
            else {
                *isChatting = 1;
                mvwprintw(textBox, 0, 0, "Say             : ");
            }
            wrefresh(textBox);
            break;

        //Backspace
        case 127:
        case 8:
        case 263:
            if(*isChatting && *chatLen > 2){
                *chatLen -= 1;
                mvwaddch(textBox, 0, getcurx(textBox) - 1, ' ');
                wmove(textBox, 0, getcurx(textBox) - 1);
                wrefresh(textBox);
            }
            break;

        default:
            //Add the character if the player's chatting
            if(*isChatting && *chatLen < 64 && c > 31 && c <= 127){
                //Add it to the buffer
                chatBuffer[(*chatLen)++] = c;
                waddch(textBox, c);
                wrefresh(textBox);
            }
            else if(id == turn && !(*isChatting)){
                //Space bar
                if(c == 32){
                    if(!(*bombing)){
                        return 2;
                    }
                    else {
                        return 3;
                    }
                }
                //B button
                else if((c == 66 || c == 98) && bombs){
                    //Hide the cursor
                    if(*bombing){
                        for(i = 0; i < 3; i++){
                            for(n = 0; n < 3; n++){
                                setValue(middle, *y + i, *x + n, 
                                    field[*y + i][*x + n] & 15,
                                    (field[*y + i][*x + n] & 240) >> 4);
                            }
                        }
                    }
                    else {
                        setValue(middle, *y, *x, field[*y][*x] & 15,
                            (field[*y][*x] & 240) >> 4);
                    }

                    //Invert
                    *bombing ^= 1;
                    //Draw
                    startCursor(middle, y, x, *bombing, field);
                }
            }
            break;
    }

    return 0;
}

void addLine(WINDOW *bottom, unsigned char msgLen, char *buffer,
    struct data *pData){

    unsigned char i;

    scroll(bottom);

    //Set the color
    wcolor_set(bottom, buffer[1] + 2, NULL);

    //Print name
    mvwprintw(bottom, LINES-19, 0,
        pData[(unsigned char) buffer[1]].name);
    wprintw(bottom, ": ");

    //Reset color
    wcolor_set(bottom, 0, NULL);
    //Add the zero and print the message
    buffer[msgLen] = 0;
    //Invalidate multi byte characters
    for(i = 0; buffer[i]; i++){
        if(buffer[i] < 32){
            buffer[i] = '?';
        }
    }
    //Print
    wprintw(bottom, buffer + 2 * sizeof(char));

    wrefresh(bottom);
}

void addPlayer(WINDOW *left, WINDOW *bottom, unsigned char id,
    struct data *pData){

    //Set the appropriate color
    wcolor_set(left, id + 2 , NULL);

    //Clean
    mvwhline(left, id * 2, 0, ' ', 20);

    //Print the name and score
    mvwprintw(left, 2 * id, 0, "%s", pData[id].name);
    mvwprintw(left, 2 * id, 17, "%3d", pData[id].score);

    //Tell the player
    wcolor_set(bottom, id + 2 , NULL);

    //Scroll and print the message
    scroll(bottom);
    mvwprintw(bottom, LINES-19, 0, "%s joined the game.", pData[id].name);

    wrefresh(left);
    wrefresh(bottom);
}

void removePlayer(WINDOW *left, WINDOW *bottom, unsigned char id,
    struct data *pData){

    //Remove the name
    mvwhline(left, id * 2, 0, ' ', 20);

    //Place Open
    wcolor_set(left, id + 2 , NULL);
    mvwprintw(left, 2*id, 0, "--------Open--------");

    //Tell the player
    wcolor_set(bottom, id + 2 , NULL);

    //Scroll and print the message
    scroll(bottom);
    mvwprintw(bottom, LINES-19, 0, "%s left the game.", pData[id].name);

    wrefresh(left);
    wrefresh(bottom);
}

void tellTurn(WINDOW *bottom, unsigned char turn, struct data *pData){
    //Set the color
    wcolor_set(bottom, turn + 2 , NULL);

    //Scroll and print the message
    scroll(bottom);
    mvwprintw(bottom, LINES-19, 0, "%s's turn.", pData[turn].name);

    wrefresh(bottom);
}

void setValue(WINDOW *middle, unsigned char y, unsigned char x,
    unsigned char v, unsigned char t){

    //Get the right color
    wcolor_set(middle, v != 9? v + 1 : t + 12, NULL);

    //Change the char
    mvwaddch(middle, y, (COLS-42)/2-8+x, VALUE[v]);

    //Move the cursor out of the way
    wmove(middle, 0, 0);

    wrefresh(middle);
}

void updateScore(WINDOW *left, unsigned char t, struct data *pData){
    //Set the appropriate color
    wcolor_set(left, t + 2 , NULL);

    //Print the name and score
    mvwprintw(left, 2 * t, 0, "%s", pData[t].name);
    mvwprintw(left, 2 * t, 17, "%3d", pData[t].score);

    wrefresh(left);
}

void startCursor(WINDOW *middle, unsigned char *y, unsigned char *x,
    unsigned char bombing, unsigned char field[16][16]){

    unsigned char i, n, v;

    //Set the appropriate color
    wcolor_set(middle, 18, NULL);

    if(!bombing){
        //Find an unrevealed spot and move the cursor
        while((field[*y][*x] & 15) < 10){
            (*x)++;
            if(*x > 15){
                *x = 0;
                (*y)++;
                if(*y > 15){
                    *y = 0;
                }
            }
        }
        //Print an x
        mvwaddch(middle, *y, (COLS-42)/2-8+*x, 'x');
    }
    else {
        v = 0;

        //Fix if it's too far
        if(*x > 13){
            *x = 13;
        }
        if(*y > 13){
            *y = 13;
        }   

        //Find an area with, at least one unrevealed spot
        while(!v){
            //Calculate how many good cells
            for(i = 0; i < 3; i++){
                for(n = 0; n < 3; n++){
                    v += (field[*y + i][*x + n] == 10);
                }
            }
            //Find a new area if that one wasn't good
            if(!v){
                (*x)++;
                if(*x > 13){
                    *x = 0;
                    (*y)++;
                    if(*y > 13){
                        *y = 0;
                    }
                }
            }
        }

        //Print the letter b
        for(i = 0; i < 3; i++){
            for(n = 0; n < 3; n++){
                if(field[*y + i][*x + n] == 10){
                    mvwaddch(middle, *y + i, (COLS-42)/2-8+*x + n, 'b');
                }
            }
        }
    }

    //Move the cursor out of the way to avoid confusion
    wmove(middle, 0,0);

    wrefresh(middle);
}

void updateBombs(WINDOW *left, unsigned char id, unsigned char bombs){
    //Set the color
    wcolor_set(left, id + 2 , NULL);

    //Print bombs
    mvwprintw(left, 2*id + 1, 0, "          Bombs: %3d", bombs);
}

void updateMines(WINDOW *left, unsigned char mines){
    wcolor_set(left, 8, NULL);
    mvwprintw(left, 15, 0, "%3d", mines);

    wrefresh(left);
}

void showWinner(WINDOW *bottom, unsigned char players, struct data *pData){
    unsigned char i, n, t;

    n = 0;
    t = 0;
    //Decide who's the winner
    for(i = 1; i < players; i++){
        if(pData[i].score > pData[n].score){
            n = i;
            t = 0;
        }
        else if(pData[i].score == pData[n].score){
            t++;
        }
    }

    //Print on the screen
    scroll(bottom);
    if(!t){
        //Set the color
        wcolor_set(bottom, n + 2, NULL);

        mvwprintw(bottom, LINES-19, 0, "The winner is: %s.", pData[n].name);
    }
    else {
        wcolor_set(bottom, 8, NULL);

        mvwprintw(bottom, LINES-19, 0, "Draw!");
    }
    scroll(bottom);

    wcolor_set(bottom, 0, NULL);
    mvwprintw(bottom, LINES-19, 0, "Press ENTER to continue.");

    wrefresh(bottom);

    //Disable nodelay on windows
#ifdef WIN32
    nodelay(stdscr, FALSE);
#endif

    //Wait for enter
    while(getch() != 10);
}
