#pragma once
#include "Player.h"
#include "../DataStructs.h"
#include "Components.h"
#include "Systems.h"
#include <unordered_map>


struct TempEntity {
    ActorType actorType;
    // Add other fields if needed
};

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
    Grid pathfindingGrid;
    AdvancedEnemyBlackboard advancedEnemyBlackboard;
    CommandoEnemyBlackboard commandoEnemyBlackboard;
    std::vector<GridCell> shadowGrid;
    
    void UpdateShadowGrid();
    void UpdatePathfindingGrid();
    TempEntity* GetEntityById(int entityID);
    const GridCell* GetGridCell(int x, int y) const;
    
    // Get component maps for direct access (used by systems/rendering)
    std::unordered_map<int, PositionComponent>& GetPositionComponents() { return positionComponents; }
    std::unordered_map<int, MovementComponent>& GetMovementComponents() { return movementComponents; }
    std::unordered_map<int, RenderComponent>& GetRenderComponents() { return renderComponents; }
    std::unordered_map<int, TypeComponent>& GetTypeComponents() { return typeComponents; }
    std::unordered_map<int, AIComponent>& GetAIComponents() { return aiComponents; }
    std::unordered_map<int, StaticEntityComponent>& GetStaticComponents() { return staticComponents; }
    std::unordered_map<int, BlackboardComponent>& GetBlackboardComponents() { return blackboardComponents; }
    int AllocateEntityID() { return kEnemyEntityIdOffset + static_cast<int>(positionComponents.size()); }
    
    void AddEntity(int x, int y, EntityDirection direction, EntityType type);
    void SetPlayer(Player newPlayer);
    void RequestRemoveEntityByID(int entityID);

    void MoveEntity();

    void UpdateState();

    // Get the appropriate blackboard for an enemy
    AdvancedEnemyBlackboard& GetAdvancedEnemyBlackboard() { return advancedEnemyBlackboard; }
    CommandoEnemyBlackboard& GetCommandoEnemyBlackboard() { return commandoEnemyBlackboard; }

    Player& GetPlayer(){return player;};
    EntityManager();
    void SetUp(Blackboard &bb);
    ~EntityManager();
    // ECS methods
    void UpdateAI(Blackboard& blackboard);
    void UpdateMovement();
    void UpdateRender(SDL_Renderer* renderer, int cellWidth, int cellHeight, int widthMargin, int heightMargin, int squareSize);};