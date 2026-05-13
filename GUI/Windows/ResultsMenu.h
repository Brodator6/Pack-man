#pragma once

#include "GUI.h"
#include "GameMenu.h"

struct ResultDataBlackboard{
    std::string ResultTitle = "Lost";
    int score = 0;
};

struct GameStateData{

};

class ResultsMenu : public GUI{
private:
    std::function<void()> menuSwitchFunction;
    Text resultMenuTitle;
    ResultDataBlackboard *resultData = nullptr;
    GameStateData * gamestateData = nullptr;
public:
    void DrawWindow() override;

    SDL_AppResult HandleEvents(SDL_Event *Event);

    void HandleGameLogic() override;

    std::function<void()> ToMainMenu();
    
    std::function<void()> ToNextLevel();

    ResultsMenu(MenuManager *menus, MenuBlackboard &mBB, TimeBlackboard &tBB, ResultDataBlackboard resultbb, std::function<void()> lambdaFunction);

    ~ResultsMenu();
};
