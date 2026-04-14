#include "./Entities/Player.h"
#include <vector>
#include <memory>
#include "TileSystem.h"
#include <iostream>

static void UpdateState(Player *player, std::vector<std::vector<TileData>> &level, int rows, int columns){
    //this IS hell

    if(player->controls[SDL_SCANCODE_DOWN] == true){
        player->SetTargetPosition(player->GetPositionX(),
        player->GetPositionY() + ((IsWalkable(level[(player->GetPositionY() + 1) % rows][player->GetPositionX()]))) - ((player->GetPositionY() >= rows - 1) * rows));
    }else if(player->controls[SDL_SCANCODE_RIGHT] == true){
        player->SetTargetPosition(player->GetPositionX() + ((IsWalkable(level[player->GetPositionY()][(player->GetPositionX() + 1) % columns])))  - ((player->GetPositionX() >= columns - 1) * columns),
        player->GetPositionY());
    }else if(player->controls[SDL_SCANCODE_UP] == true){
        player->SetTargetPosition(player->GetPositionX(),
        player->GetPositionY() - ((IsWalkable(level[(player->GetPositionY() - 1 + rows) % rows][player->GetPositionX()])))  + ((player->GetPositionY() <= 0) * rows));
    }else if(player->controls[SDL_SCANCODE_LEFT] == true){
        player->SetTargetPosition(player->GetPositionX() - ((IsWalkable(level[player->GetPositionY()][(player->GetPositionX() - 1 + columns) % columns])))  + ((player->GetPositionX() <= 0) * columns),
        player->GetPositionY());
    }

        
    ApplyEffect(player, level[player->GetPositionY()][player->GetPositionX()]);
}