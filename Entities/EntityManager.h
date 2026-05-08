#pragma once
#include "Actor.h"
#include "Player.h"
#include "../DataStructs.h"
#include "Components.h"
#include "Systems.h"
#include <unordered_map>

// ============================================================================
// ENTITY MANAGER - ECS Container & Coordinator
// ============================================================================
// This manager stores all components in separate maps indexed by entity ID.
// 
// Entity ID Scheme:
//   - 0 = Player
//   - 1, 2, 3, ... = Enemies/Traps (dynamic entities)
//
// Component Architecture (Data-Oriented Design):
//   * Entities are just IDs (integers)
//   * Components store pure data, organized in maps by entity ID
//   * Systems iterate through components to implement game logic
//
// Game Loop:
//   EntityManager::UpdateAI()      → Runs AI behavior trees
//   EntityManager::UpdateMovement()  → Updates entity positions
//   EntityManager::UpdateRender()    → Renders all entities
//
// Example: Get all data for entity 5
//   auto& pos = positionComponents[5];
//   auto& move = movementComponents[5];
//   auto& render = renderComponents[5];

struct TempEntity {
    ActorType actorType;
    // Add other fields if needed
};

class EntityManager
{
private:
    Player player = Player();
    Blackboard *bb;
    AdvancedEnemyBlackboard advancedEnemyBlackboard;
    CommandoEnemyBlackboard commandoEnemyBlackboard;
    
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
    std::vector<GridCell> shadowGrid;
    
    void UpdateShadowGrid();
    TempEntity* GetEntityById(int entityID);
    const GridCell* GetGridCell(int x, int y) const;
    
    // Get component maps for direct access (used by systems/rendering)
    std::unordered_map<int, PositionComponent>& GetPositionComponents() { return positionComponents; }
    std::unordered_map<int, MovementComponent>& GetMovementComponents() { return movementComponents; }
    std::unordered_map<int, RenderComponent>& GetRenderComponents() { return renderComponents; }
    std::unordered_map<int, TypeComponent>& GetTypeComponents() { return typeComponents; }
    
    void RequestAddEnemy(Actor newEnemy);
    void SetPlayer(Player newPlayer);
    void RequestRemoveEntityByID(int entityID);

    void DrawEntity(SDL_Renderer *renderer, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize, Actor &actor);
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