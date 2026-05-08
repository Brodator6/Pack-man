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

//needs reworking
// Structure to hold shared target information between AI agents of the same type
struct SharedTargetInfo {
    int lastSeenX = -1;
    int lastSeenY = -1;
    int reporterID = -1;  // Which enemy reported this position
    int updateTickCount = 0; // How recent is this information
};

// Base blackboard for enemies with cooperative behavior
struct AdvancedEnemyBlackboard {
    // List of IDs belonging to advanced enemies that can share information
    std::vector<int> advancedEnemyIDs;
    
    // Shared target information accessible to all advanced enemies
    SharedTargetInfo sharedTarget;
    
    // Find or create entry for an enemy
    int GetOrCreateEnemyIndex(int enemyID) {
        for (size_t i = 0; i < advancedEnemyIDs.size(); ++i) {
            if (advancedEnemyIDs[i] == enemyID) return i;
        }
        advancedEnemyIDs.push_back(enemyID);
        return advancedEnemyIDs.size() - 1;
    }
    
    // Update shared target information
    void ShareTargetInformation(int enemyID, int playerX, int playerY, int currentTick) {
        sharedTarget.lastSeenX = playerX;
        sharedTarget.lastSeenY = playerY;
        sharedTarget.reporterID = enemyID;
        sharedTarget.updateTickCount = currentTick;
    }
    
    // Get the most recent target information from all allies
    SharedTargetInfo GetLatestSharedTarget() {
        return sharedTarget;
    }
};

// Extended blackboard for Commando enemies (future extension)
// Will have a commander that collects and coordinates information
struct CommandoEnemyBlackboard : public AdvancedEnemyBlackboard{
    // Commander ID (-1 if not assigned)
    int commanderID = -1;
    int commandTickCount = 0;
    
    // Commander election based on ticks
    void UpdateCommanderElection(int tickCount) {
        commandTickCount = tickCount;
    }
};