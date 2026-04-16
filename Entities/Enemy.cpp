#include "Enemy.h"
#include <Cmath>
#include "./Pathfinding/Pathfinding.h"
#include "./AI/BehaviorTree.h"

void Enemy::DrawEntity(SDL_Renderer *renderer, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize){
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    this->rect.x = hightMargine + xPosition * cellHight + 5;
    this->rect.y = widthMargine + yPosition * cellWidth + 5;
    this->rect.w = squareSize - 10;
    this->rect.h = squareSize - 10;
    SDL_RenderFillRect(renderer, &this->rect);
}

Enemy::Enemy(int x, int y):Entity{x,y}{
    goalX = x; 
    goalY = y;
    direction = Direction::Up;
    AI = BuildAI();
}