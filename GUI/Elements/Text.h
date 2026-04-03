#pragma once
#include "GUIElement.h"

class Text : GUIElement{//class for GUIElements that will display some message
private:
    std::string text = "";
public:
    void DrawElement(SDL_Renderer *renderer){//function to display element
        SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
    };
    
    Text(int x, int y, int width, int hight, std::string text, TTF_Font *font, SDL_Color *textColor, SDL_Renderer *renderer) : GUIElement(x, y, 10, 10){
        this->rect.x = x;
        this->rect.y = y;
        this->rect.w = width;
        this->rect.h = hight;
        this->font = font;
        this->textColor = textColor;
        this->renderer = renderer;

        this->text = text;
        SetTextAsTexture(&text);
    };

    ~Text(){
        
    };
};