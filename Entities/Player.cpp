#include "Player.h"
#include "../TileSystem.h"
#include "EntityFactory.h"
#include <cmath>

//functions
int Player::GetScore() const{return score;}

void Player::AddScore(int extraPoints){score += extraPoints;}

void Player::SetTargetPosition(int x, int y){
    this->movement.targetX = x;
    this->movement.targetY = y;
}

bool Player::HasReachedNode() const {
    return position.visualX == float(position.x) && position.visualY == float(position.y);
}

int Player::GetPositionX() const {
    return position.x;
}

int Player::GetPositionY() const {
    return position.y;
}

void Player::SetPosition(int X, int Y) {
    position.x = X;
    position.y = Y;
}

void Player::SetTexture(const std::shared_ptr<SDL_Texture> &tex) {
    render.texture = tex;
}

void Player::UpdateMovement(std::vector<std::vector<TileData>> &level, int rows, int columns){
    if (this->HasReachedNode()) {
        if(this->controls[SDL_SCANCODE_DOWN] == true){
            this->position.direction = EntityDirection::Down;
            this->SetTargetPosition(this->GetPositionX(),
            this->GetPositionY() + ((IsWalkable(level[(this->GetPositionY() + 1) % rows][this->GetPositionX()]))) - ((this->GetPositionY() >= rows - 1) * rows));
        }else if(this->controls[SDL_SCANCODE_RIGHT] == true){
            this->position.direction = EntityDirection::Right;
            this->SetTargetPosition(this->GetPositionX() + ((IsWalkable(level[this->GetPositionY()][(this->GetPositionX() + 1) % columns])))  - ((this->GetPositionX() >= columns - 1) * columns),
            this->GetPositionY());
        }else if(this->controls[SDL_SCANCODE_UP] == true){
            this->position.direction = EntityDirection::Up;
            this->SetTargetPosition(this->GetPositionX(),
            this->GetPositionY() - ((IsWalkable(level[(this->GetPositionY() - 1 + rows) % rows][this->GetPositionX()])))  + ((this->GetPositionY() <= 0) * rows));
        }else if(this->controls[SDL_SCANCODE_LEFT] == true){
            this->position.direction = EntityDirection::Left;
            this->SetTargetPosition(this->GetPositionX() - ((IsWalkable(level[this->GetPositionY()][(this->GetPositionX() - 1 + columns) % columns])))  + ((this->GetPositionX() <= 0) * columns),
            this->GetPositionY());
        }
    }
}

void Player::Move(){
    int originalX = position.x;
    int originalY = position.y;

    int dx = movement.targetX - position.x;
    int dy = movement.targetY - position.y;

    float step = 0.2f * movement.speedModifier;
    float dirX = dx == 0 ? 0.0f : (dx > 0 ? 1.0f : -1.0f);
    float dirY = dy == 0 ? 0.0f : (dy > 0 ? 1.0f : -1.0f);

    float moveX = dirX * step;
    float moveY = dirY * step;
    if (dx != 0 && std::abs(moveX) > std::abs(movement.targetX - position.visualX)) {
        moveX = movement.targetX - position.visualX;
    }
    if (dy != 0 && std::abs(moveY) > std::abs(movement.targetY - position.visualY)) {
        moveY = movement.targetY - position.visualY;
    }

    position.visualX += moveX;
    position.visualY += moveY;

    if (dx == 0) position.visualX = float(position.x);
    if (dy == 0) position.visualY = float(position.y);

    if (dx != 0 && std::abs(position.visualX - movement.targetX) < 0.001f) {
        position.visualX = float(movement.targetX);
        position.x = movement.targetX;
    }
    if (dy != 0 && std::abs(position.visualY - movement.targetY) < 0.001f) {
        position.visualY = float(movement.targetY);
        position.y = movement.targetY;
    }
}

void Player::UseAbility(){
    for(int i = 0; i < 4; i++){
        Ability &ability = abilities[i];
        SDL_Scancode key = static_cast<SDL_Scancode>(SDL_SCANCODE_1 + i);
        if(!controls[key]) continue;
        if(ability.cooldownRemaining > 0.0f) continue;
        if(ability.isConsumable && ability.charges <= 0){
            ability.type = AbilityType::None;
            ability.isConsumable = false;
            continue;
        }
        if((ability.type == AbilityType::SpeedBoost || ability.type == AbilityType::Invisibility) && ability.durationRemaining > 0.0f){
            continue;
        }

        int dirX = (position.direction == EntityDirection::Right) - (position.direction == EntityDirection::Left);
        int dirY = (position.direction == EntityDirection::Down) - (position.direction == EntityDirection::Up);
        int frontX = this->GetPositionX() + dirX;
        int frontY = this->GetPositionY() + dirY;
        bool used = false;

        switch (ability.type) {
            case AbilityType::Claymore: {
                if (frontX >= 0 && frontX < blackboard->columns && frontY >= 0 && frontY < blackboard->rows && blackboard->level[frontY][frontX].isWalkable) {
                    blackboard->entityManager.AddEntity(frontX, frontY, position.direction, EntityType::Claymore);
                    used = true;
                }
                break;
            }
            case AbilityType::WallCharge: {
                if(frontX >= 0 && frontX < blackboard->columns && frontY >= 0 && frontY < blackboard->rows && !blackboard->level[frontY][frontX].isWalkable){
                    blackboard->entityManager.AddEntity(frontX, frontY, position.direction, EntityType::WallCharge);
                    used = true;
                }
                break;
            }
            case AbilityType::RoadBlocker: {
                if(frontX >= 0 && frontX < blackboard->columns && frontY >= 0 && frontY < blackboard->rows && blackboard->level[frontY][frontX].isWalkable){
                    blackboard->level[frontY][frontX].type = WALL;
                    blackboard->level[frontY][frontX].isWalkable = false;
                    used = true;
                }
                break;
            }
            case AbilityType::SpeedBoost: {
                movement.speedModifier = 2.0f;
                ability.durationRemaining = ability.duration;
                used = true;
                break;
            }
            case AbilityType::Invisibility: {
                if (!isInvisible) {
                    isInvisible = true;
                    ability.durationRemaining = ability.duration;
                    used = true;
                }
                break;
            }
            default:
                break;
        }

        if(!used) continue;

        if(ability.isConsumable){
            ability.charges--;
            if(ability.charges <= 0){ 
                ability = blackboard->abilityFactory.CreateAbility(AbilityType::None);
            }
        }

        ability.cooldownRemaining = ability.cooldownDuration;
    }
}

void Player::UpdateAbilitiesCooldown(float deltaTime){
    for(auto &ability : abilities){
        if(ability.cooldownRemaining > 0.0f){
            ability.cooldownRemaining -= deltaTime;
            if(ability.cooldownRemaining < 0.0f){
                ability.cooldownRemaining = 0.0f;
            }
        }

        if(ability.durationRemaining > 0.0f){
            ability.durationRemaining -= deltaTime;
            if(ability.durationRemaining <= 0.0f){
                ability.durationRemaining = 0.0f;
                switch(ability.type){
                    case AbilityType::SpeedBoost:
                        movement.speedModifier = 1.0f;
                        break;
                    case AbilityType::Invisibility:
                        isInvisible = false;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

void Player::DrawEntity(SDL_Renderer *renderer, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize){
    this->render.rect.x = hightMargine + position.visualX * cellHight + 5;
    this->render.rect.y = widthMargine + position.visualY * cellWidth + 5;
    this->render.rect.w = squareSize - 10;
    this->render.rect.h = squareSize - 10;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &this->render.rect);
    SDL_RenderTextureRotated(renderer, render.texture.get(), NULL, &render.rect, (0.0 + ((position.direction == EntityDirection::Down) * 90.0) + ((position.direction == EntityDirection::Left) * 180.0) + ((position.direction == EntityDirection::Up) * 270.0)), NULL, SDL_FLIP_NONE);
}

Player::Player(int x, int y, Blackboard *bb, std::map<SDL_Scancode, bool> controlsMap):blackboard{bb}, controls{controlsMap} {
    position = {x, y, static_cast<float>(x), static_cast<float>(y), EntityDirection::Down};
    this->movement = {x, y, 1.0f, x, y, -1, -1, EntityDirection::Down, false};
    render = {{nullptr}, {0, 0, 0, 0}};

    abilities[AbilityID::permanentAbility1] = bb->abilityFactory.CreateAbility(AbilityType::Invisibility);
    abilities[AbilityID::permanentAbility2] = bb->abilityFactory.CreateAbility(AbilityType::Claymore);
    abilities[AbilityID::consumableAbility1] = bb->abilityFactory.CreateAbility(AbilityType::None);
    abilities[AbilityID::consumableAbility2] = bb->abilityFactory.CreateAbility(AbilityType::SpeedBoost);

}

void Player::SetBlackboard(Blackboard *bb){
    blackboard = bb;
}

Player::~Player(){}