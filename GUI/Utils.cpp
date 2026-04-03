#include "Utils.h"
void DrawEmptySpace(SDL_Renderer *renderer, SDL_FRect rect, int x, int y , int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize){

};

void DrawWall(SDL_Renderer *renderer, SDL_FRect rect, int x, int y , int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize){
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
    rect.x = hightMargine + x * cellHight;
    rect.y = widthMargine + y * cellWidth;
    rect.w = cellWidth;
    rect.h = cellHight;
    SDL_RenderFillRect(renderer, &rect);
};

void DrawDot(SDL_Renderer *renderer, SDL_FRect rect, int x, int y , int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize){
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
    rect.x = hightMargine + x * cellHight;
    rect.y = widthMargine + y * cellWidth;
    rect.w = cellWidth;
    rect.h = cellHight;
    SDL_RenderRect(renderer, &rect);
};