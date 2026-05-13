#include "Player.h"
#include "../TileSystem.h"
#include "EntityFactory.h"
#include <cmath>

//functions
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
            this->direction = EntityDirection::Down;
            this->SetTargetPosition(this->GetPositionX(),
            this->GetPositionY() + ((IsWalkable(level[(this->GetPositionY() + 1) % rows][this->GetPositionX()]))) - ((this->GetPositionY() >= rows - 1) * rows));
        }else if(this->controls[SDL_SCANCODE_RIGHT] == true){
            this->direction = EntityDirection::Right;
            this->SetTargetPosition(this->GetPositionX() + ((IsWalkable(level[this->GetPositionY()][(this->GetPositionX() + 1) % columns])))  - ((this->GetPositionX() >= columns - 1) * columns),
            this->GetPositionY());
        }else if(this->controls[SDL_SCANCODE_UP] == true){
            this->direction = EntityDirection::Up;
            this->SetTargetPosition(this->GetPositionX(),
            this->GetPositionY() - ((IsWalkable(level[(this->GetPositionY() - 1 + rows) % rows][this->GetPositionX()])))  + ((this->GetPositionY() <= 0) * rows));
        }else if(this->controls[SDL_SCANCODE_LEFT] == true){
            this->direction = EntityDirection::Left;
            this->SetTargetPosition(this->GetPositionX() - ((IsWalkable(level[this->GetPositionY()][(this->GetPositionX() - 1 + columns) % columns])))  + ((this->GetPositionX() <= 0) * columns),
            this->GetPositionY());
        }
    }
}

void Player::Move(){
    int originalX = xPosition;
    int originalY = yPosition;

    int dx = targetX - xPosition;
    int dy = targetY - yPosition;

    float step = 0.2f * speedModifier;
    float dirX = dx == 0 ? 0.0f : (dx > 0 ? 1.0f : -1.0f);
    float dirY = dy == 0 ? 0.0f : (dy > 0 ? 1.0f : -1.0f);

    float moveX = dirX * step;
    float moveY = dirY * step;
    if (dx != 0 && std::abs(moveX) > std::abs(targetX - visualX)) {
        moveX = targetX - visualX;
    }
    if (dy != 0 && std::abs(moveY) > std::abs(targetY - visualY)) {
        moveY = targetY - visualY;
    }

    visualX += moveX;
    visualY += moveY;

    if (dx == 0) visualX = float(xPosition);
    if (dy == 0) visualY = float(yPosition);

    if (dx != 0 && std::abs(visualX - targetX) < 0.001f) {
        visualX = float(targetX);
        xPosition = targetX;
    }
    if (dy != 0 && std::abs(visualY - targetY) < 0.001f) {
        visualY = float(targetY);
        yPosition = targetY;
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

        int dirX = (direction == EntityDirection::Right) - (direction == EntityDirection::Left);
        int dirY = (direction == EntityDirection::Down) - (direction == EntityDirection::Up);
        int frontX = this->GetPositionX() + dirX;
        int frontY = this->GetPositionY() + dirY;
        bool used = false;

        switch (ability.type) {
            case AbilityType::Claymore: {
                if (frontX >= 0 && frontX < blackboard->columns && frontY >= 0 && frontY < blackboard->rows && blackboard->level[frontY][frontX].isWalkable) {
                    blackboard->entityManager.AddEntity(frontX, frontY, direction, EntityType::Claymore);
                    used = true;
                }
                break;
            }
            case AbilityType::WallCharge: {
                if(frontX >= 0 && frontX < blackboard->columns && frontY >= 0 && frontY < blackboard->rows && !blackboard->level[frontY][frontX].isWalkable){
                    blackboard->entityManager.AddEntity(frontX, frontY, direction, EntityType::WallCharge);
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
                speedModifier = 2.0f;
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
                        speedModifier = 1.0f;
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
    this->rect.x = hightMargine + visualX * cellHight + 5;
    this->rect.y = widthMargine + visualY * cellWidth + 5;
    this->rect.w = squareSize - 10;
    this->rect.h = squareSize - 10;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &this->rect);
    SDL_RenderTextureRotated(renderer, texture.get(), NULL, &rect, (0.0 + ((direction == EntityDirection::Down) * 90.0) + ((direction == EntityDirection::Left) * 180.0) + ((direction == EntityDirection::Up) * 270.0)), NULL, SDL_FLIP_NONE);
}

Player::Player(int x, int y, Blackboard *bb, std::map<SDL_Scancode, bool> movement):Entity{x, y}, blackboard{bb}, controls{movement} {
    actorType = ActorType::DinamicActor;
    targetX = x;
    targetY = y;
    visualX = x;
    visualY = y;

    abilities[AbilityID::permanentAbility1] = bb->abilityFactory.CreateAbility(AbilityType::Invisibility);
    abilities[AbilityID::permanentAbility2] = bb->abilityFactory.CreateAbility(AbilityType::Claymore);
    abilities[AbilityID::consumableAbility1] = bb->abilityFactory.CreateAbility(AbilityType::None);
    abilities[AbilityID::consumableAbility2] = bb->abilityFactory.CreateAbility(AbilityType::SpeedBoost);

}

Player::~Player(){}