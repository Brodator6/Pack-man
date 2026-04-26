#pragma once
#include "Actor.h"
#include "Player.h"
#include "../DataStructs.h"

struct Blackboard;

class EntityManager
{
private:
    Player player = Player();
    Blackboard *bb;
    AdvancedEnemyBlackboard advancedEnemyBlackboard;
    CommandoEnemyBlackboard commandoEnemyBlackboard;
    
    std::vector<int> removalQueue = {};

    static constexpr int kEnemyEntityIdOffset = 1;
public:
    std::vector<GridCell> shadowGrid;
    std::vector<Actor> enemies;
    
    void UpdateShadowGrid();
    Entity* GetEntityById(int entityID);
    const GridCell* GetGridCell(int x, int y) const;
    
    void RequestAddEnemy(Actor newEnemy);
    void SetPlayer(Player newPlayer);
    void RequestRemoveEntityByID(int entityID);

    void UpdateState();

    // Get the appropriate blackboard for an enemy
    AdvancedEnemyBlackboard& GetAdvancedEnemyBlackboard() { return advancedEnemyBlackboard; }
    CommandoEnemyBlackboard& GetCommandoEnemyBlackboard() { return commandoEnemyBlackboard; }

    Player& GetPlayer(){return player;};
    EntityManager();
    void SetUp(Blackboard &bb);
    ~EntityManager() = default;
};