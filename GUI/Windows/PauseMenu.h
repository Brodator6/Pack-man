#pragma once

#include "GUI.h"
#include "GameMenu.h"

struct PauseMenuDataTransportationStruct{
    std::chrono::time_point<std::chrono::high_resolution_clock> &previousTimeFrame;
    std::chrono::time_point<std::chrono::high_resolution_clock> &currentTimeFrame;
};


class PauseMenu : public GUI{
private:
    Button resumeButton = Button(300, 300, 200, 50, this->BackToPreviousMenu(), "Resume" , font, &textColor, (*this->renderer));
    Button quitButton = Button(300, 400, 200, 50, this->ToMainMenu(), "To main menu" , font, &textColor, (*this->renderer));
    PauseMenuDataTransportationStruct blackboard;
public:
    void DrawWindow() override;

    SDL_AppResult HandleEvents(SDL_Event *Event);

    void HandleGameLogic() override;

    std::function<void()> ToMainMenu();

    PauseMenu(MenuManager *menus, SDL_Window **window, SDL_Renderer **renderer, PauseMenuDataTransportationStruct);

    ~PauseMenu();
};
