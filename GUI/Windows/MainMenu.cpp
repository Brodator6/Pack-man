#include "MainMenu.h"
#include "PauseMenu.h"

void MainMenu::DrawWindow(){
    startButton.DrawElement((*this->renderer));
    settingsButton.DrawElement((*this->renderer));
    quitButton.DrawElement((*this->renderer));
}

SDL_AppResult MainMenu::HandleEvents(SDL_Event *Event){
    startButton.HandleKey((*Event));
    settingsButton.HandleKey((*Event));
    quitButton.HandleKey((*Event));

    return SDL_APP_CONTINUE;
}


std::function<void()> MainMenu::StartTheGame(){
    return [this](){
        menus->RequestRootSwap(std::make_unique<GameMenu>(menus, window, renderer));
    };
}


std::function<void()> MainMenu::SwitchToTheSettings(){
    return [this](){
        std::cout << "Settings" << std::endl;
        //menus->PushMenu(std::make_unique<PauseMenu>(menus, window, renderer));
    };
}

std::function<void()> MainMenu::Quit(){
    return [this](){
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyRenderer(*renderer);
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
    };
}


MainMenu::MainMenu(MenuManager *menus, SDL_Window **window, SDL_Renderer **renderer) : GUI(menus, window, renderer){
    
}

MainMenu::~MainMenu(){
    std::cout << "deconstructed" << std::endl;
}