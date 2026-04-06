#pragma once
#include "Entity.h"
#include "./Pathfinding/Pathfinding.cpp"




class Enemy : Entity
{
protected:
    std::vector<Cell> currentPath;
    int goalX, goalY;

public:

    Enemy(/* args */);
    ~Enemy();
};
