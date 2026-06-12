#pragma once
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

enum class AbilityType{
    None,
    Mine,
    WallCharge,
    RoadBlocker,
    SpeedBoost,
    Invisibility
};

enum AbilityID{
    permanentAbility1 = 0,
    permanentAbility2 = 1,
    consumableAbility1 = 2,
    consumableAbility2 = 3
};

struct Ability{
    AbilityType type;
    float cooldownRemaining;
    float cooldownDuration;
    float durationRemaining;
    float duration;
    int charges;
    bool isConsumable;

    std::shared_ptr<SDL_Texture> texture{nullptr};
};


class AbilityFactory
{
private:
    SDL_Renderer *renderer = nullptr;
public:
    Ability CreateAbility(AbilityType type){
        Ability ability;
        ability.type = AbilityType::None;
        ability.cooldownRemaining = 0.0f;
        ability.cooldownDuration = 0.0f;
        ability.durationRemaining = 0.0f;
        ability.duration = 0.0f;
        ability.charges = 0;
        ability.isConsumable = false;
        ability.texture = nullptr;


        switch (type)
        {
        case AbilityType::Mine:{
            ability.type = AbilityType::Mine;
            ability.cooldownDuration = 5.0f;
            ability.duration = 0.0f;
            ability.charges = 5;
            ability.isConsumable = true;
            ability.texture = std::shared_ptr<SDL_Texture>(IMG_LoadTexture(renderer, "./Assets/Sprites/MineSprite.png"), SDL_DestroyTexture);
            break;
        }

        case AbilityType::WallCharge:{
            ability.type = AbilityType::WallCharge;
            ability.cooldownDuration = 50.0f;
            ability.duration = 0.0f;
            ability.charges = 5;
            ability.isConsumable = true;
            ability.texture = std::shared_ptr<SDL_Texture>(IMG_LoadTexture(renderer, "./Assets/Sprites/WallChargeSprite.png"), SDL_DestroyTexture);
            break;
        }

        case AbilityType::RoadBlocker:{
            ability.type = AbilityType::RoadBlocker;
            ability.cooldownDuration = 50.0f;
            ability.duration = 0.0f;
            ability.charges = 3;
            ability.isConsumable = true;
            ability.texture = std::shared_ptr<SDL_Texture>(IMG_LoadTexture(renderer, "./Assets/Sprites/RoadBlockerSprite.png"), SDL_DestroyTexture);
            break;
        }

        case AbilityType::SpeedBoost:{
            ability.type = AbilityType::SpeedBoost;
            ability.cooldownDuration = 40.0f;
            ability.duration = 3.0f;
            ability.charges = 0;
            ability.isConsumable = false;
            ability.texture = std::shared_ptr<SDL_Texture>(IMG_LoadTexture(renderer, "./Assets/Sprites/SpeedBoostSprite.png"), SDL_DestroyTexture);
            break;
        }

        case AbilityType::Invisibility:{
            ability.type = AbilityType::Invisibility;
            ability.cooldownDuration = 40.0f;
            ability.duration = 5.0f;
            ability.charges = 0;
            ability.isConsumable = false;
            ability.texture = std::shared_ptr<SDL_Texture>(IMG_LoadTexture(renderer, "./Assets/Sprites/InvisibilitySprite.png"), SDL_DestroyTexture);
            break;
        }
        
        default:
            break;
        }

        return ability;
    }
    AbilityFactory(SDL_Renderer *rend){
        renderer = rend;
    };

    ~AbilityFactory(){
        
    };
};
