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
    

    player.DrawEntity((*this->renderer), cellWidth, cellHight, widthMargine, hightMargine, squareSize);

    for(auto &enemy : enemies){
        enemy.DrawEntity((*this->renderer), cellWidth, cellHight, widthMargine, hightMargine, squareSize);
    }
};

SDL_AppResult GameWindow::HandleEvents(SDL_Event *Event){
    switch (Event->type){
    case SDL_EVENT_KEY_DOWN:
        switch (Event->key.scancode){
            case SDL_SCANCODE_ESCAPE:
                std::cout << "THE WORLD" << std::endl;
                isPaused = !isPaused;
            default: 
                break;
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

        UpdateInput(player.controls);
        
        accumulator += currentTimeFrame - previousTimeFrame;    
        while(accumulator >= deltaTime){
            UpdateState();
            player.Move();
            for(auto &enemy : enemies){
                Blackboard bb = {enemy, level, shadowGrid};
                enemy.AI->Tick(bb);
            }
            UpdateShadowGrid();
            accumulator -= deltaTime;
        }
    }
}

void GameWindow::UpdateState(){
    //this IS hell
    player.UpdateMovement(level, rows, columns);

    player.UseAbility();

    player.UpdateAbilitiesCooldown(deltaTime.count());

    for(auto id : shadowGrid[player.GetPositionY() * rows + player.GetPositionX()].entityIDs){
        if(id > 0){
            menus->RequestRootSwap(std::make_unique<MainMenuWindow>(menus, window, renderer));
        }
    }

    ApplyEffect(&player, level[player.GetPositionY()][player.GetPositionX()]);
}

std::function<void()> GameWindow::ToMainMenu(){
    return [this](){
        menus->RequestRootSwap(std::make_unique<MainMenuWindow>(menus, window, renderer));
    };
}

GameWindow::GameWindow(MenuManager *menus, SDL_Window **window, SDL_Renderer **renderer) : GUI(menus, window, renderer){
    GenerateLevel();

    enemies.push_back(Enemy(10, 10, IMG_LoadTexture(*renderer, "./Assets/Sprites/testSprite.png")));
    shadowGrid.resize(rows * columns);
    UpdateShadowGrid();
}

void GameWindow::UpdateShadowGrid(){
    if(static_cast<int>(shadowGrid.size()) != rows * columns){
        shadowGrid.assign(rows * columns, GridCell());
    }

    for(auto &cell : shadowGrid){
        cell.Clear();
    }

    int playerIndex = player.GetPositionY() * columns + player.GetPositionX();
    if(playerIndex >= 0 && playerIndex < static_cast<int>(shadowGrid.size())){
        shadowGrid[playerIndex].AddEntity(kPlayerEntityID);
    }

    for(int i = 0; i < static_cast<int>(enemies.size()); ++i){
        const Enemy &enemy = enemies[i];
        int enemyX = enemy.GetPositionX();
        int enemyY = enemy.GetPositionY();
        if(enemyX < 0 || enemyX >= columns || enemyY < 0 || enemyY >= rows){
            continue;
        }
        int enemyIndex = enemyY * columns + enemyX;
        shadowGrid[enemyIndex].AddEntity(kEnemyEntityIdOffset + i);
    }
}

Entity* GameWindow::GetEntityById(int entityID){
    if(entityID == kPlayerEntityID){
        return &player;
    }

    int enemyIndex = entityID - kEnemyEntityIdOffset;
    if(enemyIndex >= 0 && enemyIndex < static_cast<int>(enemies.size())){
        return &enemies[enemyIndex];
    }

    return nullptr;
}

const GridCell* GameWindow::GetGridCell(int x, int y) const{
    if(x < 0 || x >= columns || y < 0 || y >= rows){
        return nullptr;
    }
    return &shadowGrid[y * columns + x];
}

int GameWindow::GetShadowGridWidth() const{
    return columns;
}

int GameWindow::GetShadowGridHeight() const{
    return rows;
}

GameWindow::~GameWindow(){

}