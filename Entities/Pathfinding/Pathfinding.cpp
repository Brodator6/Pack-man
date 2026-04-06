#include <limits>
#include <vector>
#include <algorithm>
#include <queue>
#include  <Cmath>
#include <iostream>
#include "../../Tile.h"
#include "../../Entities/Entity.h"

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

    static Grid GenerateGrid(std::vector<std::vector<Tile>> &level){
        Grid grid;

        grid.cells.resize(level[0].size(), std::vector<Cell>(level.size(), Cell(0, 0, false)));

        for(int y = 0; y < level.size(); y++){
            for(int x = 0; x < level[0].size(); x++){
                grid.cells[x][y] = Cell(x, y, level[y][x].GetIsWalkable());
            }
        }

        return grid;
    }

    static Grid GenerateGrid(std::vector<std::vector<int>> &level){
        Grid grid;

        grid.cells.resize(level[0].size(), std::vector<Cell>(level.size(), Cell(0, 0, false)));

        for(int y = 0; y < level.size(); y++){
            for(int x = 0; x < level[0].size(); x++){
                grid.cells[x][y] = Cell(x, y, level[y][x]);
            }
        }

        return grid;
    }

    bool ValidateCell(Point position, Cell *targetedCell);
};

class APAthFinding{
    public:
    std::vector<Cell> FindPath(Point startingPosition, Point targetedPosition, Grid *grid){
        std::cout << "0" <<std::endl;
        Cell* start = &grid->cells[startingPosition.x][startingPosition.y];
        Cell* target = &grid->cells[targetedPosition.x][targetedPosition.y];

        start->costFromStart = 0;
        start->costToTarget = GetDistance(start, target);

        std::cout << "1" <<std::endl;
        std::priority_queue<Cell*, std::vector<Cell*>, CellComparator> openSet;
        openSet.push(start);

        std::cout << "2" <<std::endl;
        while(!openSet.empty()){
            std::cout << "loop start" <<std::endl;
            Cell *current = openSet.top();

            if (current == target){
                std::cout << "found" <<std::endl;
                return ReconstructPath(current);
            }
                
            openSet.pop();

            float tentativeCost = current->costFromStart + 1.0f;
            for(Cell* n : FindNeighbors(current, grid)){
                if (tentativeCost < n->costFromStart) {
                    n->parent = current;
                    n->costFromStart = tentativeCost;
                    n->costToTarget = tentativeCost + GetDistance(n, target);
                    
                    // In a basic A*, we just push. If it's already there with a higher cost,
                    // the priority queue will naturally pick the lower cost one first.
                    openSet.push(n);
                }
            }
            
        }
        std::cout << "NotFound" <<std::endl;
        return {};
    }

    static float GetDistance(Cell *start, Cell *target){
        return std::abs(start->x - target->x) + std::abs(start->y - target->y);
    }

    static std::vector<Cell*> FindNeighbors(Cell *cell, Grid *grid){
        std::vector<Cell*> neighbors;
        int dx[] = {-1, 1, 0, 0};
        int dy[] = {0, 0, -1, 1};

        for (int i = 0; i < 4; i++) {
            int checkX = cell->x + dx[i];
            int checkY = cell->y + dy[i];

            // Assuming grid.IsValid and grid.GetCell handle the bounds checks
            if (checkX >= 0
                && checkX < grid->cells[0].size()
                && checkY >= 0
                && checkY < grid->cells.size()
                && grid->cells[checkX][checkY].isWalkable) {
                neighbors.push_back(&grid->cells[checkX][checkY]);
            }
        }
        return neighbors;
    }

    static std::vector<Cell> ReconstructPath(Cell *current){
        std::vector<Cell> path;
        Cell* walkingCell = current;

        while (walkingCell != nullptr) {
            path.push_back(*walkingCell);
            walkingCell = walkingCell->parent;
        }

        // C++ vectors are easier to build by pushing to back, then reversing
        std::reverse(path.begin(), path.end());

        for (size_t i = 0; i < path.size(); ++i) {
            std::cout << "Step " << i << ": [" << path[i].x << ", " << path[i].y << "]" << std::endl;
        }

        return path;
    }
};
