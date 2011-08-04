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

#define BOARD_WIDTH 252
#define BOARD_HEIGHT 384
#define CHAT_WIDTH 960
#define CHAT_HEIGHT 168
#define FIELD_WIDTH 384
#define FIELD_HEIGHT 384
#define WIDTH CHAT_WIDTH
#define HEIGHT CHAT_HEIGHT + FIELD_HEIGHT
#define BG 146, 146, 36
#define RED 255, 0, 0
#define YELLOW 219, 219, 0
#define GREEN 0, 146, 0
#define WHITE 255, 255, 255
#define CYAN 0, 219, 219
#define BLUE 0, 0, 255
#define MAGENTA 255, 0, 255

void initializeScreen(SDL_Surface *screen, SDL_Surface *tiles,
    SDL_Surface *font);

void printString(SDL_Surface *surface, SDL_Surface *font, char *string,
    int y, int x, Uint32 color);

void addString(SDL_Surface *screen, SDL_Surface *font,
    char *string, Uint32 color);

void initializeBoard(SDL_Surface *screen, SDL_Surface *font,
    struct data *pData, unsigned char bombs, unsigned char id,
    unsigned char mines, unsigned char players);

void addPlayer(SDL_Surface *screen, SDL_Surface *font,
    char *name, unsigned char id);

void deletePlayer(SDL_Surface *screen, SDL_Surface *font, unsigned char id);

inline Uint32 getPlayerColor(SDL_Surface *s, unsigned char id);

int keyboardHandler(SDL_Surface *screen, SDL_Surface *font,
    unsigned char *chatLen, unsigned char *isChatting, unsigned char id,
    char *chatBuffer, SDL_Event *event);

void setFieldValue(SDL_Surface *screen, SDL_Surface *tiles,
    int y, int x, unsigned char v, unsigned char turn);

void updateCursor(SDL_Surface *screen, SDL_Surface *tiles, int *y, int*x,
    unsigned char bombing, unsigned char nBombing, int y2, int x2);

void clearCursor(SDL_Surface *screen, SDL_Surface *tiles,
    int y, int x, unsigned char bombing);

void showWinner(SDL_Surface *screen, SDL_Surface *font, struct data *pData,
    unsigned char players);
