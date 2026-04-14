#include "Player.h"
#include <cmath>

Player::Player(int x, int y, std::string name, std::map<SDL_Scancode, bool> movement):Entity{x,y}, name{name}, controls{controls}{
    targetX = x;
    targetY = y;
    visualX = x;
    visualY = y;
}

Player::~Player(){

}

//functions
std::string Player::GetName() const{
    return name;
}

int Player::GetScore() const{
    return score;
}

void Player::AddScore(int extraPoints){
    score += extraPoints;
}

void Player::SetTargetPosition(int x, int y){
    this->targetX = x;
    this->targetY = y;
}

void Player::Move(){
    /*
    visualX = visualX + 0.2 *(targetX - xPosition) * speedModifier;
    visualY = visualY + 0.2 *(targetY - yPosition) * speedModifier;

    xPosition += (targetX - xPosition) * (std::abs((float)(visualX - targetX)) < 0.1);
    yPosition += (targetY - yPosition) * (std::abs((float)(visualY - targetY)) < 0.1);
    */

    int dx = targetX - xPosition;
    int dy = targetY - yPosition;

    float step = 0.2f * speedModifier;
    float dirX = dx == 0 ? 0.0f : (dx > 0 ? 1.0f : -1.0f);
    float dirY = dy == 0 ? 0.0f : (dy > 0 ? 1.0f : -1.0f);

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

void Player::DrawEntity(SDL_Renderer *renderer, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize){
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    this->rect.x = hightMargine + visualX * cellHight + 5;
    this->rect.y = widthMargine + visualY * cellWidth + 5;
    this->rect.w = squareSize - 10;
    this->rect.h = squareSize - 10;
    SDL_RenderFillRect(renderer, &this->rect);
}


//  gcc Entities.cpp -o Entities -I "D:\PRG\Pac-man\src\include" -L "D:\PRG\Pac-man\src\lib" -lSDL3 -lstdc++