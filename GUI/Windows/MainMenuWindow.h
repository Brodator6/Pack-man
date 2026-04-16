#pragma once

#include "GUI.h"
#include "GameWindow.h"
#include <functional>
#include <iostream>

// Forward declaration
class GameWindow;

class MainMenuWindow : public GUI{
private:
    Button startButton = Button(300, 200, 200, 50, this->StartTheGame(), "Start" , font, &textColor, (*this->renderer));
    Button settingsButton = Button(300, 300, 200, 50, this->SwitchToTheSettings(), "Settings" , font, &textColor, (*this->renderer));
    Button quitButton = Button(300, 400, 200, 50, this->Quit(), "Quit" , font, &textColor, (*this->renderer));

public:
    void DrawWindow();

    SDL_AppResult HandleEvents(SDL_Event *Event);

    std::function<void()> StartTheGame();

    std::function<void()> SwitchToTheSettings();

    std::function<void()> Quit();

    MainMenuWindow(MenuManager *menus, SDL_Window **window, SDL_Renderer **renderer);
    
    ~MainMenuWindow();
};