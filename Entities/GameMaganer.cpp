#include "GameManager.h"

#include "../UpdateInput.cpp"
#include "AI/BehaviorTree.h"
#include "../LevelLoader.h"
#include "../TileSystem.h"
#include "Player.h"
#include <filesystem>

static std::string GetNextLevelPath(const std::string &currentPath) {//needs reviewing
    std::string fileName = currentPath;
    auto pos = fileName.find_last_of("/\\");
    if (pos != std::string::npos) {
        fileName = fileName.substr(pos + 1);
    }

    if (fileName.rfind("level", 0) != 0) {
        return "./Assets/Levels/level1.json";
    }

    size_t digitPos = 5;
    int number = 0;
    while (digitPos < fileName.size() && isdigit(static_cast<unsigned char>(fileName[digitPos]))) {
        number = number * 10 + (fileName[digitPos] - '0');
        digitPos++;
    }

    if (number == 0) {
        return "./Assets/Levels/level1.json";
    }

    std::string directory = "./Assets/Levels/";
    std::string nextPath;
    for (int nextLevel = number + 1; nextLevel <= number + 10; ++nextLevel) {
        nextPath = directory + "level" + std::to_string(nextLevel) + ".json";
        if (std::filesystem::exists(nextPath)) {
            return nextPath;
        }
    }

    return directory + "level1.json";
}

void GameManager::GenerateLevel(const std::string &levelPath){
    currentLevelPath = levelPath;
    if (!LevelLoader::LoadLevel(levelPath, *this)) {
        std::cerr << "Failed to load level: " << levelPath << std::endl;
    }
}

void GameManager::HandleGameLogic(){
    UpdateInput(entityManager.GetPlayer().controls);
    entityManager.GetPlayer().UpdateMovement(level, blackboard.rows, blackboard.columns);
    entityManager.GetPlayer().UseAbility();
    entityManager.GetPlayer().UpdateAbilitiesCooldown(timeBlackboard.deltaTime.count());

    UpdateGameState();

    entityManager.GetPlayer().Move();

    entityManager.UpdateAI(blackboard);
    entityManager.UpdateMovement();
    
    entityManager.UpdateState();
    entityManager.UpdateShadowGrid();
    //entityManager.UpdatePathfindingGrid();
};

void GameManager::UpdateGameState(){
    //this IS hell

    ApplyEffect(*this, level[entityManager.GetPlayer().GetPositionY()][entityManager.GetPlayer().GetPositionX()]);

    Player &player = entityManager.GetPlayer();//gotta review
    float playerLeft = player.position.visualX;
    float playerTop = player.position.visualY;
    float playerRight = player.position.visualX + 1.0f;
    float playerBottom = player.position.visualY + 1.0f;

    const int centerX = player.GetPositionX();
    const int centerY = player.GetPositionY();
    const int neighborOffsets[5][2] = {
        {0, 0},
        {0, -1},
        {0, 1},
        {-1, 0},
        {1, 0}
    };

    for (auto &offset : neighborOffsets) {
        int checkX = centerX + offset[0];
        int checkY = centerY + offset[1];
        if (checkX < 0 || checkX >= blackboard.columns || checkY < 0 || checkY >= blackboard.rows) {
            continue;
        }

        const GridCell *cell = entityManager.GetGridCell(checkX, checkY);

        for (auto id : cell->entityIDs) {
            if (id <= 0) {
                continue;
            }

            auto it = entityManager.GetPositionComponents().find(id);
            if (it == entityManager.GetPositionComponents().end()) {
                continue;
            }

            const PositionComponent &enemyPos = it->second;
            float enemyLeft = enemyPos.visualX;
            float enemyTop = enemyPos.visualY;
            float enemyRight = enemyPos.visualX + 1.0f;
            float enemyBottom = enemyPos.visualY + 1.0f;

            if (playerLeft < enemyRight && playerRight > enemyLeft && playerTop < enemyBottom && playerBottom > enemyTop) {
                std::cout << "lost" << std::endl;
                lost = true;
                return;
            }
        }
    }
}

GameManager::GameManager(SDL_Renderer *renderer, TimeBlackboard &tBB) : timeBlackboard{tBB}, abilityFactory(renderer){
    entityFactory = EntityFactory(renderer, &blackboard);
    entityManager.SetUp(blackboard);
    currentLevelPath = "./Assets/Levels/level1.json";
    globalScore = 0;
    GenerateLevel(currentLevelPath);
};

GameManager::GameManager(SDL_Renderer *renderer, TimeBlackboard &tBB, const GameManager &previousManager) : timeBlackboard{tBB}, abilityFactory(renderer){
    entityFactory = EntityFactory(renderer, &blackboard);
    entityManager.SetUp(blackboard);
    currentLevelPath = GetNextLevelPath(previousManager.currentLevelPath);
    globalScore = 0;
    GenerateLevel(currentLevelPath);

    Player nextPlayer = previousManager.entityManager.GetPlayer();
    nextPlayer.SetBlackboard(&blackboard);
    nextPlayer.score = 0;
    nextPlayer.movement.targetX = entityManager.GetPlayer().GetPositionX();
    nextPlayer.movement.targetY = entityManager.GetPlayer().GetPositionY();
    nextPlayer.position.visualX = static_cast<float>(entityManager.GetPlayer().GetPositionX());
    nextPlayer.position.visualY = static_cast<float>(entityManager.GetPlayer().GetPositionY());
    nextPlayer.SetPosition(entityManager.GetPlayer().GetPositionX(), entityManager.GetPlayer().GetPositionY());
    nextPlayer.position.direction = entityManager.GetPlayer().position.direction;
    entityManager.SetPlayer(std::move(nextPlayer));
    entityManager.UpdateShadowGrid();
};