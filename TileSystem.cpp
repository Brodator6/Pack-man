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

void DrawTile(SDL_Renderer* renderer, const TileData& tile, SDL_FRect* rect, int x, int y, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize) {
    switch (tile.type) {
        case FLOOR:
            SDL_SetRenderDrawColor(renderer, 120, 120, 120, SDL_ALPHA_OPAQUE);
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

void DrawTile(SDL_Renderer* renderer, Blackboard &blackboard, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize){
    SDL_FRect rect;
    rect = {0, 0, (float)cellWidth, (float)cellHight};
    for (int i = 0; i < blackboard.rows; i++){// loop for displaying current tile
        for(int j = 0; j < blackboard.columns; j++){
            rect.x = hightMargine + j * cellWidth;
            rect.y = widthMargine + i * cellHight;
            rect.w = cellWidth;
            rect.h = cellHight;
            switch (blackboard.level[i][j].type) {
            case FLOOR:
                SDL_SetRenderDrawColor(renderer, 120, 120, 120, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer, &rect);
                break;
            case WALL:
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer, &rect);
                break;
            case SMALL_REWARD:
                SDL_SetRenderDrawColor(renderer, 120, 120, 120, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer, &rect);

                rect.w = cellWidth / 4;
                rect.h = cellHight / 4;
                rect.x += (cellWidth - rect.w) / 2;
                rect.y += (cellHight - rect.h) / 2;
                
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer, &rect);
                break;
            }
        }
    }
}