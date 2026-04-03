#include "GameWindow.h"
#include "../../UpdateState.cpp"
#include "../../UpdateInput.cpp"

#include <stdlib.h>  // For malloc, calloc, realloc, free
#include <stdio.h> 


void GameWindow::DrawWindow(){
    //mainMenuButton->DrawElement(*renderer);
};

void GameWindow::HandleEvents(SDL_Event *Event){
    //mainMenuButton->HandleKey(*Event);
};

void GameWindow::GenerateLevel(){
    level.resize(rows);

    // Fill each with default Tile
    for(int i = 0; i < rows; i++){
        level[i].resize(columns);
        for(int j = 0; j < rows; j++){
            level[i][j] = std::make_unique<Floor>();
        }
    }
    level[2][2] = std::make_unique<Point>();
    level[8][8] = std::make_unique<Wall>();
}

void GameWindow::HandleGameLogic(){

    previousTimeFrame = currentTimeFrame;
    currentTimeFrame = std::chrono::high_resolution_clock::now();

    UpdateInput(player->movement);
    
    accumulator += currentTimeFrame - previousTimeFrame;
    //std::cout << "accumulator: " << accumulator.count() << " || deltaTime: " << deltaTime.count() << std::endl;

    
    while(accumulator >= deltaTime ){
        UpdateState(player, level, rows, columns);
        accumulator -= deltaTime;
    }
    
    SDL_FRect rect;
    rect = {0, 0, (float)cellWidth, (float)cellHight};

    for (int i = 0; i < rows; i++){// loop for displaying current level
        for(int j = 0; j < columns; j++){
            level[i][j]->DrawTile((*this->renderer), &rect, j, i, cellWidth, cellHight, widthMargine, hightMargine, squareSize);
        }
    }
    

    player->DrawEntity((*this->renderer), cellWidth, cellHight, widthMargine, hightMargine, squareSize);
    

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