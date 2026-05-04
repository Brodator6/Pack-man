#include "Entity.h"

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

void Entity::SetTexture(const std::shared_ptr<SDL_Texture> &tex){
    texture = tex;
};

Entity::Entity(int x, int y):xPosition{x}, yPosition{y}{

}

Entity::Entity(int x, int y, std::shared_ptr<SDL_Texture> tex):xPosition{x}, yPosition{y}, texture{std::move(tex)}{

}