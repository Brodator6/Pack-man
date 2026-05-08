# ECS (Entity Component System) Design Guide for Your Pac-Man Game

## Core Concept

ECS is a **data-oriented architecture** opposite to traditional OOP. Instead of monolithic classes with mixed data and behavior, ECS separates:

### 1. **Entities** = Just IDs (integers)
```cpp
// In your system, entityID is from 0 to N
// Player = ID 0
// Enemies = ID 1, 2, 3, ...
```

### 2. **Components** = Pure Data (no methods)
```cpp
// Position component - just data
struct PositionComponent {
    int x, y;
    float visualX, visualY;  // For smooth animation
};

// Movement component - just data
struct MovementComponent {
    int targetX, targetY;
    Direction direction;
    float speedModifier;
    int goalX, goalY;
    bool isChasing;
};

// AI component - data + behavior tree reference
struct AIComponent {
    std::vector<Cell> currentPath;
    std::unique_ptr<Node> AI;  // Behavior tree
    EnemyType enemyType;
};
```

**Key insight**: No methods in components! They're just data containers.

### 3. **Systems** = Pure Logic
```cpp
// AISystem reads PositionComponent, MovementComponent, AIComponent and updates them
class AISystem {
    void Update(map<id, PositionComponent>& pos,
                map<id, MovementComponent>& move,
                map<id, AIComponent>& ai) {
        for (auto& [entityID, aiComp] : ai) {
            // Get other components for this entity
            auto& pos = positions[entityID];
            auto& move = movements[entityID];
            
            // Run AI logic (behavior tree)
            aiComp.AI->Tick(entityID, pos, move, aiComp, ...);
        }
    }
};
```

## How Your System Works (The Maps)

```
Your EntityManager stores component data in MAPS indexed by entity ID:

┌─ Entity ID 0 (Player) ──────────────────────────────────────────┐
│                                                                  │
│  positionComponents[0] = {x:8, y:8, visualX:8.0, visualY:8.0} │
│  movementComponents[0] = {direction:UP, speedModifier:1.0}     │
│  renderComponents[0] = {texture: playerTex, rect: ...}         │
│  typeComponents[0] = {actorType:DinamicActor, enemyType:None}  │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘

┌─ Entity ID 1 (Enemy 1) ──────────────────────────────────────────┐
│                                                                   │
│  positionComponents[1] = {x:2, y:2, visualX:2.0, visualY:2.0}  │
│  movementComponents[1] = {direction:DOWN, isChasing:false}      │
│  aiComponents[1] = {AI: BehaviorTree, currentPath: [...]}       │
│  renderComponents[1] = {texture: enemyTex, rect: ...}           │
│  typeComponents[1] = {actorType:DinamicActor, enemyType:Advanced}│
│  blackboardComponents[1] = {advancedBB: ptr, commandoBB: null}  │
│                                                                   │
└─────────────────────────────────────────────────────────────────┘
```

**Data-Oriented Design Advantage**: When iterating through components in a system,
all data is in contiguous memory, making CPU cache very happy! ✓

## Game Loop (How Systems Execute)

### Before (Object-Oriented):
```cpp
for(auto &actor : entityManager.enemies) {
    actor.AI->Tick(actor, blackboard);  // Tick AI
    actor.UpdateMovement();               // Update movement
    actor.Move();                         // Move
    actor.DrawEntity(renderer, ...);      // Draw (scattered calls)
}
```
Problem: Cache misses, mixed concerns, hard to parallelize

### Now (ECS):
```cpp
// Phase 1: AI Decision
entityManager.UpdateAI(blackboard);  
// All entities with AI components process in tight loop

// Phase 2: Movement
entityManager.UpdateMovement();  
// All entities with movement components move

// Phase 3: Rendering
entityManager.UpdateRender(renderer, ...);  
// All entities with render components draw
```
Benefit: Cache-friendly loops, systems don't interfere, easy to parallelize per phase

## How to Add New Functionality

### Scenario 1: Add a "PowerUp" that speeds up the player

**Step 1**: Create a new component
```cpp
// In Components.h
struct PowerUpComponent {
    PowerUpType type;
    float speedMultiplier = 1.5f;
    int duration = 100;  // ticks
};
```

**Step 2**: Create a system to process it
```cpp
// In Systems.h
class PowerUpSystem {
public:
    void Update(map<int, PowerUpComponent>& powerups,
                map<int, MovementComponent>& movements) {
        for (auto& [id, powerup] : powerups) {
            movements[id].speedModifier *= powerup.speedMultiplier;
            powerup.duration--;
            if (powerup.duration <= 0) {
                powerups.erase(id);  // Remove when expired
            }
        }
    }
};
```

**Step 3**: Add to EntityManager
```cpp
// In EntityManager.h
private:
    PowerUpSystem powerUpSystem;
    std::unordered_map<int, PowerUpComponent> powerUpComponents;

public:
    void UpdatePowerUps() {
        powerUpSystem.Update(powerUpComponents, movementComponents);
    }
```

**Step 4**: Call in game loop
```cpp
// In GameManager::HandleGameLogic()
entityManager.UpdateAI();
entityManager.UpdatePowerUps();  // NEW!
entityManager.UpdateMovement();
entityManager.UpdateRender();
```

### Scenario 2: Render different entity types differently

Currently you have one RenderSystem that draws all entities the same way.
To render enemies with a red tint and player normally:

**Option A: Multiple Specialized Systems** (Recommended)
```cpp
class PlayerRenderSystem {
public:
    void Render(unordered_map<int, PlayerComponent>& players,
                map<int, PositionComponent>& pos,
                map<int, RenderComponent>& renders,
                SDL_Renderer* renderer) {
        for (auto& [id, player] : players) {
            // Draw with special glow effect
            DrawWithGlow(renderer, pos[id], renders[id], YELLOW);
        }
    }
};

class EnemyRenderSystem {
public:
    void Render(map<int, TypeComponent>& types,
                map<int, PositionComponent>& pos,
                map<int, RenderComponent>& renders,
                SDL_Renderer* renderer) {
        for (auto& [id, type] : types) {
            if (type.actorType == ActorType::DinamicActor) {
                // Draw with red tint
                DrawWithTint(renderer, pos[id], renders[id], RED);
            }
        }
    }
};
```

Then call both:
```cpp
playerRenderSystem.Render(...);
enemyRenderSystem.Render(...);
```

**Option B: Use Entity Tags** (Simpler)
```cpp
struct TagComponent {
    enum { PLAYER = 1, ENEMY = 2, TRAP = 4 };
    int tags = 0;
};

// Create tagging system
if (entityID == 0) tagComponents[0] = {.tags = TagComponent::PLAYER};
if (entityID >= 1) tagComponents[entityID] = {.tags = TagComponent::ENEMY};

// In RenderSystem, check tags:
for (auto& [id, render] : renders) {
    auto tags = tagComponents[id].tags;
    if (tags & TagComponent::PLAYER) {
        DrawPlayerStyle(renderer, pos[id], render);
    } else if (tags & TagComponent::ENEMY) {
        DrawEnemyStyle(renderer, pos[id], render);
    }
}
```

## Why You Need EntityFactory Updates

EntityFactory still returns `Actor` objects, which is fine because:

1. EntityFactory builds the data
2. RequestAddEnemy() extracts that data into components
3. Components live in the maps

```cpp
// In EntityManager::RequestAddEnemy()
void EntityManager::RequestAddEnemy(Actor newEnemy){
    int entityID = kEnemyEntityIdOffset + positionComponents.size();
    
    // Extract Actor data → Components
    positionComponents[entityID] = {newEnemy.xPosition, newEnemy.yPosition, ...};
    movementComponents[entityID] = {newEnemy.targetX, newEnemy.targetY, ...};
    aiComponents[entityID] = {newEnemy.currentPath, std::move(newEnemy.AI), ...};
    renderComponents[entityID] = {newEnemy.texture, newEnemy.rect};
    
    // Initialize other components
    typeComponents[entityID] = {newEnemy.actorType, newEnemy.enemyType};
    if (newEnemy.actorType == ActorType::StaticActor) {
        staticComponents[entityID] = {newEnemy.StaticEntity.timer, ...};
    }
    
    // Blackboard setup
    if (newEnemy.enemyType == EnemyType::AdvancedEnemy) {
        blackboardComponents[entityID] = {&advancedEnemyBlackboard, nullptr};
        advancedEnemyBlackboard.GetOrCreateEnemyIndex(entityID);
    }
}
```

EntityFactory doesn't need changes - it constructs Actor objects
the old way, which provides data that gets converted to components.

## Why EntityManager::SetUp was Restored

**You were right!** SetUp needs to initialize the level with enemies/player:

```cpp
void EntityManager::SetUp(Blackboard &bb) {
    this->bb = &bb;
    
    // Create player entity
    player = bb.entityFactory.CreatePlayer(8, 8, Direction::Up, bb);
    
    // Create initial enemies (components created inside RequestAddEnemy)
    RequestAddEnemy(bb.entityFactory.CreateEntity(2, 2, Direction::Down, EntityType::AdvancedEnemy));
    RequestAddEnemy(bb.entityFactory.CreateEntity(9, 10, Direction::Down, EntityType::AdvancedEnemy));
    
    // Initialize shadow grid for spatial queries
    shadowGrid.resize(bb.rows * bb.columns);
    UpdateShadowGrid();
}
```

This is the **bridge** between level setup and the ECS component system!

## Component Queries (Finding Entities with Specific Data)

To find all enemies currently chasing:
```cpp
for (auto& [id, move] : movementComponents) {
    if (move.isChasing) {
        // This enemy is chasing!
        auto& pos = positionComponents[id];
        auto& type = typeComponents[id];
        std::cout << "Enemy " << id << " at (" << pos.x << "," << pos.y << ") chasing\n";
    }
}
```

To find all static entities:
```cpp
for (auto& [id, staticComp] : staticComponents) {
    // Do something with static entity
    auto& pos = positionComponents[id];
}
```

**Advantage**: You're iterating only the data you need, cache-friendly!

## Summary: The Bridge Architecture

```
EntityFactory          RequestAddEnemy()          Component Maps          Systems
   (builds              (extracts &               (store data)        (read & write)
   Actor data)         converts)
     │                     │                         │                    │
     ├─ CreateEntity() ─→ Actor ─→ RequestAddEnemy() ─→ positionComponents ──→ AISystem
     │                                              ├─ movementComponents ────→ MovementSystem
     │                                              ├─ renderComponents ──────→ RenderSystem
     │                                              ├─ aiComponents
     │                                              ├─ typeComponents
     │                                              └─ ...
     │
     └─ CreatePlayer() ─→ Player
```

Your ECS is complete! You have:
✓ Entities (IDs)
✓ Components (pure data maps)
✓ Systems (AI, Movement, Render)
✓ Entity Factory (creates data)
✓ Setup (initializes level)
