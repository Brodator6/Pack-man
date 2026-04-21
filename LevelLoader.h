#pragma once
#include "TileDataFactory.h"
#include <vector>
#include <memory>
#include <fstream>
#include <string>
#include <iostream>

class LevelLoader {
public:
    static std::vector<std::vector<TileData>> LoadLevel(const std::string& filename, int rows, int columns) {
        std::vector<std::vector<TileData>> level;
        level.resize(rows);

        std::ifstream file(filename);
        std::cout << file.is_open() << std::endl;
        if (!file.is_open()) {
            std::cerr << "Failed to open level file: " << filename << std::endl;
            // Return empty level or handle error
            for (int i = 0; i < rows; ++i) {
                level[i].resize(columns);
                for (int j = 0; j < columns; ++j) {
                    level[i][j] = TileDataFactory::CreateTile('_');
                }
            }
            return level;
        }

        std::string line;
        int row = 0;
        while (std::getline(file, line) && row < rows) {
            level[row].resize(columns);
            for (int col = 0; col < columns && col < line.size(); ++col) {
                char type = line[col];
                level[row][col] = TileDataFactory::CreateTile(type);
            }
            // Fill remaining columns with Floor if line is short
            for (int col = line.size(); col < columns; ++col) {
                level[row][col] = TileDataFactory::CreateTile('_');
            }
            ++row;
        }

        // Fill remaining rows with Floor if file has fewer rows
        for (int i = row; i < rows; ++i) {
            level[i].resize(columns);
            for (int j = 0; j < columns; ++j) {
                level[i][j] = TileDataFactory::CreateTile('_');
            }
        }

        return level;
    }
};