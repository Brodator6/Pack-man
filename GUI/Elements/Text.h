#pragma once
#include "GUIElement.h"

class Text : public GUIElement{//class for GUIElements that will display some message
private:
    std::string text = "Score: 0";
public:
    void DrawElement(SDL_Renderer *renderer) override{//function to display element
        SDL_RenderTexture(renderer, texture.get(), NULL, &textureRect);
    };
    
    void UpdateTextTexture(std::string newText, TTF_Font *font, const SDL_Color &textColor){
        if(text != newText){
            text = newText;
            texture = std::shared_ptr<SDL_Texture>(SetTextAsTexture(renderer, textureRect, rect, text, textColor, font), SDL_DestroyTexture);
        }
    }

    Text(int x, int y, int width, int hight, const std::string &text, TTF_Font *font, const SDL_Color &textColor, SDL_Renderer *renderer) : GUIElement(x, y, width, hight){
        this->renderer = renderer;
        this->text = text;
        texture = std::shared_ptr<SDL_Texture>(SetTextAsTexture(renderer, textureRect, rect, text, textColor, font), SDL_DestroyTexture);
    }

    ~Text(){
        
    };
};