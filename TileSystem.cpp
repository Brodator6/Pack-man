#include "TileSystem.h"
#include <iostream>

bool IsWalkable(const TileData& tile) {
    return tile.isWalkable;
}

void ApplyEffect(GameManager &gameManager, TileData& tile) {
    if (tile.type == SMALL_REWARD) {
        gameManager.entityManager.GetPlayer().AddScore(tile.score);
        gameManager.globalScore += tile.score;
        tile.type = FLOOR; // change to floor after collecting
        tile.isWalkable = true;
        tile.score = 0;
        std::cout << gameManager.entityManager.GetPlayer().GetScore() << std::endl;
    }
    // other effects can be added here
}

void ApplyEffect(Player* player, TileData& tile) {
    if (tile.type == SMALL_REWARD) {
        player->AddScore(tile.score);
        tile.type = FLOOR; // change to floor after collecting
        tile.isWalkable = true;
        tile.score = 0;
        std::cout << player->GetScore() << std::endl;
    }
    // other effects can be added here
}

void DrawTile(SDL_Renderer* renderer, const TileData& tile, SDL_FRect* rect, int x, int y, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize) {
    switch (tile.type) {
        case FLOOR:
            SDL_SetRenderDrawColor(renderer, 100, 255, 0, SDL_ALPHA_OPAQUE);
            break;
        case WALL:
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            break;
        case SMALL_REWARD:
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            break;
    }
    rect->x = hightMargine + x * cellHight;
    rect->y = widthMargine + y * cellWidth;
    SDL_RenderFillRect(renderer, rect);
}