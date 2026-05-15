#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <SDL3/SDL_events.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include <functional>
#include <memory>


//might want to sepatare it further

class GUIElement
{
protected:
    //data for drawing basic rectangle
    SDL_FRect rect;
    SDL_Color elementColor = {250, 250, 250, 255};

    //text related data
    SDL_Renderer *renderer = NULL;
    std::shared_ptr<SDL_Texture> texture = NULL;
    SDL_FRect textureRect;
    

    bool IsWithinRectangle(int x, int y){//function for detecting hovering over element
        return (rect.x < x) && (rect.x + rect.w > x) && (rect.y < y) && (rect.y + rect.h > y);
    }

public:
    virtual void DrawElement(SDL_Renderer *renderer){};

    virtual void HandleKey(SDL_Event &Event){};

    GUIElement(int x, int y, int w, int h){
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;
        textureRect = rect;
    }
};

void DrawBackground(SDL_Renderer *renderer);

SDL_Texture *SetTextAsTexture(SDL_Renderer *renderer, SDL_FRect &textureRect, const SDL_FRect &rect, const std::string &text, const SDL_Color &textColor, TTF_Font *font);

SDL_Texture *SetTexture(SDL_Renderer *renderer, SDL_FRect &textRect, const SDL_Texture *tex);