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

#define DEFAULTPORT 4031

int startServer(int *sSock, unsigned short port, struct sockaddr_in *sAddr,
    unsigned char players);

int waitForPlayers(int *sSock, int *cSock, struct sockaddr_in *cAddr,
    struct data *pData, unsigned char players, unsigned char mines,
    unsigned char bombs);

int play(int *cSock, struct data *pData, unsigned char players,
    unsigned char mines, unsigned char field[16][16]);

void sendReveal(unsigned char swept, char *buffer,
    unsigned char *mBuffer, int *cSock, struct data *pData,
    unsigned char players, unsigned char field[16][16]);
