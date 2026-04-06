#include "./Entities/Player.h"
#include <vector>
#include <memory>
#include "Tile.h"
#include <iostream>

static void UpdateState(Player *player, std::vector<std::vector<std::unique_ptr<Tile>>> &level, int rows, int columns){
    //this IS hell

    if(player->movement[SDL_SCANCODE_DOWN] == true){
        player->SetPosition(player->GetPositionX(),
        player->GetPositionY() + ((level[(player->GetPositionY() + 1) % rows][player->GetPositionX()]->GetIsWalkable())) - ((player->GetPositionY() >= rows - 1) * rows));
    }else if(player->movement[SDL_SCANCODE_RIGHT] == true){
        player->SetPosition(player->GetPositionX() + ((level[player->GetPositionY()][(player->GetPositionX() + 1) % columns]->GetIsWalkable()))  - ((player->GetPositionX() >= columns - 1) * columns),
        player->GetPositionY());
    }else if(player->movement[SDL_SCANCODE_UP] == true){
        player->SetPosition(player->GetPositionX(),
        player->GetPositionY() - ((level[(player->GetPositionY() - 1 + rows) % rows][player->GetPositionX()]->GetIsWalkable()))  + ((player->GetPositionY() <= 0) * rows));
    }else if(player->movement[SDL_SCANCODE_LEFT] == true){
        player->SetPosition(player->GetPositionX() - ((level[player->GetPositionY()][(player->GetPositionX() - 1 + columns) % columns]->GetIsWalkable()))  + ((player->GetPositionX() <= 0) * columns),
        player->GetPositionY());
    }

        
    level[player->GetPositionY()][player->GetPositionX()]->ApplyEffect(player, &level[player->GetPositionY()][player->GetPositionX()]);
}