#pragma once
#include "TileData.h"
#include "Entities/GameManager.h"
#include "./Entities/Player.h"
#include <SDL3/SDL.h>

bool IsWalkable(const TileData& tile);

void ApplyEffect(GameManager &gameManager, TileData& tile);

void ApplyEffect(Player* player, TileData& tile);

void DrawTile(SDL_Renderer* renderer, const TileData& tile, SDL_FRect* rect, int x, int y, int cellWidth, int cellHight, int widthMargine, int hightMargine, int squareSize);