#pragma once

enum TileType {
    FLOOR,
    WALL,
    SMALL_REWARD
};

struct TileData {
    TileType type;
    bool isWalkable;
    int score; // for rewards
};