#pragma once
#include "TileData.h"
#include <memory>
#include <string>

class TileDataFactory {
public:
    static TileData CreateTile(char type) {
        switch (type) {
            case 'W':
                return {WALL, false, 0};
            case 'F':
                return {FLOOR, true, 0};
            case 'S':
                return {SMALL_REWARD, true, 10};
            default:
                return {FLOOR, true, 0}; // default to floor
        }
    }
};