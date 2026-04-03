#pragma once

#include "GUI.h"
#include "GameWindow.h"
#include <functional>
#include <iostream>

// Forward declaration
class GameWindow;

class MainMenuWindow : public GUI{
private:
    Button *startButton;
    Button *settingsButton;
    Button *quitButton;
    //Button *ProgressionButton;

public:
    void DrawWindow();

    void HandleEvents(SDL_Event *Event);

    std::function<void()> StartTheGame();

    std::function<void()> SwitchToTheSettings();

    std::function<void()> Quit();

    MainMenuWindow(GUI **currentScreen, GUI *oldScreen, SDL_Window **window, SDL_Renderer **renderer);
    
    ~MainMenuWindow();
};