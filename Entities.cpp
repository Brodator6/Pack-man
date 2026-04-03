#include "Entities.h"

// ------------------------------------ Entity class funcs ------------------------------------------------------------------------
// Constructors and deconstructors
Entity::Entity(/* args */){
}

Entity::Entity(int x, int y):xPosition{x}, yPosition{y}{

}

Entity::~Entity(){

}

//functions
int Entity::GetPositionX() const{
    return xPosition;
}
int Entity::GetPositionY() const{
    return yPosition;
}

void Entity::SetPosition(int x, int y){
    xPosition = x;
    yPosition = y;
};

void Entity::DrawEntity(SDL_Renderer *renderer, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize){
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    this->rect.x = hightMargine + xPosition * cellHight + 5;
    this->rect.y = widthMargine + yPosition * cellWidth + 5;
    this->rect.w = squareSize - 10;
    this->rect.h = squareSize - 10;
    SDL_RenderFillRect(renderer, &this->rect);
};


// ------------------------------------ Entity class funcs ------------------------------------------------------------------------
// Constructors and deconstructors
Player::Player(int x, int y, std::string name, std::map<SDL_Scancode, bool> movement):Entity{x,y}, name{name}, movement{movement}{

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

void Player::UpdateState(int level[10][10]){

}


//  gcc Entities.cpp -o Entities -I "D:\PRG\Pac-man\src\include" -L "D:\PRG\Pac-man\src\lib" -lSDL3 -lstdc++