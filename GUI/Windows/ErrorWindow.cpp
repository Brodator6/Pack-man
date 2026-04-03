#include "ErrorWindow.h"

void ErrorWindow::DrawWindow(){//drawing all elements


}

void ErrorWindow::HandleEvents(SDL_Event *Event){//handling GUI events

}

std::function<void()> ErrorWindow::BackToMainMenu(){
    return [this](){
        currentScreen = nullptr;
    };
}

ErrorWindow::ErrorWindow(GUI **currentScreen, GUI *oldScreen, SDL_Window **window, SDL_Renderer **renderer) : GUI(currentScreen, oldScreen, window, renderer){

}

ErrorWindow::~ErrorWindow(){
    
}