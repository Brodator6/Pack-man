#include "PauseMenu.h"

void PauseMenu::DrawWindow(){//drawing all elements
    menus->GetRootMenu()->DrawWindow();

    resumeButton.DrawElement(*menuBlackboard.renderer);
    quitButton.DrawElement(*menuBlackboard.renderer);
}

SDL_AppResult PauseMenu::HandleEvents(SDL_Event *Event){//handling GUI events
    resumeButton.HandleKey(*Event);
    quitButton.HandleKey(*Event);

    switch (Event->type){
    case SDL_EVENT_KEY_DOWN:
        switch (Event->key.scancode){
            case SDL_SCANCODE_ESCAPE:
                menus->PopMenu();
            default: 
                break;
        }
    default:
        break;
    }

    return SDL_APP_CONTINUE;
}

void PauseMenu::HandleGameLogic(){
    timeBlackboard.previousTickTime = timeBlackboard.currentTime;
    timeBlackboard.currentTime = std::chrono::high_resolution_clock::now();
}

std::function<void()> PauseMenu::ToMainMenu(){
    return [this](){
        menus->RequestRootSwap(std::make_unique<MainMenu>(menus, menuBlackboard, timeBlackboard));
    };
}

PauseMenu::PauseMenu(MenuManager *menus, MenuBlackboard &mBB, TimeBlackboard &tBB) : GUI(menus, mBB, tBB){
}

PauseMenu::~PauseMenu(){
}