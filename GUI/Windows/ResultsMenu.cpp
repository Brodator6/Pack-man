#include "ResultsMenu.h"

void ResultsMenu::DrawWindow(){//drawing all elements
    //menus->GetRootMenu()->DrawWindow();


    resultMenuTitle.DrawElement(*menuBlackboard.renderer);
}

SDL_AppResult ResultsMenu::HandleEvents(SDL_Event *Event){//handling GUI events

    switch (Event->type){
    case SDL_EVENT_KEY_DOWN:{
        menuSwitchFunction();
    }
    default:
        break;
    }

    return SDL_APP_CONTINUE;
}

void ResultsMenu::HandleGameLogic(){
    timeBlackboard.previousTickTime = timeBlackboard.currentTime;
    timeBlackboard.currentTime = std::chrono::high_resolution_clock::now();
}

ResultsMenu::ResultsMenu(MenuManager *menus, MenuBlackboard &mBB, TimeBlackboard &tBB, ResultDataBlackboard resultbb, std::function<void()> lambdaFunction) : GUI(menus, mBB, tBB), menuSwitchFunction(lambdaFunction), resultMenuTitle(400, 100, 100, 100, resultbb.ResultTitle, font, textColor, (*menuBlackboard.renderer)){

}

ResultsMenu::~ResultsMenu(){
}