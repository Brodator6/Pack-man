#pragma once
#include "GUI.h"
#include "MainMenu.h"
#include "../../Entities/Player.h"
#include "../../Entities/Actor.h"
#include "../../TileData.h"
#include "../../DataStructs.h"
#include "../../Entities/EntityManager.h"
#include "../../Entities/EntityFactory.h"

#include <chrono>
#include <memory>




// Forward declaration
class MainMenuWindow;
class Tile;


class GameMenu : public GUI{
private:
    //data to manage game loop
    std::chrono::duration<double> deltaTime = std::chrono::duration<float>(1.0/10.0);
    std::chrono::duration<double> t = std::chrono::duration<float>(0.0);
    std::chrono::duration<double> accumulator = std::chrono::duration<float>(0.0);
    std::chrono::time_point<std::chrono::high_resolution_clock> previousTimeFrame = std::chrono::high_resolution_clock::now(); // might want to replace auto with smth different
    std::chrono::time_point<std::chrono::high_resolution_clock> currentTimeFrame = std::chrono::high_resolution_clock::now();
    bool isPaused = false;

    int cellWidth = 40;
    int cellHight = 40;
    int widthMargine = 40;
    int hightMargine = 50;
    int squareSize = 40;

    std::vector<std::vector<TileData>> level;

    EntityFactory entityFactory = EntityFactory(*renderer);
    EntityManager entityManager;

    Blackboard bb = {
        20, 20,
        level,
        entityManager,
        entityFactory
    };

public:

    void DrawWindow() override;

    SDL_AppResult HandleEvents(SDL_Event *Event) override;

    void HandleGameLogic() override;

    void GenerateLevel();

    void UpdateState();

    std::function<void()> ToMainMenu();

    GameMenu(MenuManager *menus, SDL_Window **window, SDL_Renderer **renderer);
    ~GameMenu();
};