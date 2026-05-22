#pragma once
#include "GUI.h"
#include "MainMenu.h"

#include "../../DataStructs.h"
#include "../../Entities/GameManager.h"
#include "../Elements/AbilityIcon.h"
#include "../Elements/Text.h"

#include <chrono>
#include <memory>




// Forward declaration
class MainMenuWindow;
class Tile;


class GameMenu : public GUI{
private:
    //data to manage game loop
    std::chrono::duration<double> accumulator = std::chrono::duration<float>(0.0);


    int cellWidth = 40;
    int cellHight = 40;
    int widthMargine = 40;
    int hightMargine = 50;
    int squareSize = 40;

    Text scoreText = Text(1400, 0, 100, 100, "Score: 0" , font, textColor, (*menuBlackboard.renderer));

    AbilityIcon abilityIcon1 = AbilityIcon(1400, 100, 200, 200);
    AbilityIcon abilityIcon2 = AbilityIcon(1400, 350, 200, 200);
    AbilityIcon abilityIcon3 = AbilityIcon(1400, 600, 200, 200);
    AbilityIcon abilityIcon4 = AbilityIcon(1400, 850, 200, 200);
    GameManager gameManager;


public:

    void DrawWindow() override;

    SDL_AppResult HandleEvents(SDL_Event *Event) override;

    void HandleGameLogic() override;

    std::function<void()> ToMainMenu();

    std::function<void()>  ToTheNextLevel();

    GameMenu(MenuManager *menus, MenuBlackboard &mBB, TimeBlackboard &tBB);
    GameMenu(MenuManager *menus, MenuBlackboard &mBB, TimeBlackboard &tBB, const GameManager &previousManager);
    ~GameMenu();
};