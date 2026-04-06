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

Entity::Entity(int x, int y):xPosition{x}, yPosition{y}{

}