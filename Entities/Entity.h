#pragma once

#include <string>
#include <map>
#include <SDL3/SDL.h>

#include <iostream>

enum class Direction{
    Left,
    Up,
    Right,
    Down
};

class Entity
{
protected:
    SDL_FRect rect;
    int xPosition;
    int yPosition;
    SDL_Texture *texture;

public:
    Direction direction;
    int GetPositionX() const;
    int GetPositionY() const;
    void SetPosition(int X, int Y);
    
    virtual void DrawEntity(SDL_Renderer *renderer, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize) = 0;
    
    Entity(int x, int y);
    Entity(int x, int y, SDL_Texture *tex);
    ~Entity() = default;
};