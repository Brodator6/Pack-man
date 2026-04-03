#pragma once

#include "GUI.h"


class ErrorWindow : public GUI{
private:


public:
    void DrawWindow();

    void HandleEvents(SDL_Event *Event);

    std::function<void()> BackToMainMenu();

    ErrorWindow(GUI **currentScreen, GUI *oldScreen, SDL_Window **window, SDL_Renderer **renderer);

    ~ErrorWindow();
};
