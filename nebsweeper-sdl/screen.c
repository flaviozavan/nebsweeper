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

#include <SDL/SDL.h>
#include "defs.h"
#include "screen.h"

void initializeScreen(SDL_Surface *screen, SDL_Surface *tiles,
    SDL_Surface *font){

    int i, n;

    SDL_Rect r = {0, 0, WIDTH, HEIGHT};
    SDL_Rect to;

    SDL_FillRect(screen, &r, SDL_MapRGB(screen->format, BG));

    //Completely hidden field
    r.w = TILE_WIDTH;
    r.h = TILE_HEIGHT;
    r.y = 0;
    r.x = UNCOVERED;
    
    //Blit each one
    for(i = 0; i < 16; i++){
        to.y = i * TILE_HEIGHT;

        for(n = 0; n < 16; n++){
            to.x = n * TILE_HEIGHT + BOARD_WIDTH;

            SDL_BlitSurface(tiles, &r, screen, &to);
        }
    }

    //Write the copyright
    printString(screen, font, "NebSweeper SDL " VERSION, 0,
        (BOARD_WIDTH + FIELD_WIDTH)/12 + 1, SDL_MapRGB(screen->format, RED));
    printString(screen, font, "Copyright 2011", 1,
        (BOARD_WIDTH + FIELD_WIDTH)/12 + 1, SDL_MapRGB(screen->format, RED));
    printString(screen, font, "Flavio Zavan", 2,
        (BOARD_WIDTH + FIELD_WIDTH)/12 + 1, SDL_MapRGB(screen->format, RED));

    printString(screen, font, "Licensed under the", 4,
        (BOARD_WIDTH + FIELD_WIDTH)/12 +1, SDL_MapRGB(screen->format, YELLOW));
    printString(screen, font, "GNU/GPL Version 3", 5,
        (BOARD_WIDTH + FIELD_WIDTH)/12 +1, SDL_MapRGB(screen->format, YELLOW));

    printString(screen, font, "Controls:", 7,
        (BOARD_WIDTH + FIELD_WIDTH)/12 + 1, SDL_MapRGB(screen->format, GREEN));
    printString(screen, font, "Mouse 1: Sweep/Bomb", 8,
        (BOARD_WIDTH + FIELD_WIDTH)/12 + 1, SDL_MapRGB(screen->format, GREEN));
    printString(screen, font, "Mouse 2: Toggle Bomb", 9,
        (BOARD_WIDTH + FIELD_WIDTH)/12 + 1, SDL_MapRGB(screen->format, GREEN));
    printString(screen, font, "Return: Chat", 10,
        (BOARD_WIDTH + FIELD_WIDTH)/12 + 1, SDL_MapRGB(screen->format, GREEN));
}

void printString(SDL_Surface *surface, SDL_Surface *font, char *string,
    int y, int x, Uint32 color){

    SDL_Rect from, to;

    //Calculate real y and x
    to.y = y * FONT_HEIGHT;
    to.w = FONT_WIDTH;
    to.h = FONT_HEIGHT;
    from.y = 0;
    from.w = FONT_WIDTH;
    from.h = FONT_HEIGHT;

    //Print
    for(to.x = x * FONT_WIDTH; *string; to.x += FONT_WIDTH){
        //Skip invalid chars
        if(*string >= 32 && *string < 127){
            from.x = (*(string++) - 32) * FONT_WIDTH;
        }
        else {
            from.x = ('?' - 32) * FONT_WIDTH;
            string++;
        }

        SDL_FillRect(surface, &to, color);
        SDL_BlitSurface(font, &from, surface, &to);
    }
}

void addString(SDL_Surface *screen, SDL_Surface *font,
    char *string, Uint32 color){

    SDL_Rect from, to;

    //Self blit the part we're keeping
    from.y = FIELD_HEIGHT + FONT_HEIGHT;
    from.x = 0;
    from.w = CHAT_WIDTH;
    from.h = CHAT_HEIGHT - FONT_HEIGHT * 2;

    to.x = 0;
    to.y = FIELD_HEIGHT;
    SDL_BlitSurface(screen, &from, screen, &to);

    //Clear the last line
    to.y = FIELD_HEIGHT + CHAT_HEIGHT - FONT_HEIGHT * 2;
    to.w = CHAT_WIDTH;
    to.h = FONT_HEIGHT;
    SDL_FillRect(screen, &to, SDL_MapRGB(screen->format, BG));

    //Add the string
    printString(screen, font, string,
        (FIELD_HEIGHT + CHAT_HEIGHT)/24 - 2, 0, color);
}

void initializeBoard(SDL_Surface *screen, SDL_Surface *font,
    struct data *pData, unsigned char bombs, unsigned char id,
    unsigned char mines, unsigned char players){

    int i;
    char s[22];

    for(i = 0; i < players; i++){
        if(pData[i].stats){
            addPlayer(screen, font, pData[i].name, i);
        }
        else {
            deletePlayer(screen, font, i);
        }
    }

    //Print bombs
    sprintf(s, "Bombs: %3d", bombs);
    printString(screen, font, s, id * 2 + 1, 0, getPlayerColor(screen, id));

    //Print mines
    sprintf(s, "%3d/%3d mines", 0, mines);
    printString(screen, font, s, (BOARD_HEIGHT / FONT_HEIGHT) - 1, 0,
        SDL_MapRGB(screen->format, RED));
}

void addPlayer(SDL_Surface *screen, SDL_Surface *font, char *name,
    unsigned char id){

    char s[22];

    sprintf(s, "%-16s %3d", name, 0);
    printString(screen, font, s, id * 2, 0, getPlayerColor(screen, id));
}

void deletePlayer(SDL_Surface *screen, SDL_Surface *font, unsigned char id){
    printString(screen, font, "--------Open--------", id * 2, 0,
        getPlayerColor(screen, id));
}

Uint32 getPlayerColor(SDL_Surface *s, unsigned char id){
    switch(id) { 
        case 1: 
            return SDL_MapRGB(s->format, GREEN); 

        case 2: 
            return SDL_MapRGB(s->format, WHITE); 

        case 3: 
            return SDL_MapRGB(s->format, YELLOW); 

        case 4: 
            return SDL_MapRGB(s->format, CYAN); 

        case 5: 
            return SDL_MapRGB(s->format, MAGENTA); 

        default: 
            return SDL_MapRGB(s->format, BLUE); 
    }
}

int keyboardHandler(SDL_Surface *screen, SDL_Surface *font,
    unsigned char *chatLen, unsigned char *isChatting, unsigned char id,
    char *chatBuffer, SDL_Event *event){

    SDL_Rect to, from;
    SDLKey c;
    unsigned int unicode;

    c = event->key.keysym.sym;
    unicode = event->key.keysym.unicode;
    
    //Pressed enter, either end or begin
    if(c == SDLK_RETURN){
        *isChatting ^= 1;

        if(!(*isChatting)){
            to.x = 0;
            to.y = FIELD_HEIGHT + CHAT_HEIGHT - FONT_HEIGHT;
            to.w = CHAT_WIDTH;
            to.h = FONT_HEIGHT;

            SDL_FillRect(screen, &to, SDL_MapRGB(screen->format, BG));

            return (*chatLen > 2);
        }

        printString(screen, font, "Say             :",
            (FIELD_HEIGHT + CHAT_HEIGHT) / 24 - 1, 0,
            getPlayerColor(screen, id));
    }
    //Regular text
    else if(*chatLen < 64 && unicode >= 32 && unicode <= 126){
        chatBuffer[*chatLen] = unicode;
        unicode -= 32;

        to.x = (16 + *chatLen) * FONT_WIDTH;
        to.y = FIELD_HEIGHT + CHAT_HEIGHT - FONT_HEIGHT;
        to.w = FONT_WIDTH;
        to.h = FONT_HEIGHT;
        from.x = unicode * FONT_WIDTH;
        from.y = 0;
        from.w = FONT_WIDTH;
        from.h = FONT_HEIGHT;

        SDL_FillRect(screen, &to, getPlayerColor(screen, id));
        SDL_BlitSurface(font, &from, screen, &to);

        (*chatLen)++;
    }
    //Backspace
    else if(c == SDLK_BACKSPACE && *chatLen > 2){
        chatBuffer[*chatLen] = 0;

        (*chatLen)--;

        to.x = (16 + *chatLen) * FONT_WIDTH;
        to.y = FIELD_HEIGHT + CHAT_HEIGHT - FONT_HEIGHT;
        to.w = FONT_WIDTH;
        to.h = FONT_HEIGHT;

        SDL_FillRect(screen, &to, SDL_MapRGB(screen->format, BG));
    }
    //Escape
    if(c == SDLK_ESCAPE && *isChatting){
        *chatLen = 2;
        chatBuffer[2] = 0;
        *isChatting = 0;

        to.x = 0;
        to.y = FIELD_HEIGHT + CHAT_HEIGHT - FONT_HEIGHT;
        to.w = CHAT_WIDTH;
        to.h = FONT_HEIGHT;

        SDL_FillRect(screen, &to, SDL_MapRGB(screen->format, BG));
    }

    return 0;
}

void setFieldValue(SDL_Surface *screen, SDL_Surface *tiles,
    int y, int x, unsigned char v, unsigned char turn){

    SDL_Rect to, from;

    from.x = v * TILE_WIDTH;
    from.y = 0;
    from.w = TILE_WIDTH;
    from.h = TILE_HEIGHT;

    to.y = y * TILE_HEIGHT;
    to.x = BOARD_WIDTH + x * TILE_WIDTH;
    to.w = TILE_WIDTH;
    to.h = TILE_HEIGHT;

    SDL_FillRect(screen, &to, getPlayerColor(screen, turn));
    SDL_BlitSurface(tiles, &from, screen, &to);
}

void updateCursor(SDL_Surface *screen, SDL_Surface *tiles, int *y, int*x,
    unsigned char bombing, unsigned char nBombing, int y2, int x2){

    SDL_Rect to, from;

    to.w = TILE_WIDTH;
    to.h = TILE_HEIGHT;
    from.y = 0;
    from.w = TILE_WIDTH;
    from.h = TILE_HEIGHT;

    //Remove last selection
    if(*x >= 0 && *x <= 15 && *y >= 0 && *y <= 15){
        clearCursor(screen, tiles, *y, *x, bombing);
    }

    *y = y2;
    *x = x2;

    //Fix the cursor when bombing
    if(nBombing){
        if(*y == 15){
            *y -= 2;
        }
        else if(*y > 0 && *y <= 15){
            (*y)--;
        }
        if(*x == 15){
            *x -= 2;
        }
        else if(*x > 0 && *x <= 15){
            (*x)--;
        }
    }

    //Blit the cursor
    if(*x >= 0 && *x <= 15 && *y >= 0 && *y <= 15){
        to.y = *y * TILE_HEIGHT;
        to.x = *x * TILE_WIDTH + BOARD_WIDTH;

        from.x = CURSOR;
        
        SDL_BlitSurface(tiles, &from, screen, &to);

        if(nBombing){
            to.x += TILE_WIDTH;
            SDL_BlitSurface(tiles, &from, screen, &to);
            
            to.x += TILE_WIDTH;
            SDL_BlitSurface(tiles, &from, screen, &to);

            to.x = *x * TILE_WIDTH + BOARD_WIDTH;
            to.y += TILE_HEIGHT;
            SDL_BlitSurface(tiles, &from, screen, &to);
            
            to.x += TILE_WIDTH;
            SDL_BlitSurface(tiles, &from, screen, &to);
            
            to.x += TILE_WIDTH;
            SDL_BlitSurface(tiles, &from, screen, &to);

            to.x = *x * TILE_WIDTH + BOARD_WIDTH;
            to.y += TILE_HEIGHT;
            SDL_BlitSurface(tiles, &from, screen, &to);
            
            to.x += TILE_WIDTH;
            SDL_BlitSurface(tiles, &from, screen, &to);
            
            to.x += TILE_WIDTH;
            SDL_BlitSurface(tiles, &from, screen, &to);
        }
    }
}

void clearCursor(SDL_Surface *screen, SDL_Surface *tiles,
    int y, int x, unsigned char bombing){

    SDL_Rect to, from;

    to.w = TILE_WIDTH;
    to.h = TILE_HEIGHT;
    from.y = 0;
    from.w = TILE_WIDTH;
    from.h = TILE_HEIGHT;

    to.y = y * TILE_HEIGHT;
    to.x = x * TILE_WIDTH + BOARD_WIDTH;

    from.x = CLEAN;
    
    SDL_BlitSurface(tiles, &from, screen, &to);

    if(bombing){
        to.x += TILE_WIDTH;
        SDL_BlitSurface(tiles, &from, screen, &to);
        
        to.x += TILE_WIDTH;
        SDL_BlitSurface(tiles, &from, screen, &to);

        to.x = x * TILE_WIDTH + BOARD_WIDTH;
        to.y += TILE_HEIGHT;
        SDL_BlitSurface(tiles, &from, screen, &to);
        
        to.x += TILE_WIDTH;
        SDL_BlitSurface(tiles, &from, screen, &to);
        
        to.x += TILE_WIDTH;
        SDL_BlitSurface(tiles, &from, screen, &to);

        to.x = x * TILE_WIDTH + BOARD_WIDTH;
        to.y += TILE_HEIGHT;
        SDL_BlitSurface(tiles, &from, screen, &to);
        
        to.x += TILE_WIDTH;
        SDL_BlitSurface(tiles, &from, screen, &to);
        
        to.x += TILE_WIDTH;
        SDL_BlitSurface(tiles, &from, screen, &to);
    }
}

void showWinner(SDL_Surface *screen, SDL_Surface *font, struct data *pData,
    unsigned char players){

    unsigned char i, n, t;
    char s[80];
    SDL_Event event;

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
    if(!t){
        sprintf(s, "The winner is %s.", pData[n].name);
        addString(screen, font, s, getPlayerColor(screen, n));
    }
    else {
        addString(screen, font, "Draw!", SDL_MapRGB(screen->format, WHITE));
    }

    SDL_Flip(screen);

    //Wait for quit
    do {
        SDL_WaitEvent(&event);
    } while(event.type != SDL_QUIT);
}
