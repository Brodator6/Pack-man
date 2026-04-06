#pragma once
#include "Entity.h"

class Player: public Entity
{
private:
    std::string name;
    int score = 0;

public:
    std::map<SDL_Scancode, bool> movement;

    int GetScore() const;
    void AddScore(int extraPoints);
    std::string GetName() const;
    void UpdateState(int level[10][10]);
    void DrawEntity(SDL_Renderer *renderer, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize) override;
    Player(int x, int Y, std::string name = "player", std::map<SDL_Scancode, bool> movement = {{SDL_SCANCODE_UP, false}, {SDL_SCANCODE_RIGHT, false}, {SDL_SCANCODE_DOWN, false}, {SDL_SCANCODE_LEFT, false}});
    ~Player();

};

