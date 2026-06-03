#pragma once

#include "Entities/Player.h"
#include "TileData.h"
#include "DataStructs.h"
#include "Entities/EntityManager.h"
#include "Entities/EntityFactory.h"

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

    bool lost = false;
    int globalScore = 0;
    int scoreGoal = 20;
    std::string currentLevelPath = "./Assets/Levels/level1.json";
    std::string currentLevelName = "";

    Blackboard blackboard = {
        25, 25,
        level,
        entityManager,
        entityFactory,
        abilityFactory
    };

    void GenerateLevel(const std::string &levelPath);
    void HandleGameLogic();
    void UpdateGameState();

    GameManager(SDL_Renderer *renderer, TimeBlackboard &tBB);
    GameManager(SDL_Renderer *renderer, TimeBlackboard &tBB, const GameManager &previousManager);
    ~GameManager(){};
};
