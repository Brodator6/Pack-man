#pragma once
#include "TileDataFactory.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

class GameManager;

class LevelLoader {
public:
    static bool LoadLevel(const std::string& filename, GameManager& gameManager);
    static std::vector<std::vector<TileData>> LoadLevel(const std::string& filename, int rows, int columns);
};
