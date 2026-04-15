#pragma once

#include "GUI.h"


class ErrorWindow : public GUI{
private:
    Button backButton;
public:
    void DrawWindow();

    SDL_AppResult HandleEvents(SDL_Event *Event);

    ErrorWindow(MenuManager *menus, SDL_Window **window, SDL_Renderer **renderer);

    ~ErrorWindow();
};
