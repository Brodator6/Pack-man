#include "ResultsMenu.h"

void ResultsMenu::DrawWindow(){//drawing all elements
    menus->GetRootMenu()->DrawWindow();

    SDL_FRect rect;
    rect = {500, 0, 1000, (float)menuBlackboard.windowHight};

    SDL_SetRenderDrawBlendMode(*menuBlackboard.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(*menuBlackboard.renderer, 100, 100, 100, 250);
    SDL_RenderFillRect(*menuBlackboard.renderer, &rect);
    SDL_SetRenderDrawBlendMode(*menuBlackboard.renderer, SDL_BLENDMODE_NONE);
    
    quitButton.DrawElement(*menuBlackboard.renderer);
    scoreText.DrawElement(*menuBlackboard.renderer);
    toNextLevelButton.DrawElement(*menuBlackboard.renderer);
    resultMenuTitle.DrawElement(*menuBlackboard.renderer);
}

SDL_AppResult ResultsMenu::HandleEvents(SDL_Event *Event){//handling GUI events
    quitButton.HandleKey(*Event);
    toNextLevelButton.HandleKey(*Event);

    return SDL_APP_CONTINUE;
}

void ResultsMenu::HandleGameLogic(){
    timeBlackboard.previousTickTime = timeBlackboard.currentTime;
    timeBlackboard.currentTime = std::chrono::high_resolution_clock::now();
}

std::function<void()> ResultsMenu::ToMainMenu(){
    return [this](){
        menus->RequestRootSwap(std::make_unique<MainMenu>(menus, menuBlackboard, timeBlackboard));
    };
}

ResultsMenu::ResultsMenu(MenuManager *menus, MenuBlackboard &mBB, TimeBlackboard &tBB, ResultDataBlackboard resultbb, std::function<void()> lambdaFunction) 
: GUI(menus, mBB, tBB), levelSwitchFunction(lambdaFunction), 
resultMenuTitle(850, 100, 100, 100, resultbb.ResultTitle, font, {255, 255, 255, 0}, (*menuBlackboard.renderer)),
scoreText(850, 300, 100, 100, "Score: " + std::to_string(resultbb.score), font, {255, 255, 255, 0}, (*menuBlackboard.renderer)),
toNextLevelButton(Button(850, 600, 200, 70, levelSwitchFunction, "To Next Level" , font, textColor, (*menuBlackboard.renderer))),
quitButton(Button(850, 800, 200, 70, this->ToMainMenu(), "To Main Menu" , font, textColor, (*menuBlackboard.renderer)))
{
    if(resultbb.lost == true){
        toNextLevelButton = Button(-200, -200, 0, 0, this->ToMainMenu(), "" , font, textColor, (*menuBlackboard.renderer));
    }
}

ResultsMenu::~ResultsMenu(){
}