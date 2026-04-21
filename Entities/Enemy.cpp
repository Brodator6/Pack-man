#include "Enemy.h"
#include <cmath>
#include "./Pathfinding/Pathfinding.h"
#include "./AI/BehaviorTree.h"

bool Enemy::HasReachedNode() {
    return visualX == float(xPosition) && visualY == float(yPosition);
}

void Enemy::SetTargetPosition(int x, int y){
    targetX = x;
    targetY = y;
    int dx = targetX - xPosition;
    int dy = targetY - yPosition;

    if (dx > 0) direction = Direction::Right;
    else if (dx < 0) direction = Direction::Left;
    else if (dy > 0) direction = Direction::Down;
    else if (dy < 0) direction = Direction::Up;
}

void Enemy::UpdateMovement(){
    if (HasReachedNode() && !currentPath.empty()){
        auto nextNode = currentPath.back();
        currentPath.pop_back();
        SetTargetPosition(nextNode.x, nextNode.y);
    }
}

void Enemy::Move(){
    int dx = targetX - xPosition;
    int dy = targetY - yPosition;

    float step = 0.2f * DinamicEntity.speedModifier;
    float dirX = (direction == Direction::Right) - (direction == Direction::Left);
    float dirY = (direction == Direction::Down) - (direction == Direction::Up);
    visualX += dirX * step;
    visualY += dirY * step;

    if (dx == 0) visualX = float(xPosition);
    if (dy == 0) visualY = float(yPosition);

    if (dx != 0 && std::abs(visualX - targetX) < 0.2f) {
        visualX = float(targetX);
        xPosition = targetX;
    }
    if (dy != 0 && std::abs(visualY - targetY) < 0.2f) {
        visualY = float(targetY);
        yPosition = targetY;
    }
}

void Enemy::DrawEntity(SDL_Renderer *renderer, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize){
    this->rect.x = hightMargine + visualX * cellHight + 5;
    this->rect.y = widthMargine + visualY * cellWidth + 5;
    this->rect.w = squareSize - 10;
    this->rect.h = squareSize - 10;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &this->rect);
    SDL_RenderTextureRotated(renderer, texture, NULL, &rect, (0.0 + ((direction == Direction::Down) * 90.0) + ((direction == Direction::Left) * 180.0) + ((direction == Direction::Up) * 270.0)), NULL, SDL_FLIP_NONE);
}

Enemy::Enemy(int x, int y, SDL_Texture *tex) : Entity{x,y, tex}{
    targetX = x;
    targetY = y;
    visualX = float(x);
    visualY = float(y);
    direction = Direction::Up;
}