#pragma once
#include "Player.h"
#include "../DataStructs.h"
#include "Components.h"
#include "Systems.h"
#include <unordered_map>


class EntityManager
{
private:
    Player player = Player();
    Blackboard *blackboard;
    
    std::vector<int> removalQueue = {};

    static constexpr int kEnemyEntityIdOffset = 1;

    // Component storage
    std::unordered_map<int, PositionComponent> positionComponents;
    std::unordered_map<int, MovementComponent> movementComponents;
    std::unordered_map<int, AIComponent> aiComponents;
    std::unordered_map<int, RenderComponent> renderComponents;
    std::unordered_map<int, TypeComponent> typeComponents;
    std::unordered_map<int, StaticEntityComponent> staticComponents;
    std::unordered_map<int, BlackboardComponent> blackboardComponents;

    // Systems
    AISystem aiSystem;
    MovementSystem movementSystem;
    RenderSystem renderSystem;
public:
    AdvancedEnemyBlackboard advancedEnemyBlackboard;
    CommandoEnemyBlackboard commandoEnemyBlackboard;
    std::vector<GridCell> shadowGrid;
    
    // Get component maps for direct access (used by systems)
    std::unordered_map<int, PositionComponent>& GetPositionComponents() { return positionComponents; }
    std::unordered_map<int, MovementComponent>& GetMovementComponents() { return movementComponents; }
    std::unordered_map<int, RenderComponent>& GetRenderComponents() { return renderComponents; }
    std::unordered_map<int, TypeComponent>& GetTypeComponents() { return typeComponents; }
    std::unordered_map<int, AIComponent>& GetAIComponents() { return aiComponents; }
    std::unordered_map<int, StaticEntityComponent>& GetStaticComponents() { return staticComponents; }
    std::unordered_map<int, BlackboardComponent>& GetBlackboardComponents() { return blackboardComponents; }
    
    //Get the player
    Player& GetPlayer(){return player;};
    const Player& GetPlayer() const { return player;};

    // Get the appropriate blackboard for an enemy
    AdvancedEnemyBlackboard& GetAdvancedEnemyBlackboard() { return advancedEnemyBlackboard; }
    CommandoEnemyBlackboard& GetCommandoEnemyBlackboard() { return commandoEnemyBlackboard; }

    int AllocateEntityID() { return kEnemyEntityIdOffset + static_cast<int>(positionComponents.size()); }
    
    void UpdateShadowGrid();
    const GridCell* GetGridCell(int x, int y) const;

    //entity manipulations
    void AddEntity(int x, int y, EntityDirection direction, EntityType type);
    void SetPlayer(Player newPlayer);
    void RequestRemoveEntityByID(int entityID);

    void UpdateState();//

    // functions for system calls
    void UpdateAI(Blackboard& blackboard);
    void UpdateMovement();
    void UpdateRender(SDL_Renderer* renderer, int cellWidth, int cellHeight, int widthMargin, int heightMargin, int squareSize, bool freezeAnimations);
    
    EntityManager();
    void SetUp(Blackboard &bb);
    ~EntityManager();
};