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

    for(auto& [entityID, pos] : positionComponents){
        if(entityID >= kEnemyEntityIdOffset){
            int enemyX = pos.x;
            int enemyY = pos.y;
            if(enemyX < 0 || enemyX >= bb->columns || enemyY < 0 || enemyY >= bb->rows){
                continue;
            }
            int enemyIndex = enemyY * bb->columns + enemyX;
            shadowGrid[enemyIndex].AddEntity(entityID);
        }
    }
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
    if(x < 0 || x >= bb->columns || y < 0 || y >= bb->rows){
        return nullptr;
    }
    return &shadowGrid[y * bb->columns + x];
}

void EntityManager::RequestAddEnemy(Actor newEnemy){
    // Generate unique entity ID
    // Player = 0, Enemies start at kEnemyEntityIdOffset (which is 1)
    int entityID = kEnemyEntityIdOffset + static_cast<int>(positionComponents.size());
    
    // ========== STEP 1: Extract data from Actor object into Components ==========
    // The Actor object is built by EntityFactory, but we decompose it into
    // separate component data that lives in unordered_maps. This is the
    // transition from traditional OOP (Actor class) to ECS (component maps).
    
    // Position & Rendering - All entities need these
    positionComponents[entityID] = {
        newEnemy.xPosition, 
        newEnemy.yPosition, 
        newEnemy.visualX, 
        newEnemy.visualY
    };
    
    renderComponents[entityID] = {
        newEnemy.texture, 
        newEnemy.rect
    };
    
    // Type metadata - All entities need this
    typeComponents[entityID] = {
        newEnemy.actorType, 
        newEnemy.enemyType
    };
    
    // ========== STEP 2: Entity-type specific components ==========
    // Dynamic actors (enemies that move and think)
    if (newEnemy.actorType == ActorType::DinamicActor) {
        // Movement data - for entities that move
        movementComponents[entityID] = {
            newEnemy.targetX,                                    // Current target tile
            newEnemy.targetY,
            newEnemy.direction,                                  // Facing direction
            newEnemy.DinamicEntity.speedModifier,               // Movement speed
            newEnemy.DinamicEntity.goalX,                       // Final destination
            newEnemy.DinamicEntity.goalY,
            newEnemy.DinamicEntity.LastSeenPlayerX,            // Remember player location
            newEnemy.DinamicEntity.LastSeenPlayerY,
            newEnemy.DinamicEntity.isChasing                    // Hunting state
        };
        
        // AI system - Behavior tree for decision making
        aiComponents[entityID] = {
            newEnemy.currentPath,                               // Pathfinding result
            std::move(newEnemy.AI),                             // Behavior tree
            newEnemy.enemyType
        };
    } 
    // Static actors (traps, obstacles)
    else if (newEnemy.actorType == ActorType::StaticActor) {
        // Static entities don't have movement but have timers/effects
        staticComponents[entityID] = {
            newEnemy.StaticEntity.timer,
            newEnemy.StaticEntity.cooldown
        };
        
        // Even static entities have AI (for triggering logic)
        aiComponents[entityID] = {
            newEnemy.currentPath,
            std::move(newEnemy.AI),
            newEnemy.enemyType
        };
    }
    
    // ========== STEP 3: Blackboard setup for multi-agent communication ==========
    // Different enemy types have access to different shared data
    if (newEnemy.enemyType == EnemyType::AdvancedEnemy) {
        // Advanced enemies can share target information with each other
        blackboardComponents[entityID] = {&advancedEnemyBlackboard, nullptr};
        advancedEnemyBlackboard.GetOrCreateEnemyIndex(entityID);
    } else if (newEnemy.enemyType == EnemyType::CommandoEnemy) {
        // Commando enemies have hierarchical commandt structure
        blackboardComponents[entityID] = {nullptr, &commandoEnemyBlackboard};
        commandoEnemyBlackboard.GetOrCreateEnemyIndex(entityID);
    }
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
    this->bb = &bb;
    player = bb.entityFactory.CreatePlayer(8, 8, Direction::Up, bb);
    RequestAddEnemy(bb.entityFactory.CreateEntity(2, 2, Direction::Down, EntityType::BasicEnemy));
    RequestAddEnemy(bb.entityFactory.CreateEntity(9, 10, Direction::Down, EntityType::BasicEnemy));
    shadowGrid.resize(bb.rows * bb.columns);
    UpdateShadowGrid();
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