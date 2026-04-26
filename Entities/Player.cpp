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
    int originalX = xPosition;
    int originalY = yPosition;

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

    if (ID == -1 && (xPosition != originalX || yPosition != originalY)) {
        ID = 0;
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

        int dirX = (direction == Direction::Right) - (direction == Direction::Left);
        int dirY = (direction == Direction::Down) - (direction == Direction::Up);
        int frontX = this->GetPositionX() + dirX;
        int frontY = this->GetPositionY() + dirY;
        bool used = false;

        switch (ability.type) {
            case AbilityType::Claymore: {
                Actor claymore = blackboard->entityFactory.CreateStaticEntity(this->GetPositionX() + dirX, this->GetPositionY() + dirY, direction, EntityType::Claymore);
                blackboard->entityManager.RequestAddEnemy(std::move(claymore));
                used = true;
                break;
            }
            case AbilityType::WallCharge: {
                if(frontX >= 0 && frontX < blackboard->columns && frontY >= 0 && frontY < blackboard->rows && !blackboard->level[frontY][frontX].isWalkable){
                    Actor charge = blackboard->entityFactory.CreateStaticEntity(frontX, frontY, direction, EntityType::WallCharge);
                    blackboard->entityManager.RequestAddEnemy(std::move(charge));
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
                if(this->ID != -1){
                    this->ID = -1;
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
                ability.type = AbilityType::None;
                ability.cooldownDuration = 0;
                ability.cooldownRemaining = 0;
                ability.isConsumable = false;
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
                        if(ID == -1){
                            ID = 0;
                        }
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
    SDL_RenderTextureRotated(renderer, texture, NULL, &rect, (0.0 + ((direction == Direction::Down) * 90.0) + ((direction == Direction::Left) * 180.0) + ((direction == Direction::Up) * 270.0)), NULL, SDL_FLIP_NONE);
}

Player::Player(int x, int y, Blackboard *bb, std::map<SDL_Scancode, bool> movement):Entity{x, y}, blackboard{bb}, controls{movement} {
    actorType = ActorType::DinamicActor;
    targetX = x;
    targetY = y;
    visualX = x;
    visualY = y;

    abilities[AbilityID::permanentAbility1] = {AbilityType::SpeedBoost, 0.0f, 10.0f, 0.0f, 3.0f, 0, false};
    abilities[AbilityID::permanentAbility2] = {AbilityType::Invisibility, 0.0f, 15.0f, 0.0f, 5.0f, 0, false};
    abilities[AbilityID::consumableAbility1] = {AbilityType::Claymore, 0.0f, 4.0f, 0.0f, 0.0f, 3, true};
    abilities[AbilityID::consumableAbility2] = {AbilityType::WallCharge, 0.0f, 6.0f, 0.0f, 0.0f, 2, true};

}

Player::~Player(){}