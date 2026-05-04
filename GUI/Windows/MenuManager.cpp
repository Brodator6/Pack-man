#include "MenuManager.h"
#include "MainMenu.h"
#include "GUI.h"
#include <chrono>
#include <thread>

void MenuManager::DrawMenu(){
    GetMenu()->DrawWindow();
    
    timeBlackboard.frameEnd = std::chrono::high_resolution_clock::now();
    timeBlackboard.frameDuration = timeBlackboard.frameEnd - timeBlackboard.currentTime;
    if(timeBlackboard.frameDuration < std::chrono::milliseconds(16)){
        std::this_thread::sleep_for(std::chrono::milliseconds(16) - timeBlackboard.frameDuration);
    }
};

void MenuManager::HandleEvents(SDL_Event *event){
    GetMenu()->HandleEvents(event);
};

void MenuManager::HandleLogic(){
    GetMenu()->HandleGameLogic();
};

void MenuManager::PushMenu(std::unique_ptr<GUI> newMenu){
    menus.push_back(std::move(newMenu));
};

void MenuManager::PopMenu(){
    if(!menus.empty()){
        menus.pop_back();
    }
};

bool MenuManager::IsEmpty(){
    return menus.empty();
}

GUI* MenuManager::GetRootMenu(){
    return menus.front().get();
};

GUI* MenuManager::GetMenu(){
    return menus.back().get();
};

void MenuManager::RequestRootSwap(std::unique_ptr<GUI> newMenu) {
    pendingRoot = std::move(newMenu);
}

void MenuManager::RequestMenuPop(){
    pendingPop = true;
}

void MenuManager::applyPendingRequest() {
    if (pendingPop) {
        this->PopMenu();
        pendingPop = false;
    }
    if (!pendingRoot) return;

    menus.clear(); 
    menus.push_back(std::move(pendingRoot));
    
    pendingRoot = nullptr; // Reset the request

}

void MenuManager::SetTheStage(SDL_Window **window, SDL_Renderer **renderer, int WindowWidth, int WindowHight){
    menuBlackboard = {
        window,
        renderer,
        WindowWidth, WindowHight,
    };

    PushMenu(std::make_unique<MainMenu>(this, menuBlackboard, timeBlackboard));
}