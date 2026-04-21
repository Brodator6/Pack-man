#pragma once
#include "Enemy.h"
#include "Player.h"
#include "../DataStructs.h"

struct Blackboard;

class EntityManager
{
private:
    Player player = Player();
    Blackboard *bb;

    static constexpr int kPlayerEntityID = 0;
    static constexpr int kEnemyEntityIdOffset = 1;
public:
    std::vector<GridCell> shadowGrid;
    std::vector<Enemy> enemies;
    
    void UpdateShadowGrid();
    Entity* GetEntityById(int entityID);
    const GridCell* GetGridCell(int x, int y) const;
    
    void AddEnemy();
    void SetPlayer();

    Player& GetPlayer(){return player;};
    EntityManager();
    void SetUp(Blackboard &bb);
    ~EntityManager() = default;
};