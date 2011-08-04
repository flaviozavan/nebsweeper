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

#define MINLINES 20
#define MINCOLS 80

int initializeScreen(WINDOW **left, WINDOW **middle, WINDOW **right,
    WINDOW **bottom, WINDOW **textBox);

void drawMiddle(WINDOW *middle, unsigned char field[16][16]);

void drawLeft(WINDOW *left, unsigned char players, unsigned char id,
    unsigned char bombs, unsigned char mines, struct data *pData);

void drawRight(WINDOW *right);

int keyboardHandler(WINDOW *middle, WINDOW *textBox,
    unsigned char *chatLen, unsigned char *isChatting, unsigned char bombs,
    unsigned char *bombing, unsigned char *y, unsigned char *x,
    unsigned char id, unsigned char turn, unsigned char field[16][16],
    char *chatBuffer);

void addLine(WINDOW *bottom, unsigned char msgLen, char *buffer,
    struct data *pData);

void addPlayer(WINDOW *left, WINDOW *bottom, unsigned char id,
    struct data *pData);

void removePlayer(WINDOW *left, WINDOW *bottom, unsigned char id,
    struct data *pData);

void tellTurn(WINDOW *bottom, unsigned char turn, struct data *pData);

void setValue(WINDOW *middle, unsigned char y, unsigned char x,
    unsigned char v, unsigned char t);

void updateScore(WINDOW *left, unsigned char t, struct data *pData);

void startCursor(WINDOW *middle, unsigned char *y, unsigned char *x,
    unsigned char bombing, unsigned char field[16][16]);

void updateBombs(WINDOW *left, unsigned char id, unsigned char bombs);

void updateMines(WINDOW *left, unsigned char mines);

void showWinner(WINDOW *bottom, unsigned char players, struct data *pData);
