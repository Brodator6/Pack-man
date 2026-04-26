#include "PauseMenu.h"

void PauseMenu::DrawWindow(){//drawing all elements
    menus->GetRootMenu()->DrawWindow();

    resumeButton.DrawElement(*renderer);
    quitButton.DrawElement(*renderer);
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
    blackboard.previousTimeFrame = blackboard.currentTimeFrame;
    blackboard.currentTimeFrame = std::chrono::high_resolution_clock::now();
}

std::function<void()> PauseMenu::ToMainMenu(){
    return [this](){
        menus->RequestRootSwap(std::make_unique<MainMenu>(menus, window, renderer));
    };
}

PauseMenu::PauseMenu(MenuManager *menus, SDL_Window **window, SDL_Renderer **renderer, PauseMenuDataTransportationStruct bb) : GUI(menus, window, renderer), blackboard{bb}{
}

PauseMenu::~PauseMenu(){
    std::cout << "deleted" << std::endl;
}