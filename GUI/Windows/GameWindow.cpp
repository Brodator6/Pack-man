#include "GameWindow.h"

#include "../../Entities/EntityFactory.h"
#include "../../UpdateInput.cpp"
#include "../../Entities/AI/BehaviorTree.h"
#include "../../LevelLoader.h"
#include "../../TileSystem.h"


void GameWindow::DrawWindow(){
    SDL_FRect rect;
    rect = {0, 0, (float)cellWidth, (float)cellHight};

    for (int i = 0; i < bb.rows; i++){// loop for displaying current level
        for(int j = 0; j < bb.columns; j++){
            DrawTile((*this->renderer), level[i][j], &rect, j, i, cellWidth, cellHight, widthMargine, hightMargine, squareSize);
        }
    }
    

    entityManager.GetPlayer().DrawEntity((*this->renderer), cellWidth, cellHight, widthMargine, hightMargine, squareSize);

    for(auto &enemy : entityManager.enemies){
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
    level = LevelLoader::LoadLevel("./Assets/Levels/level1.txt", bb.rows, bb.columns);
}

void GameWindow::HandleGameLogic(){
    previousTimeFrame = currentTimeFrame;
    currentTimeFrame = std::chrono::high_resolution_clock::now();
    if(!isPaused){

        UpdateInput(entityManager.GetPlayer().controls);
        
        accumulator += currentTimeFrame - previousTimeFrame;    
        while(accumulator >= deltaTime){
            UpdateState();
            entityManager.GetPlayer().Move();

            for(auto &enemy : entityManager.enemies){
                enemy.AI->Tick(enemy, bb);
                enemy.UpdateMovement();
                enemy.Move();
            }
            entityManager.UpdateShadowGrid();
            accumulator -= deltaTime;
        }
    }
}

void GameWindow::UpdateState(){
    //this IS hell
    entityManager.GetPlayer().UpdateMovement(level, bb.rows, bb.columns);

    entityManager.GetPlayer().UseAbility();

    entityManager.GetPlayer().UpdateAbilitiesCooldown(deltaTime.count());

    int playerGridIndex = entityManager.GetPlayer().GetPositionY() * bb.columns + entityManager.GetPlayer().GetPositionX();
    for(auto id : entityManager.shadowGrid[playerGridIndex].entityIDs){
        if(id > 0){
            menus->RequestRootSwap(std::make_unique<MainMenuWindow>(menus, window, renderer));
        }
    }

    ApplyEffect(&entityManager.GetPlayer(), level[entityManager.GetPlayer().GetPositionY()][entityManager.GetPlayer().GetPositionX()]);
}

std::function<void()> GameWindow::ToMainMenu(){
    return [this](){
        menus->RequestRootSwap(std::make_unique<MainMenuWindow>(menus, window, renderer));
    };
}

Entity* GameWindow::GetEntityById(int entityID){
    if(entityID == kPlayerEntityID){
        return &entityManager.GetPlayer();
    }

    int enemyIndex = entityID - kEnemyEntityIdOffset;
    if(enemyIndex >= 0 && enemyIndex < static_cast<int>(entityManager.enemies.size())){
        return &entityManager.enemies[enemyIndex];
    }

    return nullptr;
}

const GridCell* GameWindow::GetGridCell(int x, int y) const{
    if(x < 0 || x >= bb.columns || y < 0 || y >= bb.rows){
        return nullptr;
    }
    return &entityManager.shadowGrid[y * bb.columns + x];
}

int GameWindow::GetShadowGridWidth() const{
    return bb.columns;
}

int GameWindow::GetShadowGridHeight() const{
    return bb.rows;
}

GameWindow::GameWindow(MenuManager *menus, SDL_Window **window, SDL_Renderer **renderer) : GUI(menus, window, renderer){
    GenerateLevel();
    entityManager.SetUp(bb);
    std::cout << "done1" << std::endl;
}

GameWindow::~GameWindow(){

}