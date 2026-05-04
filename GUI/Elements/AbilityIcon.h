#pragma once
#include <memory>
#include "GUIElement.h"
#include "../../Entities/Abilities.h"

class AbilityIcon : public GUIElement{//class for GUIElements that will display some message
private:
    Ability *ability = nullptr;
    std::string text = "";
public:
    void DrawElement(SDL_Renderer *renderer){//function to display element
        if (!ability || !ability->texture) {
            return;
        }

        SDL_SetRenderDrawColor(renderer, elementColor.r, elementColor.g, elementColor.b, elementColor.a);
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderDrawColor(renderer, 250, 250, 250, elementColor.a);
        SDL_RenderFillRect(renderer, &textureRect);

        SDL_RenderTexture(renderer, ability->texture.get(), NULL, &textureRect);
        
        
        if (ability->cooldownDuration > 0.0f && ability->cooldownRemaining > 0.0f) {
            float ratio = ability->cooldownRemaining / ability->cooldownDuration;
            SDL_FRect cooldownRect = textureRect;
            cooldownRect.w *= ratio;
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
            SDL_RenderFillRect(renderer, &cooldownRect);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }

        if (ability->isConsumable) {
            SDL_FRect tmpRect = {rect.x + rect.w - 40, rect.y + rect.h - 40, 40, 40};
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, elementColor.a);
            SDL_RenderFillRect(renderer, &tmpRect);

        }
    };
    
    void SetTargetAbility(Ability *targetedAbility, SDL_Renderer *renderer){
        this->renderer = renderer;
        this->ability = targetedAbility;
    }
    
    AbilityIcon(int x, int y, int width, int hight) : GUIElement(x, y, width, hight){
        elementColor = { 0, 0, 0, 255};
        textureRect = {
            rect.x + 10,
            rect.y + 10,
            rect.w - 20,
            rect.h - 20
        };
    };
    
    ~AbilityIcon(){
        
    };
};