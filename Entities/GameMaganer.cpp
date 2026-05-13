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
    //entityManager.UpdatePathfindingGrid();
};

void GameManager::UpdateState(){
    //this IS hell
    entityManager.GetPlayer().UpdateMovement(level, blackboard.rows, blackboard.columns);

    entityManager.GetPlayer().UseAbility();

    entityManager.GetPlayer().UpdateAbilitiesCooldown(timeBlackboard.deltaTime.count());


    ApplyEffect(&entityManager.GetPlayer(), level[entityManager.GetPlayer().GetPositionY()][entityManager.GetPlayer().GetPositionX()]);

    int playerGridIndex = entityManager.GetPlayer().GetPositionY() * blackboard.rows + entityManager.GetPlayer().GetPositionX();
    for(auto id : entityManager.shadowGrid[playerGridIndex].entityIDs){
        if(id > entityManager.GetPlayer().ID){
            std::cout << "lost" << std::endl;
            lost = true;
        }
    }
}

GameManager::GameManager(SDL_Renderer *renderer, TimeBlackboard &tBB) : timeBlackboard{tBB}, abilityFactory(renderer){
    GenerateLevel();
    entityFactory = EntityFactory(renderer, &blackboard);
    entityManager.SetUp(blackboard);
};