#pragma once
#include "GUIElement.h"

class ScoreIcon : public GUIElement{//class for GUIElements that will display some message
private:
    std::string text = "Score: 0";
    int currentScore = 0;
public:
    void DrawElement(SDL_Renderer *renderer) override{//function to display element
        SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);
        SDL_RenderFillRect(renderer, &rect);

        float ratio = currentScore / 200.0f;
        SDL_FRect ScoreBar = rect;
        ScoreBar.w *= ratio;

        SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255);
        SDL_RenderFillRect(renderer, &ScoreBar);

        SDL_RenderTexture(renderer, texture.get(), NULL, &textureRect);
    };
    
    void UpdateTextTexture(std::string newText, int newScore, TTF_Font *font, const SDL_Color &textColor){
        if(text != newText){
            currentScore = newScore;
            text = newText;
            texture = std::shared_ptr<SDL_Texture>(SetTextAsTextureCentered(renderer, textureRect, rect, text, textColor, font), SDL_DestroyTexture);
        }
    }

    ScoreIcon(int x, int y, int width, int hight, const int score, const std::string &text, TTF_Font *font, const SDL_Color &textColor, SDL_Renderer *renderer) : GUIElement(x, y, width, hight){
        this->renderer = renderer;
        this->text = text;
        currentScore = score;
        texture = std::shared_ptr<SDL_Texture>(SetTextAsTextureCentered(renderer, textureRect, rect, text, textColor, font), SDL_DestroyTexture);

    }

    ~ScoreIcon(){
        
    };
};