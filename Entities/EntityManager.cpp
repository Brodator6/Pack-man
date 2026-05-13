#include "EntityManager.h"
#include "EntityFactory.h"
#include "AI/BehaviorTree.h"

void EntityManager::UpdateShadowGrid(){

    if(static_cast<int>(shadowGrid.size()) != blackboard->rows * blackboard->columns){
        shadowGrid.assign(blackboard->rows * blackboard->columns, GridCell());
    }

    for(auto &cell : shadowGrid){
        cell.Clear();
    }

    int playerIndex = player.GetPositionY() * blackboard->columns + player.GetPositionX();
    if(!player.isInvisible && player.ID >= 0 && playerIndex >= 0 && playerIndex < static_cast<int>(shadowGrid.size())){
        shadowGrid[playerIndex].AddEntity(player.ID);
    }

    for(auto& [entityID, pos] : positionComponents){
        if(entityID >= kEnemyEntityIdOffset){
            int enemyX = pos.x;
            int enemyY = pos.y;
            if(enemyX < 0 || enemyX >= blackboard->columns || enemyY < 0 || enemyY >= blackboard->rows){
                continue;
            }
            int enemyIndex = enemyY * blackboard->columns + enemyX;
            shadowGrid[enemyIndex].AddEntity(entityID);
        }
    }
}

void EntityManager::UpdatePathfindingGrid(){
    pathfindingGrid = Grid::GenerateGrid(&blackboard->level);
}

TempEntity* EntityManager::GetEntityById(int entityID){
    if(entityID == player.ID){
        static TempEntity tempPlayer = {ActorType::DinamicActor};
        return &tempPlayer;
    }

    auto typeIt = typeComponents.find(entityID);
    if(typeIt != typeComponents.end()){
        static TempEntity tempEnemy = {typeIt->second.actorType};
        return &tempEnemy;
    }

    return nullptr;
}

const GridCell* EntityManager::GetGridCell(int x, int y) const{
    if(x < 0 || x >= blackboard->columns || y < 0 || y >= blackboard->rows){
        std::cout << "issue with getting grid cell" << std:: endl;
        return nullptr;
    }
    return &shadowGrid[y * blackboard->columns + x];
}

void EntityManager::RequestRemoveEntityByID(int entityID){
    removalQueue.push_back(entityID);
}

void EntityManager::SetPlayer(Player newPlayer){
    player = newPlayer;
};

void EntityManager::UpdateState(){
    if(!removalQueue.empty()){
        for(int id : removalQueue){
            if(id == player.ID){
                //show score window with "try again"
                std::cout << "death" << std::endl;
                return;
            }
            // Remove from all component maps
            positionComponents.erase(id);
            movementComponents.erase(id);
            aiComponents.erase(id);
            renderComponents.erase(id);
            typeComponents.erase(id);
            staticComponents.erase(id);
            blackboardComponents.erase(id);
        }
        removalQueue.clear();
    }
}

EntityManager::EntityManager(){
};

EntityManager::~EntityManager(){
};

void EntityManager::SetUp(Blackboard &bb){
    this->blackboard = &bb;
    player = bb.entityFactory.CreatePlayer(8, 8, EntityDirection::Up, bb);
    AddEntity(2, 2, EntityDirection::Down, EntityType::AdvancedEnemy);
    AddEntity(9, 10, EntityDirection::Down, EntityType::AdvancedEnemy);
    shadowGrid.resize(bb.rows * bb.columns);
    UpdateShadowGrid();
    //pathfindingGrid = Grid::GenerateGrid(&bb.level);
}

void EntityManager::AddEntity(int x, int y, EntityDirection direction, EntityType type) {
    if (!blackboard) return;
    if (type == EntityType::Claymore || type == EntityType::WallCharge) {
        blackboard->entityFactory.CreateAndAddStaticEntity(x, y, direction, type);
    } else {
        blackboard->entityFactory.CreateAndAddDynamicEnemy(x, y, direction, type);
    }
}

void EntityManager::UpdateAI(Blackboard& blackboard) {
    aiSystem.Update(aiComponents, positionComponents, movementComponents, typeComponents, staticComponents, blackboardComponents, blackboard);
}

void EntityManager::UpdateMovement() {
    movementSystem.Update(positionComponents, movementComponents, aiComponents);
}

void EntityManager::UpdateRender(SDL_Renderer* renderer, int cellWidth, int cellHeight, int widthMargin, int heightMargin, int squareSize) {
    renderSystem.Update(positionComponents, movementComponents, renderComponents, renderer, cellWidth, cellHeight, widthMargin, heightMargin, squareSize);
}