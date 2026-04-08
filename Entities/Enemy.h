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
    int goalX = 0, goalY = 0;
    std::unique_ptr<Node> AI;
    std::vector<std::vector<TileData>> *level;

    void DrawEntity(SDL_Renderer *renderer, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize) override;

    Enemy(int x, int y, std::vector<std::vector<TileData>> *level);
};
