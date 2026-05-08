#include "GameMenu.h"

#include "../../Entities/Player.h"
#include "../../Entities/Actor.h"
#include "../../TileSystem.h"
#include "PauseMenu.h"
#include "AbilityMenu.h"


void GameMenu::DrawWindow(){
    SDL_FRect rect;
    rect = {0, 0, (float)cellWidth, (float)cellHight};

    // Draw tiles
    for (int i = 0; i < gameManager.blackboard.rows; i++){// loop for displaying current level
        for(int j = 0; j < gameManager.blackboard.columns; j++){
            DrawTile((*menuBlackboard.renderer), gameManager.level[i][j], &rect, j, i, cellWidth, cellHight, widthMargine, hightMargine, squareSize);
        }
    }

    // Draw player
    gameManager.entityManager.GetPlayer().DrawEntity((*menuBlackboard.renderer), cellWidth, cellHight, widthMargine, hightMargine, squareSize);

    // Draw all other entities using ECS render system
    gameManager.entityManager.UpdateRender(*menuBlackboard.renderer, cellWidth, cellHight, widthMargine, hightMargine, squareSize);
    
    // Draw UI
    scoreText.DrawElement(*menuBlackboard.renderer);
    abilityIcon1.DrawElement(*menuBlackboard.renderer);
    abilityIcon2.DrawElement(*menuBlackboard.renderer);
    abilityIcon3.DrawElement(*menuBlackboard.renderer);
    abilityIcon4.DrawElement(*menuBlackboard.renderer);
};

SDL_AppResult GameMenu::HandleEvents(SDL_Event *Event){
    switch (Event->type){
    case SDL_EVENT_KEY_DOWN:
        switch (Event->key.scancode){
            case SDL_SCANCODE_ESCAPE: {
                menus->PushMenu(std::make_unique<PauseMenu>(menus, menuBlackboard, timeBlackboard));
                break;
            }
            case SDL_SCANCODE_F2: {
                AbilityMenuBlackboard abb = {
                    gameManager.entityManager.GetPlayer().abilities,
                    &gameManager.blackboard.abilityFactory,
                    *menuBlackboard.renderer
                };
                menus->PushMenu(std::make_unique<AbilityMenu>(menus, menuBlackboard, timeBlackboard, abb));
                break;
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

    abilityIcon1.SetTargetAbility(&gameManager.entityManager.GetPlayer().abilities[0], *menuBlackboard.renderer);
    abilityIcon2.SetTargetAbility(&gameManager.entityManager.GetPlayer().abilities[1], *menuBlackboard.renderer);
    abilityIcon3.SetTargetAbility(&gameManager.entityManager.GetPlayer().abilities[2], *menuBlackboard.renderer);
    abilityIcon4.SetTargetAbility(&gameManager.entityManager.GetPlayer().abilities[3], *menuBlackboard.renderer);
}

GameMenu::~GameMenu(){

}