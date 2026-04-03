#ifndef _UTILS_H_
#define _UTILS_H_

#include <SDL3/SDL.h>

//drawing the level
void DrawEmptySpace(SDL_Renderer *renderer, SDL_FRect rect, int x, int y , int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize);

void DrawWall(SDL_Renderer *renderer, SDL_FRect rect, int x, int y , int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize);

void DrawDot(SDL_Renderer *renderer, SDL_FRect rect, int x, int y , int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize);

//drawing the menus



#endif
