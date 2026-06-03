#pragma once

#include "GUI.h"
#include "GameMenu.h"
#include "../Elements/Text.h"

struct ResultDataBlackboard{
    std::string ResultTitle = "Lost";
    int score = 0;
    int lost = false;
};

struct GameStateData{

};

class ResultsMenu : public GUI{
private:
    std::function<void()> levelSwitchFunction;
    Button toNextLevelButton;
    Button quitButton;

    Text resultMenuTitle;
    Text scoreText;
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
