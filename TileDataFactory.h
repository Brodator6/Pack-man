#pragma once
#include "TileData.h"
#include <memory>
#include <string>

class TileDataFactory {
public:
    static TileData CreateTile(char type) {
        switch (type) {
            case '#':
                return {WALL, false, 0};
            case '_':
                return {FLOOR, true, 0};
            case '*':
                return {SMALL_REWARD, true, 20};
            default:
                return {FLOOR, true, 0}; // default to floor
        }
    }
};