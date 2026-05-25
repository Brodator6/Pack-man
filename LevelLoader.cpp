#include "LevelLoader.h"
#include "Entities/GameManager.h"
#include "Entities/Player.h"
#include "Entities/EntityManager.h"
#include "Entities/EntityFactory.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <tuple>

static std::string ReadFileText(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return std::string();
    }
    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

static size_t SkipWhitespace(const std::string& text, size_t pos) {
    while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) {
        pos++;
    }
    return pos;
}

static bool ParseQuotedString(const std::string& text, size_t& pos, std::string& out) {
    pos = SkipWhitespace(text, pos);
    if (pos >= text.size() || text[pos] != '"') {
        return false;
    }
    pos++;
    out.clear();
    while (pos < text.size()) {
        char c = text[pos++];
        if (c == '"') {
            return true;
        }
        if (c == '\\' && pos < text.size()) {
            out.push_back(text[pos++]);
            continue;
        }
        out.push_back(c);
    }
    return false;
}

static bool ParseInteger(const std::string& text, size_t& pos, int& out) {
    pos = SkipWhitespace(text, pos);
    if (pos >= text.size()) {
        return false;
    }
    bool negative = false;
    if (text[pos] == '-') {
        negative = true;
        pos++;
    }
    if (pos >= text.size() || !std::isdigit(static_cast<unsigned char>(text[pos]))) {
        return false;
    }
    int value = 0;
    while (pos < text.size() && std::isdigit(static_cast<unsigned char>(text[pos]))) {
        value = value * 10 + (text[pos] - '0');
        pos++;
    }
    out = negative ? -value : value;
    return true;
}

static bool FindJsonKey(const std::string& text, const std::string& key, size_t start, size_t& keyPos) {
    std::string quotedKey = '"' + key + '"';
    size_t pos = text.find(quotedKey, start);
    if (pos == std::string::npos) {
        return false;
    }
    keyPos = pos;
    return true;
}

static bool ReadJsonStringField(const std::string& text, const std::string& key, std::string& out, size_t start = 0) {
    size_t pos;
    if (!FindJsonKey(text, key, start, pos)) {
        return false;
    }
    pos += key.size() + 2;
    pos = text.find(':', pos);
    if (pos == std::string::npos) {
        return false;
    }
    pos++;
    return ParseQuotedString(text, pos, out);
}

static bool ReadJsonIntField(const std::string& text, const std::string& key, int& out, size_t start = 0) {
    size_t pos;
    if (!FindJsonKey(text, key, start, pos)) {
        return false;
    }
    pos += key.size() + 2;
    pos = text.find(':', pos);
    if (pos == std::string::npos) {
        return false;
    }
    pos++;
    return ParseInteger(text, pos, out);
}

static bool ParseStringArray(const std::string& text, const std::string& key, std::vector<std::string>& out) {
    size_t pos;
    if (!FindJsonKey(text, key, 0, pos)) {
        return false;
    }
    pos += key.size() + 2;
    pos = text.find(':', pos);
    if (pos == std::string::npos) {
        return false;
    }
    pos++;
    pos = SkipWhitespace(text, pos);
    if (pos >= text.size() || text[pos] != '[') {
        return false;
    }
    pos++;
    out.clear();
    while (pos < text.size()) {
        pos = SkipWhitespace(text, pos);
        if (pos >= text.size()) {
            return false;
        }
        if (text[pos] == ']') {
            pos++;
            return true;
        }
        if (text[pos] == ',') {
            pos++;
            continue;
        }
        std::string value;
        if (!ParseQuotedString(text, pos, value)) {
            return false;
        }
        out.push_back(value);
    }
    return false;
}

static bool ParseObjectBounds(const std::string& text, const std::string& key, size_t& objectStart, size_t& objectEnd) {
    size_t pos;
    if (!FindJsonKey(text, key, 0, pos)) {
        return false;
    }
    pos += key.size() + 2;
    pos = text.find(':', pos);
    if (pos == std::string::npos) {
        return false;
    }
    pos = SkipWhitespace(text, pos + 1);
    if (pos >= text.size() || text[pos] != '{') {
        return false;
    }
    objectStart = pos;
    int depth = 1;
    pos++;
    while (pos < text.size() && depth > 0) {
        if (text[pos] == '{') {
            depth++;
        } else if (text[pos] == '}') {
            depth--;
        }
        pos++;
    }
    if (depth != 0) {
        return false;
    }
    objectEnd = pos;
    return true;
}

static bool ExtractNextObject(const std::string& text, size_t& pos, size_t& objectStart, size_t& objectEnd) {
    pos = SkipWhitespace(text, pos);
    if (pos >= text.size() || text[pos] != '{') {
        return false;
    }
    objectStart = pos;
    int depth = 1;
    pos++;
    while (pos < text.size() && depth > 0) {
        if (text[pos] == '{') {
            depth++;
        } else if (text[pos] == '}') {
            depth--;
        }
        pos++;
    }
    if (depth != 0) {
        return false;
    }
    objectEnd = pos;
    return true;
}

static bool ParseEnemyList(const std::string& text, std::vector<std::tuple<std::string,int,int>>& out) {
    size_t pos;
    if (!FindJsonKey(text, "enemies", 0, pos)) {
        return false;
    }
    pos += 9;
    pos = text.find(':', pos);
    if (pos == std::string::npos) {
        return false;
    }
    pos++;
    pos = SkipWhitespace(text, pos);
    if (pos >= text.size() || text[pos] != '[') {
        return false;
    }
    pos++;
    out.clear();
    while (pos < text.size()) {
        pos = SkipWhitespace(text, pos);
        if (pos >= text.size()) {
            return false;
        }
        if (text[pos] == ']') {
            pos++;
            return true;
        }
        if (text[pos] == ',') {
            pos++;
            continue;
        }
        size_t objStart = 0;
        size_t objEnd = 0;
        if (!ExtractNextObject(text, pos, objStart, objEnd)) {
            return false;
        }
        std::string enemyType;
        int x = 0;
        int y = 0;
        ReadJsonStringField(text, "type", enemyType, objStart);
        ReadJsonIntField(text, "x", x, objStart);
        ReadJsonIntField(text, "y", y, objStart);
        out.emplace_back(enemyType, x, y);
    }
    return false;
}

static EntityType ParseEnemyType(const std::string& typeName) {
    if (typeName == "BasicEnemy") {
        return EntityType::BasicEnemy;
    }
    if (typeName == "AdvancedEnemy") {
        return EntityType::AdvancedEnemy;
    }
    if (typeName == "CommandoEnemy") {
        return EntityType::CommandoEnemy;
    }
    if (typeName == "CommandoLeaderEnemy") {
        return EntityType::CommandoLeaderEnemy;
    }
    return EntityType::None;
}

std::vector<std::vector<TileData>> LevelLoader::LoadLevel(const std::string& filename, int rows, int columns) {
    std::vector<std::vector<TileData>> level;
    level.resize(rows);

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open level file: " << filename << std::endl;
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
        for (int col = line.size(); col < columns; ++col) {
            level[row][col] = TileDataFactory::CreateTile('_');
        }
        ++row;
    }
    for (int i = row; i < rows; ++i) {
        level[i].resize(columns);
        for (int j = 0; j < columns; ++j) {
            level[i][j] = TileDataFactory::CreateTile('_');
        }
    }
    return level;
}

bool LevelLoader::LoadLevel(const std::string& filename, GameManager& gameManager) {
    const std::string fileText = ReadFileText(filename);
    if (fileText.empty()) {
        std::cerr << "Failed to read level JSON file: " << filename << std::endl;
        return false;
    }

    std::string levelName;
    int scoreGoal = 20;
    int playerX = 0;
    int playerY = 0;
    std::vector<std::string> rows;
    std::vector<std::tuple<std::string,int,int>> enemies;

    ReadJsonStringField(fileText, "name", levelName);
    ReadJsonIntField(fileText, "scoreGoal", scoreGoal);

    size_t playerObjectStart = 0;
    size_t playerObjectEnd = 0;
    if (ParseObjectBounds(fileText, "player", playerObjectStart, playerObjectEnd)) {
        ReadJsonIntField(fileText, "x", playerX, playerObjectStart);
        ReadJsonIntField(fileText, "y", playerY, playerObjectStart);
    }

    ParseStringArray(fileText, "rows", rows);
    ParseEnemyList(fileText, enemies);

    int rowsCount = gameManager.blackboard.rows;
    int columnsCount = gameManager.blackboard.columns;
    gameManager.level.clear();
    gameManager.level.resize(rowsCount);
    for (int row = 0; row < rowsCount; ++row) {
        gameManager.level[row].resize(columnsCount);
        if (row < static_cast<int>(rows.size())) {
            const std::string& line = rows[row];
            for (int col = 0; col < columnsCount; ++col) {
                char tileChar = (col < static_cast<int>(line.size())) ? line[col] : '_';
                gameManager.level[row][col] = TileDataFactory::CreateTile(tileChar);
            }
        } else {
            for (int col = 0; col < columnsCount; ++col) {
                gameManager.level[row][col] = TileDataFactory::CreateTile('_');
            }
        }
    }

    if (!levelName.empty()) {
        gameManager.currentLevelName = levelName;
    }
    gameManager.scoreGoal = scoreGoal;
    gameManager.globalScore = 0;

    if (playerX < 0 || playerX >= columnsCount || playerY < 0 || playerY >= rowsCount) {
        playerX = 0;
        playerY = 0;
    }

    Player player = gameManager.entityFactory.CreatePlayer(playerX, playerY, EntityDirection::Up, gameManager.blackboard);
    player.SetPosition(playerX, playerY);
    player.positionComponent.visualX = static_cast<float>(playerX);
    player.positionComponent.visualY = static_cast<float>(playerY);
    player.movementComponent.targetX = playerX;
    player.movementComponent.targetY = playerY;
    gameManager.entityManager.SetPlayer(std::move(player));

    for (auto &enemyDefinition : enemies) {
        std::string typeName;
        int x = 0;
        int y = 0;
        std::tie(typeName, x, y) = enemyDefinition;
        EntityType type = ParseEnemyType(typeName);
        if (type != EntityType::None && x >= 0 && x < columnsCount && y >= 0 && y < rowsCount) {
            gameManager.entityManager.AddEntity(x, y, EntityDirection::Down, type);
        }
    }

    gameManager.entityManager.UpdateShadowGrid();
    return true;
}
