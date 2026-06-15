#pragma once
#include "Abilities.h"
#include "Components.h"
#include "../DataStructs.h"
#include <vector>
#include <map>
#include <memory>
#include <SDL3/SDL.h>

struct TileData;

class Player
{
private:
    Blackboard *blackboard;

public:
    // ECS Components
    PositionComponent positionComponent;
    MovementComponent movementComponent;
    RenderComponent renderComponent;

    // Player-specific data
    Ability abilities[4];
    int ID = 0;
    bool isInvisible = false;
    int score = 0;
    std::map<SDL_Scancode, bool> controls;

    // Accessors for backwards compatibility
    int GetPositionX() const;
    int GetPositionY() const;
    void SetPosition(int X, int Y);
    void SetTexture(const std::shared_ptr<SDL_Texture> &tex);

    int GetScore() const;
    void SetTargetPosition(int x, int y);
    void SetBlackboard(Blackboard *bb);
    void AddScore(int extraPoints);

    bool HasReachedNode() const;


    void Move();
    void UpdateMovement(std::vector<std::vector<TileData>> &level, int rows, int columns);

    void UseAbility();
    void UpdateAbilitiesCooldown(float deltaTime);


    void DrawEntity(SDL_Renderer *renderer, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize, bool freezeAnimations);
    
    Player(){};
    Player(int x, int Y, Blackboard *bb, std::map<SDL_Scancode, bool> controlsMap = {{SDL_SCANCODE_UP, false}, {SDL_SCANCODE_RIGHT, false}, {SDL_SCANCODE_DOWN, false}, {SDL_SCANCODE_LEFT, false}, {SDL_SCANCODE_1, false}, {SDL_SCANCODE_2, false}, {SDL_SCANCODE_3, false}, {SDL_SCANCODE_4, false}});
    ~Player();

};

