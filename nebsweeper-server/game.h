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

#define MAXPLAYERS 6
#define DEFAULTBOMBS 1
#define DEFAULTMINES 51

struct data {
    //Stat: byte0 = connected, byte1 = named
    unsigned char stats;
    unsigned char bombs;
    unsigned short score;
    unsigned char namelen;
    unsigned char toReceive;
    unsigned char toDiscard;
    unsigned char len;
    char name[17];
    char buffer[65];
};

void initialize(unsigned char players, unsigned char mines,
    unsigned char bombs, struct data *pData, unsigned char field[16][16]);

int sweep(unsigned char y, unsigned char x, unsigned char *mBuffer,
    unsigned char field[16][16]);

int bomb(unsigned char y, unsigned char x, unsigned char *mBuffer,
    unsigned char *mines, struct data *pData, unsigned char p,
    unsigned char field[16][16]);
