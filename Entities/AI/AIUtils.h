#include <vector>
#include <memory>
#include <stdlib.h> // Required for rand() and srand()
#include <ctime>   // Required if you use time(0) to seed rand
#include <iostream>
#include "../Enemy.h"
#include "../Pathfinding/Pathfinding.h"
#include "../../DataStructs.h"
#include "../EntityManager.h"

namespace AIUtils
{
    std::vector<Cell> GetPathToNearestWall(Blackboard &bb, Enemy &enemy, int dirX, int dirY);
    std::vector<Cell> GetPathToNextWalkableTile(Blackboard &bb, Enemy &enemy, int dirX, int dirY);
    //std::tuple<int, int, bool> DesideTurnOnJunction(int x, int y, int dirX, int DirY, Blackboard &bb);

} // namespace AIUtils
