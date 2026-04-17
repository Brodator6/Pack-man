#include "Enemy.h"
#include <Cmath>
#include "./Pathfinding/Pathfinding.h"
#include "./AI/BehaviorTree.h"

void Enemy::DrawEntity(SDL_Renderer *renderer, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize){
    this->rect.x = hightMargine + xPosition * cellHight + 5;
    this->rect.y = widthMargine + yPosition * cellWidth + 5;
    this->rect.w = squareSize - 10;
    this->rect.h = squareSize - 10;
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &this->rect);
    SDL_RenderTextureRotated(renderer, texture, NULL, &rect, (0.0 + ((direction == Direction::Down) * 90.0) + ((direction == Direction::Left) * 180.0) + ((direction == Direction::Up) * 270.0)), NULL, SDL_FLIP_NONE);
}

Enemy::Enemy(int x, int y, SDL_Texture *tex) : Entity{x, y, tex}{
    goalX = x; 
    goalY = y;
    direction = Direction::Up;
    AI = BuildAI();
}