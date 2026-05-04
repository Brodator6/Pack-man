#include "GameMenu.h"

#include "../../Entities/Player.h"
#include "../../Entities/Actor.h"
#include "../../TileSystem.h"
#include "PauseMenu.h"


void GameMenu::DrawWindow(){
    SDL_FRect rect;
    rect = {0, 0, (float)cellWidth, (float)cellHight};

    for (int i = 0; i < gameManager.blackboard.rows; i++){// loop for displaying current level
        for(int j = 0; j < gameManager.blackboard.columns; j++){
            DrawTile((*menuBlackboard.renderer), gameManager.level[i][j], &rect, j, i, cellWidth, cellHight, widthMargine, hightMargine, squareSize);
        }
    }

    gameManager.entityManager.GetPlayer().DrawEntity((*menuBlackboard.renderer), cellWidth, cellHight, widthMargine, hightMargine, squareSize);

    for(auto &actor : gameManager.entityManager.enemies){
        actor.DrawEntity((*menuBlackboard.renderer), cellWidth, cellHight, widthMargine, hightMargine, squareSize);
    }
    
    scoreText.DrawElement(*menuBlackboard.renderer);
    abilityIcon1.DrawElement(*menuBlackboard.renderer);
};

SDL_AppResult GameMenu::HandleEvents(SDL_Event *Event){
    switch (Event->type){
    case SDL_EVENT_KEY_DOWN:
        switch (Event->key.scancode){
            case SDL_SCANCODE_ESCAPE: {
                menus->PushMenu(std::make_unique<PauseMenu>(menus, menuBlackboard, timeBlackboard));
            }
            default: 
                break;
        }
    default:
        break;
    }

    return SDL_APP_CONTINUE;
};

void GameMenu::HandleGameLogic(){
    timeBlackboard.previousTickTime = timeBlackboard.currentTime;
    timeBlackboard.currentTime = std::chrono::high_resolution_clock::now();
    
    accumulator += timeBlackboard.currentTime - timeBlackboard.previousTickTime;    
    while(accumulator >= timeBlackboard.deltaTime){
        scoreText.UpdateTextTexture(("Score: " + std::to_string(gameManager.entityManager.GetPlayer().score)), font, textColor);
        if(gameManager.lost){
            menus->PushMenu(std::make_unique<MainMenu>(menus, menuBlackboard, timeBlackboard));
        }
        gameManager.HandleGameLogic();
        accumulator -= timeBlackboard.deltaTime;
    }

    if (accumulator > std::chrono::milliseconds(250)) {
        accumulator = std::chrono::milliseconds(250); // "відрізаємо" занадто велику затримку
    }
}

std::function<void()> GameMenu::ToMainMenu(){
    return [this](){
        menus->RequestRootSwap(std::make_unique<MainMenu>(menus, menuBlackboard, timeBlackboard));
    };
}

GameMenu::GameMenu(MenuManager *menus, MenuBlackboard &mBB, TimeBlackboard &tBB) : GUI(menus, mBB, tBB), gameManager{*mBB.renderer, tBB}{

    abilityIcon1.SetTargetAbility(&gameManager.entityManager.GetPlayer().abilities[1], *menuBlackboard.renderer);
}

GameMenu::~GameMenu(){

}