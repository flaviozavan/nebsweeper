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

#define DEFAULTIP "127.0.0.1"
#define DEFAULTPORT 4031

int startSocket(int *sock, const char *host, unsigned short port);

int exchangeBasics(int sock, unsigned char *players, unsigned char *bombs,
    unsigned char *mines, unsigned char *id, char *name, struct data **pData);

int game(SDL_Surface *screen, SDL_Surface *tiles, SDL_Surface *font,
    int sock, unsigned char id, unsigned char players,
    unsigned char bombs, unsigned char mines, struct data *pData,
    unsigned char field[16][16]);
