#pragma once
#include "GUIElement.h"

class Button : public GUIElement{//class for GUIElements that will perform action on click
private:
    int darkening = 0;
    std::function<void()>functionPointer;

public:

    void DrawElement(SDL_Renderer *renderer) override{//function to display element
        SDL_SetRenderDrawColor(renderer, elementColor.r + darkening, elementColor.g + darkening, elementColor.b + darkening, elementColor.a);
        SDL_RenderFillRect(renderer, &rect);
        SDL_RenderTexture(renderer, texture.get(), NULL, &textureRect);
    };

    void HandleKey(SDL_Event &Event) override{//function to handle element event
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

    Button(int x, int y, int w, int h, std::function<void()> function, const std::string &text, TTF_Font *font, const SDL_Color &textColor, SDL_Renderer *renderer) : GUIElement(x, y, w, h){
        this->renderer = renderer;
        this->functionPointer = function;
        texture = std::shared_ptr<SDL_Texture>(SetTextAsTexture(renderer, textureRect, rect, text, textColor, font), SDL_DestroyTexture);
    }

    Button(int x, int y, int w, int h, std::function<void()> function, SDL_Texture *tex, SDL_Renderer *renderer) : GUIElement(x, y, w, h){
        this->renderer = renderer;
        this->functionPointer = function;
        texture = std::shared_ptr<SDL_Texture>(SetTexture(renderer, textureRect, tex), SDL_DestroyTexture);
    }
};