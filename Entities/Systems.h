#pragma once
#include <unordered_map>
#include <vector>
#include "Components.h"
#include "../DataStructs.h"

// ============================================================================
// SYSTEMS - Game Logic Execution (ECS)
// ============================================================================
// Systems implement game logic by iterating through components and updating them.
// Each system is responsible for one aspect of the game (AI, Movement, Rendering).
//
// Key Principle: Systems operate on COMPONENTS, not Actors!
// Instead of: for each actor { actor.Update(); }
// We have:   for each (entity with AI components) { runAI(entity); }
//
// This makes the code:
// - Cache efficient (iterating component arrays)
// - Parallelizable (systems can run independently)
// - Flexible (easy to add/remove components)

// Forward declarations
class EntityManager;
struct Blackboard;

/// **AISystem**
/// Runs behavior trees for all entities with AI
/// Reads: AIComponent, PositionComponent, MovementComponent, TypeComponent
/// Writes: MovementComponent (goals, chasing state), currentPath
class AISystem {
public:
    // Update all entities with AI components
    // Runs their behavior trees (decision-making)
    void Update(std::unordered_map<int, AIComponent>& aiComponents,
                std::unordered_map<int, PositionComponent>& posComponents,
                std::unordered_map<int, MovementComponent>& moveComponents,
                std::unordered_map<int, TypeComponent>& typeComponents,
                std::unordered_map<int, StaticEntityComponent>& staticComponents,
                std::unordered_map<int, BlackboardComponent>& bbComponents,
                Blackboard& blackboard);
};

/// **MovementSystem**
/// Updates positions and handles pathfollowing
/// Reads: AIComponent (currentPath), PositionComponent
/// Writes: PositionComponent (visualX, visualY, x, y, direction)
class MovementSystem {
public:
    // Update all moving entities
    // Interpolates visual position and updates tile-based position
    void Update(std::unordered_map<int, PositionComponent>& posComponents,
                std::unordered_map<int, MovementComponent>& moveComponents,
                std::unordered_map<int, AIComponent>& aiComponents);

    bool static HasReachedNode(const PositionComponent& pos, MovementComponent& move);
private:
    void SetTargetDirection(PositionComponent& pos, MovementComponent& move, int targetX, int targetY);
    void Move(PositionComponent& pos, MovementComponent& move);
};

/// **RenderSystem**
/// Renders all entities to the screen
/// Reads: PositionComponent, RenderComponent
/// Writes: Nothing (only reads)
class RenderSystem {
public:
    // Draw all entities with render components
    void Update(std::unordered_map<int, PositionComponent>& posComponents,
                std::unordered_map<int, MovementComponent>& moveComponents,
                std::unordered_map<int, RenderComponent>& renderComponents,
                SDL_Renderer* renderer, int cellWidth, int cellHeight, int widthMargin, int heightMargin, int squareSize);
};