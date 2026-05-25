#include "GUIElement.h"




void DrawBackground(SDL_Renderer *renderer){// function for drawing background
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
}

SDL_Texture *SetTextAsTexture(SDL_Renderer *renderer, SDL_FRect &textureRect, const SDL_FRect &rect, const std::string &text, const SDL_Color &textColor, TTF_Font *font){//function for setting text as a texture for further text displaying
    SDL_Texture *texture = NULL;
    if(!(text).empty()){
        SDL_Surface *textSurface = NULL;
        textSurface = TTF_RenderText_Blended(font, (text).c_str(), 0, (textColor));
        textureRect = {rect.x, (rect.y + rect.h /2) - (float)textSurface->h / 2, 
            (float)textSurface->w, (float)textSurface->h};
        texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        return texture;
    }else return texture;
} 

SDL_Texture *SetTextAsTextureCentered(SDL_Renderer *renderer, SDL_FRect &textureRect, const SDL_FRect &rect, const std::string &text, const SDL_Color &textColor, TTF_Font *font){//function for setting text as a texture for further text displaying
    SDL_Texture *texture = NULL;
    if(!(text).empty()){
        SDL_Surface *textSurface = NULL;
        textSurface = TTF_RenderText_Blended(font, (text).c_str(), 0, (textColor));
        textureRect = {(rect.x + rect.w / 2) - (float)textSurface->w / 2, (rect.y + rect.h /2) - (float)textSurface->h / 2, 
            (float)textSurface->w, (float)textSurface->h};
        texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        return texture;
    }else return texture;
} 

SDL_Texture *SetTexture(SDL_Renderer *renderer, SDL_FRect &textRect, SDL_Texture *tex){
    return tex;
}