#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <iostream>
#include <string>
#include "functional"

#include "../Elements/GUIElement.h"
#include "../Elements/Button.h"


class GUI
{
protected:
    //window related data 
    SDL_Window **window = NULL;
    GUI **currentScreen = NULL;
    GUI *oldScreen = NULL;
    SDL_Renderer **renderer = NULL;

    //centralized data for checkbox focusing detection
    int inactiveTextBoxCounter = 0;

    //text related data
    SDL_Color textColor = {0, 0, 0, 255};
    TTF_Font *font = TTF_OpenFont( "./Assets/Font/Nasa21-l23X.ttf", 24);


public:
    virtual void DrawWindow() = 0;

    virtual void HandleEvents(SDL_Event *Event) = 0;

    virtual  void HandleGameLogic(){

    }

    void deleteOldScreen(GUI* oldScreen){
        delete oldScreen;
    }
    
    GUI(GUI **currentScreen, GUI *oldScreen, SDL_Window **window, SDL_Renderer **renderer){
        this->currentScreen = currentScreen;
        this->window = window;
        this->renderer = renderer;
        this->oldScreen = oldScreen;
    }

    ~GUI(){
        std::cout << "deconstructed" << std::endl;
        TTF_CloseFont(font);
    }
};
