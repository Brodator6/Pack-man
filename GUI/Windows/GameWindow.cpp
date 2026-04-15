#include "GameWindow.h"

//#include "../../UpdateState.cpp"
#include "../../UpdateInput.cpp"
#include "../../Entities/AI/BehaviorTree.h"
#include "../../LevelLoader.h"
#include "../../TileSystem.h"


#include <stdlib.h>  // For malloc, calloc, realloc, free
#include <stdio.h> 


void GameWindow::DrawWindow(){
    SDL_FRect rect;
    rect = {0, 0, (float)cellWidth, (float)cellHight};

    for (int i = 0; i < rows; i++){// loop for displaying current level
        for(int j = 0; j < columns; j++){
            DrawTile((*this->renderer), level[i][j], &rect, j, i, cellWidth, cellHight, widthMargine, hightMargine, squareSize);
        }
    }
    

    player->DrawEntity((*this->renderer), cellWidth, cellHight, widthMargine, hightMargine, squareSize);

    testEnemy->DrawEntity((*this->renderer), cellWidth, cellHight, widthMargine, hightMargine, squareSize);
};

SDL_AppResult GameWindow::HandleEvents(SDL_Event *Event){
    switch (Event->type){
    case SDL_EVENT_KEY_DOWN:
        switch (Event->key.scancode){
            case SDL_SCANCODE_ESCAPE:
                std::cout << "THE WORLD" << std::endl;
                isPaused = !isPaused;
        }

    default:
        break;
    }

    return SDL_APP_CONTINUE;
};

void GameWindow::GenerateLevel(){
    level = LevelLoader::LoadLevel("./Assets/Levels/level1.txt", rows, columns);
}

void GameWindow::HandleGameLogic(){
    previousTimeFrame = currentTimeFrame;
    currentTimeFrame = std::chrono::high_resolution_clock::now();
    if(!isPaused){

        UpdateInput(player->controls);
        
        accumulator += currentTimeFrame - previousTimeFrame;    
        while(accumulator >= deltaTime){
            UpdateState();
            player->Move();
            testEnemy->AI->Tick(testEnemy);
            accumulator -= deltaTime;
        }
    }
}

void GameWindow::UpdateState(){
    //this IS hell
    player->UpdateMovement(level, rows, columns);

    player->UseAbility();

    player->UpdateAbilitiesCooldown(deltaTime.count());

    ApplyEffect(player, level[player->GetPositionY()][player->GetPositionX()]);
}

std::function<void()> GameWindow::ToMainMenu(){
    return [this](){
        menus->RequestRootSwap(std::make_unique<MainMenuWindow>(menus, window, renderer));
    };
}

GameWindow::GameWindow(MenuManager *menus, SDL_Window **window, SDL_Renderer **renderer) : GUI(menus, window, renderer){
    GenerateLevel();
    mainMenuButton = new Button(300, 200, 200, 50, this->ToMainMenu(), "To The Menu" , font, &textColor, (*this->renderer));
}

GameWindow::~GameWindow(){

}