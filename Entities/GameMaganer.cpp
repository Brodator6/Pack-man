#include "GameManager.h"

#include "../UpdateInput.cpp"
#include "AI/BehaviorTree.h"
#include "../LevelLoader.h"
#include "../TileSystem.h"

void GameManager::GenerateLevel(){
    level = LevelLoader::LoadLevel("./Assets/Levels/level1.txt", blackboard.rows, blackboard.columns);
}

void GameManager::HandleGameLogic(){
    UpdateInput(entityManager.GetPlayer().controls);
    UpdateState();
    entityManager.GetPlayer().Move();

    entityManager.UpdateAI(blackboard);
    entityManager.UpdateMovement();
    
    entityManager.UpdateState();
    entityManager.UpdateShadowGrid();
};

void GameManager::UpdateState(){
    //this IS hell
    entityManager.GetPlayer().UpdateMovement(level, blackboard.rows, blackboard.columns);

    entityManager.GetPlayer().UseAbility();

    entityManager.GetPlayer().UpdateAbilitiesCooldown(timeBlackboard.deltaTime.count());

    int playerGridIndex = entityManager.GetPlayer().GetPositionY() * blackboard.columns + entityManager.GetPlayer().GetPositionX();
    for(auto id : entityManager.shadowGrid[playerGridIndex].entityIDs){
        if(id > entityManager.GetPlayer().ID && entityManager.GetEntityById(id)->actorType == ActorType::DinamicActor){
            lost = true;
        }
    }

    ApplyEffect(&entityManager.GetPlayer(), level[entityManager.GetPlayer().GetPositionY()][entityManager.GetPlayer().GetPositionX()]);
}

GameManager::GameManager(SDL_Renderer *renderer, TimeBlackboard &tBB) : timeBlackboard{tBB}, abilityFactory(renderer){
    GenerateLevel();
    entityFactory = EntityFactory(renderer);
    entityManager.SetUp(blackboard);
};