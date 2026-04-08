#pragma once
#include <limits>
#include <vector>
#include <memory>


class Tile;
class Enemy;

struct Point{
    int x;
    int y;
};

struct Cell{
    int x,y;
    bool isWalkable;
    float costToTarget, costFromStart;
    Cell *parent;

    Cell(int x, int y, bool isWalkable)
        :costToTarget(std::numeric_limits<float>::infinity()), 
        costFromStart(std::numeric_limits<float>::infinity())
    {
        this->x = x;
        this->y = y;
        this->isWalkable = isWalkable;
        parent = nullptr;
    }
};

struct CellComparator{
    bool operator()(const Cell* a, const Cell* b) const {
        return a->costToTarget > b->costToTarget;
    }
};

class Grid{
    public:
    std::vector<std::vector<Cell>> cells;

    static Grid GenerateGrid(std::vector<std::vector<std::unique_ptr<Tile>>> *level);

    static Grid GenerateGrid(std::vector<std::vector<int>> &level);

    bool ValidateCell(Point position, Cell *targetedCell);
};

class APAthFinding{
    public:
    std::vector<Cell> FindPath(Point startingPosition, Point targetedPosition, Grid *grid);

    static float GetDistance(Cell *start, Cell *target);

    static std::vector<Cell*> FindNeighbors(Cell *cell, Grid *grid);

    static std::vector<Cell> ReconstructPath(Cell *current);
};
