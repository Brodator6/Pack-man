#pragma once
#include "GUI.h"
#include "MainMenuWindow.h"

#include <chrono>
#include <memory>
#include "../../Entities/Player.h"
#include "../../Tile.h"



// Forward declaration
class MainMenuWindow;
class Tile;

class GameWindow : public GUI{
private:
    //data to manage game loop
    std::chrono::duration<double> deltaTime = std::chrono::duration<float>(1.0/20.0);
    std::chrono::duration<double> t = std::chrono::duration<float>(0.0);
    std::chrono::duration<double> accumulator = std::chrono::duration<float>(0.0);
    std::chrono::time_point<std::chrono::high_resolution_clock> previousTimeFrame = std::chrono::high_resolution_clock::now(); // might want to replace auto with smth different
    std::chrono::time_point<std::chrono::high_resolution_clock> currentTimeFrame = std::chrono::high_resolution_clock::now();

    int rows = 20;
    int columns = 20;
    int cellWidth = 40;
    int cellHight = 40;
    int widthMargine = 40;
    int hightMargine = 50;
    int squareSize = 40;

    Player *player = new Player(10, 10);//player test

    std::vector<std::vector<std::unique_ptr<Tile>>> level;

    Button *mainMenuButton;
public:

    void DrawWindow();

    void HandleEvents(SDL_Event *Event);

    void HandleGameLogic() override;

    void GenerateLevel();

    std::function<void()> ToMainMenu();

    GameWindow(GUI **currentScreen, GUI *oldScreen, SDL_Window **window, SDL_Renderer **renderer);
    ~GameWindow();
};