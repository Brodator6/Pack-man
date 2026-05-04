#include "EntityManager.h"
#include "EntityFactory.h"
#include "AI/BehaviorTree.h"

void EntityManager::UpdateShadowGrid(){

    if(static_cast<int>(shadowGrid.size()) != bb->rows * bb->columns){
        shadowGrid.assign(bb->rows * bb->columns, GridCell());
    }

    for(auto &cell : shadowGrid){
        cell.Clear();
    }

    int playerIndex = player.GetPositionY() * bb->columns + player.GetPositionX();
    if(player.ID >= 0 && playerIndex >= 0 && playerIndex < static_cast<int>(shadowGrid.size())){
        shadowGrid[playerIndex].AddEntity(player.ID);//needs modifying
    }

    for(int i = 0; i < static_cast<int>(enemies.size()); ++i){
        const Actor &enemy = enemies[i];
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
    if(entityID == player.ID){
        return &player;
    }

    int entityIndex = entityID - kEnemyEntityIdOffset;
    if(entityIndex >= 0 && entityIndex < static_cast<int>(enemies.size())){
        return &enemies[entityIndex];
    }

    return nullptr;
}

const GridCell* EntityManager::GetGridCell(int x, int y) const{
    if(x < 0 || x >= bb->columns || y < 0 || y >= bb->rows){
        return nullptr;
    }
    return &shadowGrid[y * bb->columns + x];
}

void EntityManager::RequestAddEnemy(Actor newEnemy){
    newEnemy.entityID = kEnemyEntityIdOffset + static_cast<int>(enemies.size());
    
    // Set up blackboard pointers based on enemy type
    if (newEnemy.enemyType == EnemyType::AdvancedEnemy) {
        newEnemy.advancedEnemyBB = &advancedEnemyBlackboard;
        advancedEnemyBlackboard.GetOrCreateEnemyIndex(newEnemy.entityID);
    } else if (newEnemy.enemyType == EnemyType::CommandoEnemy) {
        newEnemy.advancedEnemyBB = &commandoEnemyBlackboard;
        commandoEnemyBlackboard.GetOrCreateEnemyIndex(newEnemy.entityID);
    }
    
    enemies.push_back(std::move(newEnemy));
};

void EntityManager::RequestRemoveEntityByID(int entityID){
    removalQueue.push_back(entityID);
}

void EntityManager::SetPlayer(Player newPlayer){
    player = newPlayer;
};

void EntityManager::DrawEntity(SDL_Renderer *renderer, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize, Actor &actor){
    actor.rect.x = hightMargine + actor.visualX * cellHight + 5;
    actor.rect.y = widthMargine + actor.visualY * cellWidth + 5;
    actor.rect.w = squareSize - 10;
    actor.rect.h = squareSize - 10;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &actor.rect);
    SDL_RenderTextureRotated(renderer, actor.texture.get(), NULL, &actor.rect, (0.0 + ((actor.direction == Direction::Down) * 90.0) + ((actor.direction == Direction::Left) * 180.0) + ((actor.direction == Direction::Up) * 270.0)), NULL, SDL_FLIP_NONE);
}

void EntityManager::UpdateState(){
    if(!removalQueue.empty()){
        for(int i = 0; i < removalQueue.size() - 1; i++){
            if(removalQueue[i] == player.ID){
                //show score window with "try again"
                std::cout << "death" << std::endl;
                return;
            }
            int entityIndex = removalQueue[i] - kEnemyEntityIdOffset;
            if(entityIndex >= 0 && entityIndex < static_cast<int>(enemies.size())){
                int lastIndex = static_cast<int>(enemies.size() - 1);
                if(entityIndex != lastIndex){
                    enemies[entityIndex] = std::move(enemies.back());
                    enemies[entityIndex].entityID = kEnemyEntityIdOffset + entityIndex;
                }
                enemies.pop_back();
            }
        }
    }
}

EntityManager::EntityManager(){
};

EntityManager::~EntityManager(){
};

void EntityManager::SetUp(Blackboard &bb){
    this->bb = &bb;
    player = bb.entityFactory.CreatePlayer(8, 8, Direction::Up, bb);
    RequestAddEnemy(bb.entityFactory.CreateEntity(9, 10, Direction::Down, EntityType::BasicEnemy));
    shadowGrid.resize(bb.rows * bb.columns);
    UpdateShadowGrid();
}