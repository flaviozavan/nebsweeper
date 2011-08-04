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

#define VERSION "1.0 RC8"
#define TILES "data/tiles.png"
#define FONT "data/font.png"
#define FONT_HEIGHT 24
#define FONT_WIDTH 12
#define TILE_HEIGHT 24
#define TILE_WIDTH 24
#define UNCOVERED 10 * TILE_WIDTH
#define MINE 9 * TILE_WIDTH
#define CURSOR 11 * TILE_WIDTH
#define CLEAN 12 * TILE_WIDTH

struct data{
    char name[17];
    unsigned char score;
    unsigned char stats;
};

int main(int argc, char *argv[]);
