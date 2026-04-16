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


class GUI
{
protected:
    //window related data 
    SDL_Window **window = NULL;
    SDL_Renderer **renderer = NULL;
    MenuManager *menus;


    //centralized data for checkbox focusing detection
    int inactiveTextBoxCounter = 0;

    //text related data
    SDL_Color textColor = {0, 0, 0, 255};
    TTF_Font *font = TTF_OpenFont( "./Assets/Font/Nasa21-l23X.ttf", 24);


public:
    virtual void DrawWindow() = 0;

    virtual SDL_AppResult HandleEvents(SDL_Event *Event) = 0;

    virtual  void HandleGameLogic(){

    }


    std::function<void()> BackToPreviousMenu(){
        return [this](){
            if(!menus->IsEmpty()){
            menus->RequestMenuPop();
            }
        };
    }
    
    GUI(MenuManager *menus, SDL_Window **window, SDL_Renderer **renderer){
        this->menus = menus;
        this->window = window;
        this->renderer = renderer;
    }

    ~GUI(){
        std::cout << "Menu deconstructed" << std::endl;
        TTF_CloseFont(font);
    }
};
