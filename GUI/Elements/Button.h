#pragma once
#include "GUIElement.h"

class Button : GUIElement{//class for GUIElements that will perform action on click
private:
    std::string text = "";
    int darkening = 0;
    std::function<void()>functionPointer;

public:

    void DrawElement(SDL_Renderer *renderer){//function to display element
        SDL_SetRenderDrawColor(renderer, elementColor.r + darkening, elementColor.g + darkening, elementColor.b + darkening, elementColor.a);
        SDL_RenderFillRect(renderer, &rect);
        SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
    };

    void HandleKey(SDL_Event Event){//function to handle element event
        if(IsWithinRectangle((int)Event.motion.x ,(int)Event.motion.y)){
            darkening = -55;
        }else{
            darkening = 0;
        }
        if(Event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && IsWithinRectangle((int)Event.button.x ,(int)Event.button.y)){
            /* handle button click */
            darkening = -55;
            functionPointer();
        }
    }

    Button(int x, int y, int w, int h, std::function<void()> function, std::string text, TTF_Font *font, SDL_Color *textColor, SDL_Renderer *renderer) : GUIElement(x, y, w, h){
        this->text = text;
        this->font = font;
        this->textColor= textColor;
        this->renderer = renderer;
        this->functionPointer = function;
        SetTextAsTexture(&this->text);
    }
};