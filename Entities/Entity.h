#pragma once

#include <string>
#include <map>
#include <SDL3/SDL.h>

class Entity
{
protected:
    SDL_FRect rect;
    int xPosition;
    int yPosition;
public:
    int GetPositionX() const;
    int GetPositionY() const;
    void SetPosition(int X, int Y);
    
    virtual void DrawEntity(SDL_Renderer *renderer, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize) = 0;
    
    //void DoMovement(int level[10][10]);
    Entity(int x, int y);
    ~Entity() = default;
};