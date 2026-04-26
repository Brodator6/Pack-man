#pragma once

#include "GUI.h"
#include "GameMenu.h"
#include <functional>
#include <iostream>

// Forward declaration
class GameWindow;

class MainMenu : public GUI{
private:
    Button startButton = Button(300, 200, 200, 50, this->StartTheGame(), "Start" , font, &textColor, (*this->renderer));
    Button settingsButton = Button(300, 300, 200, 50, this->SwitchToTheSettings(), "Settings" , font, &textColor, (*this->renderer));
    Button quitButton = Button(300, 400, 200, 50, this->Quit(), "Quit" , font, &textColor, (*this->renderer));

public:
    void DrawWindow() override;

    SDL_AppResult HandleEvents(SDL_Event *Event) override;

    std::function<void()> StartTheGame();

    std::function<void()> SwitchToTheSettings();

    std::function<void()> Quit();

    MainMenu(MenuManager *menus, SDL_Window **window, SDL_Renderer **renderer);
    
    ~MainMenu();
};