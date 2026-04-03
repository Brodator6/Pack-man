#include "GUIElement.h"

void DrawBackground(SDL_Renderer *renderer){// function for drawing background
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
}