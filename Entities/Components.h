#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include "../TileData.h"
#include "../DataStructs.h"
#include "Pathfinding/Pathfinding.h"

// Forward declarations
class Node;
struct AdvancedEnemyBlackboard;
struct CommandoEnemyBlackboard;

// ============================================================================
// ENUMS FOR ENTITY TYPES AND BEHAVIOR
// ============================================================================

enum class EnemyType {
    BasicEnemy,
    AdvancedEnemy,
    CommandoEnemy,
    None
};

enum class ActorType {
    DinamicActor,
    StaticActor,
    None
};

enum class EntityType {
    BasicEnemy,
    AdvancedEnemy,
    CommandoEnemy,
    CommandoLeaderEnemy,
    Mine,
    WallCharge,
    None
};

enum EntityDirection {
    Left = 0,
    Up = 1,
    Right = 2,
    Down = 3
};

//needs reworking
// Structure to hold shared target information between AI agents of the same type
struct SharedTargetInfo {
    int lastSeenX = -1;
    int lastSeenY = -1;
    int lastSeenDirection = EntityDirection::Down;
    int reporterID = -1;  // Which enemy reported this position
    bool isValidInfo = false; // How recent is this information
};

// Base blackboard for enemies with cooperative behavior
struct AdvancedEnemyBlackboard {
    // List of IDs belonging to members that can use shared blackboard
    std::vector<int> MemberIDs;
    
    // Shared target information accessible to all advanced enemies
    SharedTargetInfo sharedTarget;
    
    // Find or create entry for an enemy
    int GetOrCreateEnemyIndex(int enemyID) {
        for (size_t i = 0; i < MemberIDs.size(); ++i) {
            if (MemberIDs[i] == enemyID) return i;
        }
        MemberIDs.push_back(enemyID);
        return MemberIDs.size() - 1;
    }
    
    // Update shared target information
    void ShareTargetInformation(int enemyID, int playerX, int playerY, EntityDirection direction, bool dataValidity) {
        sharedTarget.lastSeenX = playerX;
        sharedTarget.lastSeenY = playerY;
        sharedTarget.lastSeenDirection = direction;
        sharedTarget.reporterID = enemyID;
        sharedTarget.isValidInfo = dataValidity;
    }
    
    // Get the most recent target information from all allies
    SharedTargetInfo GetLatestSharedTarget() {
        return sharedTarget;
    }
};

// Extended blackboard for Commando enemies (future extension)
// Will have a commander that collects and coordinates information
struct CommandoOrder {
    bool isAssigned = false;
    int targetX = -1;
    int targetY = -1;
    bool holdPosition = false;
    bool chaseReporter = false;
};

struct CommandoEnemyBlackboard : public AdvancedEnemyBlackboard{
    // Commander ID (-1 if not assigned)
    int commanderID = -1;
    int commandTickCount = 0;

    // Orders assigned by leader to each commando member
    std::unordered_map<int, CommandoOrder> orders;
    
    // Commander election based on ticks
    void UpdateCommanderElection(int tickCount) {
        commandTickCount = tickCount;
    }

    void AssignOrder(int enemyID, const CommandoOrder& order) {
        orders[enemyID] = order;
    }

    CommandoOrder GetOrder(int enemyID) const {
        auto it = orders.find(enemyID);
        if (it != orders.end()) {
            return it->second;
        }
        return CommandoOrder();
    }

    void ClearOrders() {
        orders.clear();
    }
};



/// **PositionComponent**
/// Stores where an entity is in the game world
/// - int x, y: Grid position
/// - float visualX, visualY: Smooth animation position (interpolated between grid tiles)
struct PositionComponent {
    int x, y;                    // Actual grid position
    float visualX, visualY;      // Visual position for smooth movement animation
    EntityDirection direction;         // Which way entity is facing
};

/// **MovementComponent**
/// Stores movement-related data for dynamic entities
/// Used by MovementSystem to update entity positions

struct MovementComponent {
    int targetX, targetY;        // Current target tile to move toward
    float speedModifier;         // 1.0 = normal, 0.5 = half speed, 2.0 = double
    int goalX, goalY;            // Final destination (for pathfinding)
    int LastSeenPlayerX, LastSeenPlayerY;  // Last known player position
    EntityDirection lastSeenDirection;
    bool isChasing;              // Is this enemy currently chasing player?
};

/// **AIComponent**
/// Stores AI behavior data for entities with decision-making
/// Used by AISystem to run behavior trees
struct AIComponent {
    std::vector<Cell> currentPath;  // Path from pathfinding algorithm
    std::unique_ptr<Node> AI;       // Behavior tree root node
    EnemyType enemyType;            // What type of enemy (affects behavior)
    int tickCounter = 0;
};

/// **RenderComponent**
/// Stores rendering data
/// Used by RenderSystem to draw entities to screen
struct RenderComponent {
    std::shared_ptr<SDL_Texture> texture;
    SDL_FRect rect;
    // EntityDirection stored in MovementComponent (no duplication)
};

/// **BlackboardComponent**
/// Reference to shared data structures for multi-agent AI
/// Allows enemies to communicate target locations, etc.
struct BlackboardComponent {
    AdvancedEnemyBlackboard* sharedBB = nullptr;    // Shared data for advanced enemies or commando enemies
};


/// **TypeComponent**
/// Metadata about entity type
/// Used to quickly check what kind of entity this is
struct TypeComponent {
    ActorType actorType;         // Dynamic or static?
    EnemyType enemyType;         // Which enemy type?
};

/// **StaticEntityComponent**
/// For static entities like traps (Mine, WallCharge)
/// They don't move but have timers for effects
struct StaticEntityComponent {
    int timer;       // Time until effect triggers
    int cooldown;    // Cooldown before next use
};

struct EnemyBlackboard {
    int entityID = -1;
    PositionComponent* position = nullptr;
    MovementComponent* movement = nullptr;
    AIComponent* AI = nullptr;
    TypeComponent* type = nullptr;
    StaticEntityComponent* staticComp = nullptr;
    BlackboardComponent* blackboardComponent = nullptr;
};