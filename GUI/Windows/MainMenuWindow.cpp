#include "MainMenuWindow.h"
#include "ErrorWindow.h"

void MainMenuWindow::DrawWindow(){
    startButton.DrawElement((*this->renderer));
    settingsButton.DrawElement((*this->renderer));
    quitButton.DrawElement((*this->renderer));
}

SDL_AppResult MainMenuWindow::HandleEvents(SDL_Event *Event){
    startButton.HandleKey((*Event));
    settingsButton.HandleKey((*Event));
    quitButton.HandleKey((*Event));

    return SDL_APP_CONTINUE;
}


std::function<void()> MainMenuWindow::StartTheGame(){
    return [this](){
        menus->RequestRootSwap(std::make_unique<GameWindow>(menus, window, renderer));
    };
}


std::function<void()> MainMenuWindow::SwitchToTheSettings(){
    return [this](){
        menus->PushMenu(std::make_unique<ErrorWindow>(menus, window, renderer));
    };
}

std::function<void()> MainMenuWindow::Quit(){
    return [this](){
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyRenderer(*renderer);
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
    };
}


MainMenuWindow::MainMenuWindow(MenuManager *menus, SDL_Window **window, SDL_Renderer **renderer) : GUI(menus, window, renderer){
    
}

MainMenuWindow::~MainMenuWindow(){
    std::cout << "deconstructed" << std::endl;
}