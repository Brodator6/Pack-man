#pragma once
#include "Entity.h"
#include "Abilities.h"
#include <vector>

struct TileData;

class Player: public Entity
{
private:
    float speedModifier = 1;
    float visualX, visualY;
    int targetX, targetY;

    std::string name;
    int score = 0;

    Ability abilities[4];

public:
    std::map<SDL_Scancode, bool> controls;

    int GetScore() const;
    void AddScore(int extraPoints);
    std::string GetName() const;
    void SetTargetPosition(int x, int y);

    bool HasReachedNode() const;

    void Move();

    void UpdateMovement(std::vector<std::vector<TileData>> &level, int rows, int columns);

    void UseAbility();

    void UpdateAbilitiesCooldown(float deltaTime);

    void DrawEntity(SDL_Renderer *renderer, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize) override;
    
    
    Player(int x, int Y, SDL_Texture *tex,std::string name = "player", std::map<SDL_Scancode, bool> movement = {{SDL_SCANCODE_UP, false}, {SDL_SCANCODE_RIGHT, false}, {SDL_SCANCODE_DOWN, false}, {SDL_SCANCODE_LEFT, false}, {SDL_SCANCODE_1, true}, {SDL_SCANCODE_2, false}, {SDL_SCANCODE_3, false}, {SDL_SCANCODE_4, false}});
    ~Player();

};

