#pragma once
#include <memory>
#include <vector>
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

enum Direction {
    Left = 0,
    Up = 1,
    Right = 2,
    Down = 3
};

// ============================================================================
// COMPONENTS - Pure Data Structures (ECS)
// ============================================================================
// Components have NO METHODS. They are just data containers.
// Systems will read/write these components to implement game logic.

/// **PositionComponent**
/// Stores where an entity is in the game world
/// - int x, y: Grid position
/// - float visualX, visualY: Smooth animation position (interpolated between grid tiles)
struct PositionComponent {
    int x, y;                    // Actual grid position
    float visualX, visualY;      // Visual position for smooth movement animation
};

/// **MovementComponent**
/// Stores movement-related data for dynamic entities
/// Used by MovementSystem to update entity positions

struct MovementComponent {
    int targetX, targetY;        // Current target tile to move toward
    Direction direction;         // Which way entity is facing
    float speedModifier;         // 1.0 = normal, 0.5 = half speed, 2.0 = double
    int goalX, goalY;            // Final destination (for pathfinding)
    int LastSeenPlayerX, LastSeenPlayerY;  // Last known player position
    bool isChasing;              // Is this enemy currently chasing player?
};

/// **AIComponent**
/// Stores AI behavior data for entities with decision-making
/// Used by AISystem to run behavior trees
struct AIComponent {
    std::vector<Cell> currentPath;  // Path from pathfinding algorithm
    std::unique_ptr<Node> AI;       // Behavior tree root node
    EnemyType enemyType;            // What type of enemy (affects behavior)
};

/// **RenderComponent**
/// Stores rendering data
/// Used by RenderSystem to draw entities to screen
struct RenderComponent {
    std::shared_ptr<SDL_Texture> texture;
    SDL_FRect rect;
    // Direction stored in MovementComponent (no duplication)
};

/// **BlackboardComponent**
/// Reference to shared data structures for multi-agent AI
/// Allows enemies to communicate target locations, etc.
struct BlackboardComponent {
    AdvancedEnemyBlackboard* advancedBB;    // Shared data for advanced enemies
    CommandoEnemyBlackboard* commandoBB;    // Shared data for commando enemies
};

/// **TypeComponent**
/// Metadata about entity type
/// Used to quickly check what kind of entity this is
struct TypeComponent {
    ActorType actorType;         // Dynamic or static?
    EnemyType enemyType;         // Which enemy type?
};

/// **StaticEntityComponent**
/// For static entities like traps (Claymore, WallCharge)
/// They don't move but have timers for effects
struct StaticEntityComponent {
    int timer;       // Time until effect triggers
    int cooldown;    // Cooldown before next use
};