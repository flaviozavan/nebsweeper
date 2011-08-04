/*
 * NebSweeper Curses - Curses client for NebSweeper
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

#define VERSION "1.0 RC8"

struct data{
    char name[17];
    unsigned char score;
    unsigned char stats;
};

int main(int argc, char *argv[]);

void cleanup(int param);
