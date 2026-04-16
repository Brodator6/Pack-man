#include "ErrorWindow.h"

void ErrorWindow::DrawWindow(){//drawing all elements
    backButton.DrawElement(*renderer);
}

SDL_AppResult ErrorWindow::HandleEvents(SDL_Event *Event){//handling GUI events
    backButton.HandleKey(*Event);

    return SDL_APP_CONTINUE;
}

ErrorWindow::ErrorWindow(MenuManager *menus, SDL_Window **window, SDL_Renderer **renderer) : GUI(menus, window, renderer), 
    backButton(300, 400, 200, 50, this->BackToPreviousMenu(), "Back" , font, &textColor, (*this->renderer)){
}

ErrorWindow::~ErrorWindow(){
    std::cout << "deleted" << std::endl;
}