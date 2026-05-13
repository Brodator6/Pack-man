#pragma once

#include "Player.h"
#include "../TileData.h"
#include "../DataStructs.h"
#include "EntityManager.h"
#include "EntityFactory.h"

#include <SDL3/SDL.h>
#include <chrono>
#include <memory>

class GameManager
{
public:
    std::vector<std::vector<TileData>> level;

    TimeBlackboard &timeBlackboard;

    EntityFactory entityFactory = EntityFactory();
    EntityManager entityManager;
    AbilityFactory abilityFactory;
    Blackboard blackboard = {
        20, 20,
        level,
        entityManager,
        entityFactory,
        abilityFactory
    };

    bool lost = false;
    bool won = false;


    void GenerateLevel();

    void HandleGameLogic();

    void UpdateState();
    GameManager(SDL_Renderer *renderer, TimeBlackboard &tBB);
    ~GameManager(){};
};
