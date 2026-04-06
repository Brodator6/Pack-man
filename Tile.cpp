#include "Tile.h"
#include <iostream>

void Tile::DrawTile(SDL_Renderer *renderer, SDL_FRect *rect, int x, int y,  int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize) {

}

void Tile::ApplyEffect(Player *player, std::unique_ptr<Tile> *currentTile){

}

bool Tile::GetIsWalkable() const{
    return this->isWalkable;
}

Tile::Tile(bool isWalkable){
    this->isWalkable = isWalkable;
}

Tile::~Tile(){

}



void Floor::DrawTile(SDL_Renderer *renderer, SDL_FRect *rect, int x, int y,  int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize){
    SDL_SetRenderDrawColor(renderer, 100, 255, 0, SDL_ALPHA_OPAQUE);
    rect->x = hightMargine + x * cellHight;
    rect->y = widthMargine + y * cellWidth;
    SDL_RenderFillRect(renderer, rect);
};

void Floor::ApplyEffect(Player *player, std::unique_ptr<Tile> *currentTile){

}

Floor::Floor():Tile(true){

}

Floor::~Floor(){

}



void Wall::DrawTile(SDL_Renderer *renderer, SDL_FRect *rect, int x, int y,  int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    rect->x = hightMargine + x * cellHight;
    rect->y = widthMargine + y * cellWidth;
    SDL_RenderFillRect(renderer, rect);
};

void Wall::ApplyEffect(Player *player, std::unique_ptr<Tile> *currentTile){

}

Wall::Wall():Tile(false){

}

Wall::~Wall(){

}

void SmallReward::DrawTile(SDL_Renderer *renderer, SDL_FRect *rect, int x, int y,  int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize){
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    rect->x = hightMargine + x * cellHight;
    rect->y = widthMargine + y * cellWidth;
    SDL_RenderFillRect(renderer, rect);
};

void SmallReward::ApplyEffect(Player *player, std::unique_ptr<Tile> *currentTile){
    player->AddScore(this->score);
    *currentTile = std::make_unique<Floor>();
    std::cout << player->GetScore() << std::endl;
}

SmallReward::SmallReward():Tile(true){

}

SmallReward::~SmallReward(){
}