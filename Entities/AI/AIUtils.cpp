#include "AIUtils.h"

namespace AIUtils
{
    std::vector<Cell> GetPathToNearestWall(Blackboard &bb, Actor &actor, int dirX, int dirY){
        int checkX = actor.GetPositionX();
        int checkY = actor.GetPositionY();
        std::vector<Cell> pathToWall = {};
        while (true) {//check till first wall
            checkX += dirX;
            checkY += dirY;
            
            if (checkX < 0 || checkX >= bb.columns || checkY < 0 || checkY >= bb.rows){break;/* out of bounds?*/}
            if (!bb.level[checkY][checkX].isWalkable){break; /* out of bounds?*/}
            pathToWall.push_back({checkX, checkY, true});
        }
        return pathToWall;
    }

    std::vector<Cell> GetPathToNextWalkableTile(Blackboard &bb, Actor &actor, int dirX, int dirY){
        int checkX = actor.GetPositionX();
        int checkY = actor.GetPositionY();
        std::vector<Cell> pathToWall = {};
        
        if (checkX < 0 || checkX >= bb.columns || checkY < 0 || checkY >= bb.rows){return pathToWall;/* out of bounds?*/}
        if (bb.level[checkY + dirY][checkX + dirX].isWalkable){
            pathToWall.push_back({checkX + dirX, checkY + dirY, true});
        }
        return pathToWall;
    }

} // namespace AIUtils
