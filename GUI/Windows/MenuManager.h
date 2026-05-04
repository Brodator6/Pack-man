#pragma once
#include <vector>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include "../../DataStructs.h"

class GUI;

class MenuManager
{
private:
    std::vector<std::unique_ptr<GUI>> menus;
    std::unique_ptr<GUI> pendingRoot = nullptr;
    bool pendingPop = false;

    MenuBlackboard menuBlackboard;

    TimeBlackboard timeBlackboard;

public:

    void DrawMenu();
    void HandleEvents(SDL_Event *event);
    void HandleLogic();

    void PushMenu(std::unique_ptr<GUI> newMenu);
    void PopMenu();
    bool IsEmpty();

    GUI* GetRootMenu();
    GUI* GetMenu();

    void RequestRootSwap(std::unique_ptr<GUI> newMenu);
    void RequestMenuPop();
    void applyPendingRequest();

    void SetTheStage(SDL_Window **window, SDL_Renderer **renderer, int WindowWidth, int WindowHight);
    MenuManager() = default;
    ~MenuManager()  = default;
};
