#include "EntityManager.h"
#include "EntityFactory.h"

void EntityManager::UpdateShadowGrid(){

    if(static_cast<int>(shadowGrid.size()) != bb->rows * bb->columns){
        shadowGrid.assign(bb->rows * bb->columns, GridCell());
    }

    for(auto &cell : shadowGrid){
        cell.Clear();
    }

    int playerIndex = player.GetPositionY() * bb->columns + player.GetPositionX();
    if(playerIndex >= 0 && playerIndex < static_cast<int>(shadowGrid.size())){
        shadowGrid[playerIndex].AddEntity(kPlayerEntityID);
    }

    for(int i = 0; i < static_cast<int>(enemies.size()); ++i){
        const Enemy &enemy = enemies[i];
        int enemyX = enemy.GetPositionX();
        int enemyY = enemy.GetPositionY();
        if(enemyX < 0 || enemyX >= bb->columns || enemyY < 0 || enemyY >= bb->rows){
            continue;
        }
        int enemyIndex = enemyY * bb->columns + enemyX;
        shadowGrid[enemyIndex].AddEntity(kEnemyEntityIdOffset + i);
    }

}

Entity* EntityManager::GetEntityById(int entityID){

};

const GridCell* EntityManager::GetGridCell(int x, int y) const{

};

void EntityManager::AddEnemy(){

};

void EntityManager::SetPlayer(){

};

EntityManager::EntityManager(){
    std::cout << "done2" << std::endl;
};

void EntityManager::SetUp(Blackboard &bb){
    this->bb = &bb;
    player = bb.entityFactory.CreatePlayer(8, 8, Direction::Up, bb);
    enemies.push_back(bb.entityFactory.CreateEntity(9, 10, Direction::Down, EntityType::BasicEnemy));
    shadowGrid.resize(bb.rows * bb.columns);
    UpdateShadowGrid();
}