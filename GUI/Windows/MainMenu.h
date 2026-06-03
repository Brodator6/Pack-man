#pragma once

#include "GUI.h"
#include "GameMenu.h"
#include <functional>
#include <iostream>

// Forward declaration
class GameWindow;

class MainMenu : public GUI{
private:
    Button startButton = Button(850, 300, 200, 50, this->StartTheGame(), "Start" , font, textColor, (*menuBlackboard.renderer));
    Button quitButton = Button(850, 400, 200, 50, this->Quit(), "Quit" , font, textColor, (*menuBlackboard.renderer));

public:
    void DrawWindow() override;

    SDL_AppResult HandleEvents(SDL_Event *Event) override;

    std::function<void()> StartTheGame();

    std::function<void()> SwitchToTheSettings();

    std::function<void()> Quit();

    MainMenu(MenuManager *menus, MenuBlackboard &mBB, TimeBlackboard &tBB);
    
    ~MainMenu();
};