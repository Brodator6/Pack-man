#include "GameWindow.h"
#include "../../UpdateState.cpp"
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

void GameWindow::HandleEvents(SDL_Event *Event){
    //mainMenuButton->HandleKey(*Event);
};

void GameWindow::GenerateLevel(){
    level = LevelLoader::LoadLevel("./Assets/Levels/level1.txt", rows, columns);
}

void GameWindow::HandleGameLogic(){

    previousTimeFrame = currentTimeFrame;
    currentTimeFrame = std::chrono::high_resolution_clock::now();

    UpdateInput(player->movement);
    
    accumulator += currentTimeFrame - previousTimeFrame;    
    while(accumulator >= deltaTime ){
        UpdateState(player, level, rows, columns);
        testEnemy->AI->Tick(testEnemy);
        accumulator -= deltaTime;
    }
}

std::function<void()> GameWindow::ToMainMenu(){
    return [this](){
        *currentScreen = new MainMenuWindow(currentScreen, *currentScreen, window, renderer);
        delete oldScreen;
        
    };
}

GameWindow::GameWindow(GUI **currentScreen, GUI *oldScreen, SDL_Window **window, SDL_Renderer **renderer) : GUI(currentScreen, oldScreen, window, renderer){
    GenerateLevel();
    mainMenuButton = new Button(300, 200, 200, 50, this->ToMainMenu(), "To The Menu" , font, &textColor, (*this->renderer));
}

GameWindow::~GameWindow(){

}