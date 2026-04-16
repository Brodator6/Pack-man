#ifndef _GUIElement_H_
#define _GUIElement_H_

#include <SDL3/SDL.h>
#include <string>
#include <SDL3/SDL_events.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include <functional>


//might want to sepatare it further

class GUIElement
{
protected:
    //data for drawing basic rectangle
    SDL_FRect rect;
    SDL_Color elementColor = {250, 250, 250, 255};

    //text related data
    TTF_Font *font = NULL;
    SDL_Color *textColor = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Surface *textSurface = NULL;
    SDL_Texture *textTexture = NULL;
    SDL_FRect textRect = rect;
    

    bool IsWithinRectangle(int x, int y){//function for detecting hovering over element
        return (rect.x < x) && (rect.x + rect.w > x) && (rect.y < y) && (rect.y + rect.h > y);
    }

    void SetTextAsTexture(std::string *text){//function for setting text as a texture for further text displaying
        if(!(*text).empty()){
            textSurface = TTF_RenderText_Blended(font, (*text).c_str(), 0, (*textColor));
            textRect = {rect.x, (rect.y + rect.h /2) - (float)textSurface->h / 2, 
                (float)textSurface->w, (float)textSurface->h};
            textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        }else textTexture = NULL;
    }

public:
    virtual void DrawElement(SDL_Renderer *renderer) = 0;

    GUIElement(int x, int y, int w, int h){
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;
    }
};

void DrawBackground(SDL_Renderer *renderer);

#endif