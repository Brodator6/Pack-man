#include "MainMenuWindow.h"

void MainMenuWindow::DrawWindow(){
    startButton->DrawElement((*this->renderer));
    settingsButton->DrawElement((*this->renderer));
    quitButton->DrawElement((*this->renderer));
}

void MainMenuWindow::HandleEvents(SDL_Event *Event){
    startButton->HandleKey((*Event));
    settingsButton->HandleKey((*Event));
    quitButton->HandleKey((*Event));
}


std::function<void()> MainMenuWindow::StartTheGame(){
    return [this](){
        *currentScreen = new GameWindow(currentScreen, *currentScreen, window, renderer);
        delete oldScreen;
    };
}


std::function<void()> MainMenuWindow::SwitchToTheSettings(){
    return [this](){
        std::cout << "Start" << std::endl;
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


MainMenuWindow::MainMenuWindow(GUI **currentScreen, GUI *oldScreen, SDL_Window **window, SDL_Renderer **renderer) : GUI(currentScreen, oldScreen, window, renderer){
    startButton = new Button(300, 200, 200, 50, this->StartTheGame(), "Start" , font, &textColor, (*this->renderer));
    settingsButton = new Button(300, 300, 200, 50, this->SwitchToTheSettings(), "Settings" , font, &textColor, (*this->renderer));
    quitButton = new Button(300, 400, 200, 50, this->Quit(), "Quit" , font, &textColor, (*this->renderer));

}

MainMenuWindow::~MainMenuWindow(){
    std::cout << "deconstructed" << std::endl;
}