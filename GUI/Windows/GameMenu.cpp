#include "GameMenu.h"

#include "../../Entities/EntityFactory.h"
#include "../../UpdateInput.cpp"
#include "../../Entities/AI/BehaviorTree.h"
#include "../../LevelLoader.h"
#include "../../TileSystem.h"
#include "PauseMenu.h"


void GameMenu::DrawWindow(){
    SDL_FRect rect;
    rect = {0, 0, (float)cellWidth, (float)cellHight};

    for (int i = 0; i < bb.rows; i++){// loop for displaying current level
        for(int j = 0; j < bb.columns; j++){
            DrawTile((*this->renderer), level[i][j], &rect, j, i, cellWidth, cellHight, widthMargine, hightMargine, squareSize);
        }
    }
    

    entityManager.GetPlayer().DrawEntity((*this->renderer), cellWidth, cellHight, widthMargine, hightMargine, squareSize);

    for(auto &actor : entityManager.enemies){
        actor.DrawEntity((*this->renderer), cellWidth, cellHight, widthMargine, hightMargine, squareSize);
    }
};

SDL_AppResult GameMenu::HandleEvents(SDL_Event *Event){
    switch (Event->type){
    case SDL_EVENT_KEY_DOWN:
        switch (Event->key.scancode){
            case SDL_SCANCODE_ESCAPE: {
                PauseMenuDataTransportationStruct transportationBB = {previousTimeFrame, currentTimeFrame};
                menus->PushMenu(std::make_unique<PauseMenu>(menus, window, renderer, transportationBB));
            }
            default: 
                break;
        }
    default:
        break;
    }

    return SDL_APP_CONTINUE;
};

void GameMenu::GenerateLevel(){
    level = LevelLoader::LoadLevel("./Assets/Levels/level1.txt", bb.rows, bb.columns);
}

void GameMenu::HandleGameLogic(){
    previousTimeFrame = currentTimeFrame;
    currentTimeFrame = std::chrono::high_resolution_clock::now();

    UpdateInput(entityManager.GetPlayer().controls);
    
    accumulator += currentTimeFrame - previousTimeFrame;    
    while(accumulator >= deltaTime){
        UpdateState();
        entityManager.GetPlayer().Move();

        for(auto &actor : entityManager.enemies){
            actor.AI->Tick(actor, bb);
            actor.UpdateMovement();
            actor.Move();
        }
        
        entityManager.UpdateState();
        entityManager.UpdateShadowGrid();
        accumulator -= deltaTime;
    }
}

void GameMenu::UpdateState(){
    //this IS hell
    entityManager.GetPlayer().UpdateMovement(level, bb.rows, bb.columns);

    entityManager.GetPlayer().UseAbility();

    entityManager.GetPlayer().UpdateAbilitiesCooldown(deltaTime.count());

    int playerGridIndex = entityManager.GetPlayer().GetPositionY() * bb.columns + entityManager.GetPlayer().GetPositionX();
    for(auto id : entityManager.shadowGrid[playerGridIndex].entityIDs){
        if(id > entityManager.GetPlayer().ID && entityManager.GetEntityById(id)->actorType == ActorType::DinamicActor){
            menus->RequestRootSwap(std::make_unique<MainMenu>(menus, window, renderer));
        }
    }

    ApplyEffect(&entityManager.GetPlayer(), level[entityManager.GetPlayer().GetPositionY()][entityManager.GetPlayer().GetPositionX()]);
}

std::function<void()> GameMenu::ToMainMenu(){
    return [this](){
        menus->RequestRootSwap(std::make_unique<MainMenu>(menus, window, renderer));
    };
}

GameMenu::GameMenu(MenuManager *menus, SDL_Window **window, SDL_Renderer **renderer) : GUI(menus, window, renderer){
    GenerateLevel();
    entityManager.SetUp(bb);
    std::cout << "done1" << std::endl;
}

GameMenu::~GameMenu(){

}