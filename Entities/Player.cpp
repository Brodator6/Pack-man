#include "Player.h"
#include "../TileSystem.h"
#include <cmath>

//functions
std::string Player::GetName() const{return name;}

int Player::GetScore() const{return score;}

void Player::AddScore(int extraPoints){score += extraPoints;}

void Player::SetTargetPosition(int x, int y){
    this->targetX = x;
    this->targetY = y;
}

bool Player::HasReachedNode() const {
    return visualX == float(xPosition) && visualY == float(yPosition);
}

void Player::UpdateMovement(std::vector<std::vector<TileData>> &level, int rows, int columns){
    if (this->HasReachedNode()) {
        if(this->controls[SDL_SCANCODE_DOWN] == true){
            this->direction = Direction::Down;
            this->SetTargetPosition(this->GetPositionX(),
            this->GetPositionY() + ((IsWalkable(level[(this->GetPositionY() + 1) % rows][this->GetPositionX()]))) - ((this->GetPositionY() >= rows - 1) * rows));
        }else if(this->controls[SDL_SCANCODE_RIGHT] == true){
            this->direction = Direction::Right;
            this->SetTargetPosition(this->GetPositionX() + ((IsWalkable(level[this->GetPositionY()][(this->GetPositionX() + 1) % columns])))  - ((this->GetPositionX() >= columns - 1) * columns),
            this->GetPositionY());
        }else if(this->controls[SDL_SCANCODE_UP] == true){
            this->direction = Direction::Up;
            this->SetTargetPosition(this->GetPositionX(),
            this->GetPositionY() - ((IsWalkable(level[(this->GetPositionY() - 1 + rows) % rows][this->GetPositionX()])))  + ((this->GetPositionY() <= 0) * rows));
        }else if(this->controls[SDL_SCANCODE_LEFT] == true){
            this->direction = Direction::Left;
            this->SetTargetPosition(this->GetPositionX() - ((IsWalkable(level[this->GetPositionY()][(this->GetPositionX() - 1 + columns) % columns])))  + ((this->GetPositionX() <= 0) * columns),
            this->GetPositionY());
        }
    }
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

void Player::UseAbility(){
    for(int i = 0; i < 4; i++){
        if(abilities[i].cooldownRemaining < 0.1f && !controls[(SDL_Scancode)(i + SDL_SCANCODE_1)]) {continue;}
        if (abilities[i].isConsumable && abilities[i].charges <= 0) {
            abilities[i] = {AbilityType::None, 0, 0, 0, false}; 
            continue;
        }

        abilities[i].maxCooldown = abilities[i].cooldownRemaining;

        if(abilities[i].isConsumable) {abilities[i].charges--;}
        switch (abilities[i].type) {
            case AbilityType::Claymore:
                std::cout << "Casting Fireball" << std::endl;
                break;
            default:
                std::cout << "Unidentified behavior" << std::endl;
                break;
        }
    }
}

void Player::UpdateAbilitiesCooldown(float deltaTime){
    for(auto &ability : abilities){
        if(ability.cooldownRemaining > 0.0f){
            ability.cooldownRemaining -= deltaTime;
        }else{
            ability.cooldownRemaining = 0;
        }
    }
}

void Player::DrawEntity(SDL_Renderer *renderer, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize){
    this->rect.x = hightMargine + visualX * cellHight + 5;
    this->rect.y = widthMargine + visualY * cellWidth + 5;
    this->rect.w = squareSize - 10;
    this->rect.h = squareSize - 10;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &this->rect);
    SDL_RenderTextureRotated(renderer, texture, NULL, &rect, (0.0 + ((direction == Direction::Down) * 90.0) + ((direction == Direction::Left) * 180.0) + ((direction == Direction::Up) * 270.0)), NULL, SDL_FLIP_NONE);
}

Player::Player(int x, int y, SDL_Texture *tex, std::string name, std::map<SDL_Scancode, bool> movement):Entity{x,y, tex}, name{name}, controls{controls}{
    targetX = x;
    targetY = y;
    visualX = x;
    visualY = y;

    abilities[AbilityID::permanentAbility1] = Ability{AbilityType::None, 0, 0, 0, false};
    abilities[AbilityID::permanentAbility2] = {AbilityType::Claymore, 0, 10, 0, false};
    abilities[AbilityID::consumableAbility1] = {AbilityType::Claymore, 0, 0, 0, true};
    abilities[AbilityID::consumableAbility2] = {AbilityType::Claymore, 0, 10, 1, true};

}

Player::~Player(){}