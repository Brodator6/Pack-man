#pragma once
#include <memory>
#include "GUIElement.h"
#include "../../Entities/Abilities.h"

class AbilityIcon : public GUIElement{//class for GUIElements that will display some message
private:
    Ability *ability = nullptr;
    std::string name = "";
    std::string chargesText = "0";
    std::shared_ptr<SDL_Texture> chargesTexture = nullptr;
    SDL_FRect chargesTextureRect = {0, 0, 0, 0};
public:
    void DrawElement(SDL_Renderer *renderer) override{//function to display element

        SDL_SetRenderDrawColor(renderer, elementColor.r, elementColor.g, elementColor.b, elementColor.a);
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderDrawColor(renderer, 250, 250, 250, elementColor.a);
        SDL_RenderFillRect(renderer, &textureRect);
        
        if(ability && ability->texture.get() != nullptr){
            SDL_RenderTexture(renderer, ability->texture.get(), NULL, &textureRect);
        }
        
        
        if (ability && ability->cooldownDuration > 0.0f && ability->cooldownRemaining > 0.0f) {
            float ratio = ability->cooldownRemaining / ability->cooldownDuration;
            SDL_FRect cooldownRect = textureRect;
            cooldownRect.w *= ratio;
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
            SDL_RenderFillRect(renderer, &cooldownRect);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }

        if (ability && ability->isConsumable) {
            SDL_FRect tmpRect = {rect.x + rect.w - 40, rect.y + rect.h - 40, 40, 40};
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, elementColor.a);
            SDL_RenderFillRect(renderer, &tmpRect);


            SDL_RenderTexture(renderer, chargesTexture.get(), NULL, &chargesTextureRect);
        }
    };
    
    void UpdateChargesTexture(TTF_Font *font, const SDL_Color &textColor){
        if(!ability || !renderer || !font || !ability->isConsumable){
            chargesTexture.reset();
            chargesText = "0";
            return;
        }

        std::string newText = std::to_string(ability->charges);
        if(chargesTexture == nullptr || chargesText != newText){
            chargesText = newText;
            SDL_FRect tmpRect = {rect.x + rect.w - 40, rect.y + rect.h - 40, 40, 40};
            chargesTexture = std::shared_ptr<SDL_Texture>(
                SetTextAsTextureCentered(renderer, chargesTextureRect, tmpRect, chargesText, {255, 255, 255, 0}, font),
                SDL_DestroyTexture);
        }
    }

    void SetTargetAbility(Ability *targetedAbility, SDL_Renderer *renderer, TTF_Font *font, const SDL_Color &textColor){
        this->renderer = renderer;
        this->ability = targetedAbility;
        UpdateChargesTexture(font, textColor);
    }
    
    AbilityIcon(int x, int y, int width, int hight) : GUIElement(x, y, width, hight){
        elementColor = { 0, 0, 0, 255};
        textureRect = {
            rect.x + 10,
            rect.y + 10,
            rect.w - 20,
            rect.h - 20
        };
        chargesTextureRect = {rect.x + rect.w - 40, rect.y + rect.h - 40, 40, 40};
    };
    
    ~AbilityIcon(){
        
    };
};