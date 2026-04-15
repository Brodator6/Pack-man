#pragma once
#include "GUI.h"
#include "MainMenuWindow.h"
#include "../../Entities/Player.h"
#include "../../Entities/Enemy.h"
#include "../../TileData.h"

#include <chrono>
#include <memory>




// Forward declaration
class MainMenuWindow;
class Tile;


class GameWindow : public GUI{
private:
    //data to manage game loop
    std::chrono::duration<double> deltaTime = std::chrono::duration<float>(1.0/10.0);
    std::chrono::duration<double> t = std::chrono::duration<float>(0.0);
    std::chrono::duration<double> accumulator = std::chrono::duration<float>(0.0);
    std::chrono::time_point<std::chrono::high_resolution_clock> previousTimeFrame = std::chrono::high_resolution_clock::now(); // might want to replace auto with smth different
    std::chrono::time_point<std::chrono::high_resolution_clock> currentTimeFrame = std::chrono::high_resolution_clock::now();
    bool isPaused = false;

    int rows = 20;
    int columns = 20;
    int cellWidth = 40;
    int cellHight = 40;
    int widthMargine = 40;
    int hightMargine = 50;
    int squareSize = 40;

    std::vector<std::vector<TileData>> level;

    Player player = Player(10, 10);//player test

    std::vector<Enemy> enemies;

    static constexpr int kMaxEntitiesPerCell = 3;
    static constexpr int kPlayerEntityID = 0;
    static constexpr int kEnemyEntityIdOffset = 1;

    struct GridCell {
        int entityIDs[kMaxEntitiesPerCell];
        GridCell(){ Clear(); }
        void Clear(){ for(int i = 0; i < kMaxEntitiesPerCell; ++i) entityIDs[i] = -1; }
        bool AddEntity(int entityID){
            for(int i = 0; i < kMaxEntitiesPerCell; ++i){
                if(entityIDs[i] == -1){
                    entityIDs[i] = entityID;
                    return true;
                }
            }
            return false;
        }
    };

    std::vector<GridCell> shadowGrid;

    Button *mainMenuButton;
public:

    void DrawWindow();
    void UpdateShadowGrid();
    Entity* GetEntityById(int entityID);
    const GridCell* GetGridCell(int x, int y) const;
    int GetShadowGridWidth() const;
    int GetShadowGridHeight() const;

    SDL_AppResult HandleEvents(SDL_Event *Event);

    void HandleGameLogic() override;

    void GenerateLevel();

    void UpdateState();

    std::function<void()> ToMainMenu();

    GameWindow(MenuManager *menus, SDL_Window **window, SDL_Renderer **renderer);
    ~GameWindow();
};