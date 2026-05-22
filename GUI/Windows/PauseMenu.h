#pragma once

#include "GUI.h"
#include "GameMenu.h"

class PauseMenu : public GUI{
private:
    Button resumeButton = Button(850, 300, 300, 70, this->BackToPreviousMenu(), "Resume" , font, textColor, (*menuBlackboard.renderer));
    Button quitButton = Button(850, 440, 300, 70, this->ToMainMenu(), "To main menu" , font, textColor, (*menuBlackboard.renderer));
public:
    void DrawWindow() override;

    SDL_AppResult HandleEvents(SDL_Event *Event);

    void HandleGameLogic() override;

    std::function<void()> ToMainMenu();

    PauseMenu(MenuManager *menus, MenuBlackboard &mBB, TimeBlackboard &tBB);

    ~PauseMenu();
};
