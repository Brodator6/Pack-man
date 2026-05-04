#pragma once

#include <string>
#include <map>
#include <memory>
#include <SDL3/SDL.h>

#include <iostream>

enum class ActorType{
    DinamicActor,
    StaticActor,
    None
};

enum Direction{
    Left = 0,
    Up = 1,
    Right = 2,
    Down = 3
};

class Entity
{
public:
    float visualX, visualY;
    int targetX, targetY;
    SDL_FRect rect;
    int xPosition;
    int yPosition;
    std::shared_ptr<SDL_Texture> texture;
    ActorType actorType = ActorType::None;
    Direction direction;
    
    int GetPositionX() const;
    int GetPositionY() const;
    void SetPosition(int X, int Y);
    void SetTexture(const std::shared_ptr<SDL_Texture> &tex);
    
    virtual void DrawEntity(SDL_Renderer *renderer, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize) = 0;
    
    Entity(){};
    Entity(int x, int y);
    Entity(int x, int y, std::shared_ptr<SDL_Texture> tex);
    ~Entity() = default;
};