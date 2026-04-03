#pragma once
#include "GUIElement.h"

class TextBox : GUIElement{//class for GUIElements that will gather information that user pasted
private:
    SDL_Rect textAreaRect = {(int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h};
    std::string content = "template";
    bool isFocused = false;
    int darkening = 0;

public:
    void DrawElement(SDL_Renderer *renderer){//function to display element
        SDL_SetRenderDrawColor(renderer, elementColor.r + darkening, elementColor.g + darkening, elementColor.b + darkening, elementColor.a);
        SDL_RenderFillRect(renderer, &rect);
        SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
    };

    void HandleKey(SDL_Event Event, SDL_Window *window, int *inactiveTextBoxCounter){//function to handle element event
        if(IsWithinRectangle((int)Event.motion.x ,(int)Event.motion.y)){
            darkening = -55;
        }else{
            darkening = 0;
        }

        if(Event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && IsWithinRectangle((int)Event.button.x ,(int)Event.button.y)){
            isFocused = true;
            if (SDL_StartTextInput(window)){
            }
            SDL_SetTextInputArea(window, &textAreaRect, (int)Event.button.x - (int)rect.x);


        }else if(Event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && !IsWithinRectangle((int)Event.button.x ,(int)Event.button.y)){
            isFocused = false;
            (*inactiveTextBoxCounter)++;
        }

        if(Event.type == SDL_EVENT_TEXT_INPUT && isFocused){
            content += Event.text.text;
            SetTextAsTexture(&content);
            std::cout << "content: " << content << std::endl;
        }

        if(Event.key.key == SDLK_BACKSPACE && isFocused){
            if(!content.empty()){
                content.pop_back();
                SetTextAsTexture(&content);
                std::cout << "content: " << content << std::endl;
            }
        }
    }

    std::string GetValue(){
        return content;//getter for content
    }

    TextBox(int x, int y, int w, int h, std::string *content, TTF_Font *font, SDL_Color *textColor, SDL_Renderer *renderer) : GUIElement(x, y, w, h){
        this->font = font;
        this->textColor = textColor;
        this->renderer = renderer;
        this->content = (*content);
        SetTextAsTexture(content);
    }

};