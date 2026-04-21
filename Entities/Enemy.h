#pragma once
#include "Entity.h"
#include "../TileData.h"
#include <memory>
#include <vector>

class Cell;
class Node;
class Tile;



class Enemy : public Entity
{
public:
    std::vector<Cell> currentPath;
    std::unique_ptr<Node> AI;
    float visualX, visualY;
    int targetX, targetY;
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

    void DrawEntity(SDL_Renderer *renderer, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize) override;

    void SetTargetPosition(int x, int y);
    bool HasReachedNode();
    void UpdateMovement();
    void Move();

    Enemy(int x, int y, SDL_Texture *tex);
};
