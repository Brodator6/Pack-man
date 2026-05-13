#pragma once
#include <vector>
#include <chrono>
#include "TileData.h"
#include <SDL3/SDL.h>

class EntityManager;
class EntityFactory;
class AbilityFactory;
class Ability;

struct GridCell {
    int entityIDs[3];
    GridCell(){ Clear(); }
    void Clear(){ for(int i = 0; i < 3; ++i) entityIDs[i] = -1; }
    bool AddEntity(int entityID){
        for(int i = 0; i < 3; ++i){
            if(entityIDs[i] == -1){
                entityIDs[i] = entityID;
                return true;
            }
        }
        return false;
    }
};

struct Blackboard {
    int rows, columns;
    std::vector<std::vector<TileData>> &level;
    EntityManager &entityManager;
    EntityFactory &entityFactory;
    AbilityFactory &abilityFactory;
};

struct MenuBlackboard{
    SDL_Window **window = NULL;
    SDL_Renderer **renderer = NULL;

    int windowWidth, windowHight;
};

struct AbilityMenuBlackboard {
    Ability* playerAbilities = nullptr;           // Pointer to player's abilities array (size 4)
    AbilityFactory* abilityFactory = nullptr;     // Factory to create new abilities
    SDL_Renderer* renderer = nullptr;             // Renderer for creating textures
};

struct TimeBlackboard{
    //data to manage game loop
    std::chrono::duration<double> deltaTime = std::chrono::duration<float>(1.0/20.0);

    std::chrono::time_point<std::chrono::high_resolution_clock> previousTickTime = std::chrono::high_resolution_clock::now(); // might want to replace auto with smth different
    std::chrono::time_point<std::chrono::high_resolution_clock> currentTime = std::chrono::high_resolution_clock::now();
    
    std::chrono::time_point<std::chrono::high_resolution_clock> frameEnd = std::chrono::high_resolution_clock::now(); // might want to replace auto with smth different
    std::chrono::duration<double>  frameDuration = std::chrono::duration<float>(0.0);
};