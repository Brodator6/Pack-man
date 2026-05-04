#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

#include <iostream>
#include <string>
#include <memory>
#include "functional"

#include "MenuManager.h"
#include "../Elements/GUIElement.h"
#include "../Elements/Button.h"
#include "../../DataStructs.h"

class GUI
{
protected:
    //window related data 
    MenuManager *menus;
    MenuBlackboard &menuBlackboard;

    TimeBlackboard &timeBlackboard;

    //centralized data for checkbox focusing detection
    int inactiveTextBoxCounter = 0;

    //text related data
    SDL_Color textColor = {0, 0, 0, 255};
    TTF_Font *font = TTF_OpenFont( "./Assets/Font/Nasa21-l23X.ttf", 24);


public:
    virtual void DrawWindow(){};

    virtual SDL_AppResult HandleEvents(SDL_Event *Event){};

    virtual  void HandleGameLogic(){};
    
    std::function<void()> BackToPreviousMenu(){
        return [this](){
            if(!menus->IsEmpty()){
            menus->RequestMenuPop();
            }
        };
    }
    
    GUI(MenuManager *menus, MenuBlackboard &mBB, TimeBlackboard &tBB) : menuBlackboard{mBB}, timeBlackboard{tBB}{
        this->menus = menus;
    }

    ~GUI(){
        TTF_CloseFont(font);
    }
};
