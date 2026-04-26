#pragma once
#include "Entity.h"
#include "../TileData.h"
#include <memory>
#include <vector>

class Cell;
class Node;
class Tile;

// Forward declarations for blackboards
struct AdvancedEnemyBlackboard;

enum class EnemyType {
    BasicEnemy,
    AdvancedEnemy,
    CommandoEnemy,
    None
};

class Actor : public Entity
{
public:
    int entityID = -1; //need inspection on if it can be removed/optimized
    float visualX, visualY;
    int targetX, targetY;
    EnemyType enemyType = EnemyType::None; //*//

    std::vector<Cell> currentPath;
    std::unique_ptr<Node> AI;

    union{
        struct{
            int timer, cooldown;
        } StaticEntity;

        struct{
            float speedModifier = 1.0f;
            int goalX, goalY;
            int LastSeenPlayerX, LastSeenPlayerY;
            bool isChasing;
        } DinamicEntity;
    };

    // Separate pointers for different blackboard types (not in union)
    // Only one will be used based on enemyType
    AdvancedEnemyBlackboard *advancedEnemyBB = nullptr;//*//

    void DrawEntity(SDL_Renderer *renderer, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize) override;

    void SetTargetPosition(int x, int y);
    bool HasReachedNode();
    void UpdateMovement();
    void Move();

    Actor(int x, int y, SDL_Texture *tex);
};
