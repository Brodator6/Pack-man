#include "Pathfinding.h"
#include <algorithm>
#include <queue>
#include <Cmath>
#include <iostream>
#include "../../Tile.h"
#include "../../Entities/Enemy.h"

//class forwarding
class Grid;

Grid Grid::GenerateGrid(std::vector<std::vector<std::unique_ptr<Tile>>> *level){
        Grid grid;

        grid.cells.resize(level[0].size(), std::vector<Cell>(level->size(), Cell(0, 0, false)));

        for(int y = 0; y < level->size(); y++){
            for(int x = 0; x < level[0].size(); x++){
                grid.cells[x][y] = Cell(x, y, (*level)[y][x]->GetIsWalkable());
            }
        }

        return grid;
}

/*
Grid Grid::GenerateGrid(std::vector<std::vector<int>> &level){
    Grid grid;

    grid.cells.resize(level[0].size(), std::vector<Cell>(level.size(), Cell(0, 0, false)));

    for(int y = 0; y < level.size(); y++){
        for(int x = 0; x < level[0].size(); x++){
            grid.cells[x][y] = Cell(x, y, level[y][x]);
        }
    }

    return grid;
}
*/

std::vector<Cell> APAthFinding::FindPath(Point startingPosition, Point targetedPosition, Grid *grid){
    std::cout << "start" <<std::endl;
    std::cout << grid->cells.size() <<std::endl;

    Cell* start = &grid->cells[startingPosition.x][startingPosition.y];
    Cell* target = &grid->cells[targetedPosition.x][targetedPosition.y];

    start->costFromStart = 0;
    start->costToTarget = GetDistance(start, target);


    std::priority_queue<Cell*, std::vector<Cell*>, CellComparator> openSet;
    openSet.push(start);

    std::cout << "3" <<std::endl;
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

float APAthFinding::GetDistance(Cell *start, Cell *target){
    return std::abs(start->x - target->x) + std::abs(start->y - target->y);
}

std::vector<Cell*> APAthFinding::FindNeighbors(Cell *cell, Grid *grid){
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

std::vector<Cell> APAthFinding::ReconstructPath(Cell *current){
    std::vector<Cell> path;
    Cell* walkingCell = current;

    while (walkingCell != nullptr) {
        path.push_back(*walkingCell);
        walkingCell = walkingCell->parent;
    }

    for (size_t i = 0; i < path.size(); ++i) {
        std::cout << "Step " << i << ": [" << path[i].x << ", " << path[i].y << "]" << std::endl;
    }
    std::cout << "Path found" <<std::endl;
    return path;
}