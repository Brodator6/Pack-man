#include "MainMenu.h"
#include "PauseMenu.h"

void MainMenu::DrawWindow(){
    startButton.DrawElement((*menuBlackboard.renderer));
    settingsButton.DrawElement((*menuBlackboard.renderer));
    quitButton.DrawElement((*menuBlackboard.renderer));
}

SDL_AppResult MainMenu::HandleEvents(SDL_Event *Event){
    startButton.HandleKey((*Event));
    settingsButton.HandleKey((*Event));
    quitButton.HandleKey((*Event));

    return SDL_APP_CONTINUE;
}


std::function<void()> MainMenu::StartTheGame(){
    return [this](){
        menus->RequestRootSwap(std::make_unique<GameMenu>(menus, menuBlackboard, timeBlackboard));
    };
}


std::function<void()> MainMenu::SwitchToTheSettings(){
    return [this](){
        std::cout << "Settings" << std::endl;
        //menus->PushMenu(std::make_unique<PauseMenu>(menus, menuBlackboard, timeBlackboard));
    };
}

std::function<void()> MainMenu::Quit(){
    return [this](){
        SDL_DestroyRenderer(*menuBlackboard.renderer);
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
    };
}


MainMenu::MainMenu(MenuManager *menus, MenuBlackboard &mBB, TimeBlackboard &tBB) : GUI(menus, mBB, tBB){
}

MainMenu::~MainMenu(){
}