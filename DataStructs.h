#pragma once
#include <vector>
#include "TileData.h"

class EntityManager;
class EntityFactory;

struct GridCell {
    int entityIDs[3];
    GridCell(){ Clear(); }
    void Clear(){ for(int i = 0; i < 3; ++i) entityIDs[i] = -1; }
    bool AddEntity(int entityID){
        for(int i = 0; i < 3; ++i){
            if(entityIDs[i] == -1){
                entityIDs[i] = entityID;
                return true;
            }
        }
        return false;
    }
};

struct Blackboard {
    int rows, columns;
    std::vector<std::vector<TileData>> &level;
    EntityManager &entityManager;
    EntityFactory &entityFactory;
};
