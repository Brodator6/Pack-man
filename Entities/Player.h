#pragma once
#include "Entity.h"

class Player: public Entity
{
private:
    float speedModifier = 1;
    std::string name;
    int score = 0;
    float visualX, visualY;
    int targetX, targetY;

public:
    std::map<SDL_Scancode, bool> controls;

    int GetScore() const;
    void AddScore(int extraPoints);
    std::string GetName() const;
    void SetTargetPosition(int x, int y);
    void Move();

    void DrawEntity(SDL_Renderer *renderer, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize) override;
    
    
    Player(int x, int Y, std::string name = "player", std::map<SDL_Scancode, bool> movement = {{SDL_SCANCODE_UP, false}, {SDL_SCANCODE_RIGHT, false}, {SDL_SCANCODE_DOWN, false}, {SDL_SCANCODE_LEFT, false}});
    ~Player();

};

