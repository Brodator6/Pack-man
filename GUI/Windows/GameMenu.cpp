#include "GameMenu.h"

#include "../../Entities/Player.h"
#include "../../TileSystem.h"
#include "PauseMenu.h" 
#include "AbilityMenu.h"
#include "ResultsMenu.h"


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
            case SDL_SCANCODE_F3: {
                ResultDataBlackboard transferData = {"You Have Won", 100, false};
                menus->PushMenu(std::make_unique<ResultsMenu>(menus, menuBlackboard, timeBlackboard, transferData, this->ToTheNextLevel()));
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

        gameManager.HandleGameLogic();
        abilityIcon1.UpdateChargesTexture(font, textColor);
        abilityIcon2.UpdateChargesTexture(font, textColor);
        abilityIcon3.UpdateChargesTexture(font, textColor);
        abilityIcon4.UpdateChargesTexture(font, textColor);
        
        if(gameManager.lost){
            ResultDataBlackboard transferData = {"You Have Lost", gameManager.globalScore, true};
            menus->PushMenu(std::make_unique<ResultsMenu>(menus, menuBlackboard, timeBlackboard, transferData, this->ToTheNextLevel()));
        }
        if(gameManager.globalScore >= gameManager.scoreGoal){
            ResultDataBlackboard transferData = {"You Have Won", gameManager.globalScore, false};
            menus->PushMenu(std::make_unique<ResultsMenu>(menus, menuBlackboard, timeBlackboard, transferData, this->ToTheNextLevel()));
        }
        
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

std::function<void()> GameMenu::ToTheNextLevel(){
    return [this](){
        menus->RequestRootSwap(std::make_unique<GameMenu>(menus, menuBlackboard, timeBlackboard, gameManager));
    };
}

GameMenu::GameMenu(MenuManager *menus, MenuBlackboard &mBB, TimeBlackboard &tBB) : GUI(menus, mBB, tBB), gameManager{*mBB.renderer, tBB}{
    timeBlackboard.currentTime = std::chrono::high_resolution_clock::now();
    timeBlackboard.previousTickTime = timeBlackboard.currentTime;
    abilityIcon1.SetTargetAbility(&gameManager.entityManager.GetPlayer().abilities[0], *menuBlackboard.renderer, font, textColor);
    abilityIcon2.SetTargetAbility(&gameManager.entityManager.GetPlayer().abilities[1], *menuBlackboard.renderer, font, textColor);
    abilityIcon3.SetTargetAbility(&gameManager.entityManager.GetPlayer().abilities[2], *menuBlackboard.renderer, font, textColor);
    abilityIcon4.SetTargetAbility(&gameManager.entityManager.GetPlayer().abilities[3], *menuBlackboard.renderer, font, textColor);
}

GameMenu::GameMenu(MenuManager *menus, MenuBlackboard &mBB, TimeBlackboard &tBB, const GameManager &previousManager) : GUI(menus, mBB, tBB), gameManager{*mBB.renderer, tBB, previousManager}{
    timeBlackboard.currentTime = std::chrono::high_resolution_clock::now();
    timeBlackboard.previousTickTime = timeBlackboard.currentTime;
    abilityIcon1.SetTargetAbility(&gameManager.entityManager.GetPlayer().abilities[0], *menuBlackboard.renderer, font, textColor);
    abilityIcon2.SetTargetAbility(&gameManager.entityManager.GetPlayer().abilities[1], *menuBlackboard.renderer, font, textColor);
    abilityIcon3.SetTargetAbility(&gameManager.entityManager.GetPlayer().abilities[2], *menuBlackboard.renderer, font, textColor);
    abilityIcon4.SetTargetAbility(&gameManager.entityManager.GetPlayer().abilities[3], *menuBlackboard.renderer, font, textColor);
}

GameMenu::~GameMenu(){

}