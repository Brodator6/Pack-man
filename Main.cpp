// gcc *.cpp GUI/Elements/*.cpp GUI/Windows/*.cpp Entities/*.cpp  Entities/Pathfinding/*.cpp -o main -I "src\SDL_Lib\include" -I "src\TTF_Lib\include" -L "src\SDL_Lib\lib" -L "src\TTF_Lib\lib" -lSDL3 -lstdc++ -lSDL3_ttf

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL_main.h>
#include <iostream>
#include <vector>
#include <fstream>

#include "GUI/Windows/GUI.h"
#include "GUI/Windows/MenuManager.h"
#include "GUI/Windows/MainMenuWindow.h"
#include "./Entities/Entity.h"

/* We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
SDL_FRect rect;

MenuManager menus;

//some parameters for displaying staff
const int rows = 20;
const int columns = 20;
int cellWidth = 40;
int cellHight = 40;
int widthMargine = 40;
int hightMargine = 50;
int squareSize = 40;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{

    //std::ifstream file("./Assets/Levels/level1.txt");
    //std::cout << file.is_open() << std::endl;

    SDL_SetAppMetadata("Example Renderer Primitives", "0.3", "Puck-man");

    if (!TTF_Init()) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Puck-man", columns * cellWidth + widthMargine * 2, rows * cellHight + hightMargine * 2, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, columns * cellWidth + widthMargine * 2, rows * cellHight + hightMargine * 2, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    SDL_SetWindowTitle(window, "sas");

    menus.PushMenu(std::make_unique<MainMenuWindow>(&menus, &window, &renderer));

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}


/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    menus.GetMenu()->HandleEvents(event);
    switch (event->type)
    {
    case SDL_EVENT_MOUSE_MOTION:
        break;
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    
    default:
        break;
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}


/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    menus.applyPendingRequest();
    
    DrawBackground(renderer);

    menus.GetMenu()->HandleGameLogic();

    menus.GetMenu()->DrawWindow();

    SDL_RenderPresent(renderer);
    
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}


/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyRenderer(renderer);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    TTF_Quit();
    SDL_Quit();
    /* SDL will clean up the window/renderer for us. */
}