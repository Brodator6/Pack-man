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
    float visualX, visualY;
    int targetX, targetY;
    float speedModifier = 1.0f;
    int goalX, goalY;
    std::unique_ptr<Node> AI;

    void DrawEntity(SDL_Renderer *renderer, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize) override;

    void SetTargetPosition(int x, int y);
    bool HasReachedNode() const;
    void UpdateMovement();
    void Move();

    Enemy(int x, int y, SDL_Texture *tex);
};
