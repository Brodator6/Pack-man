# Quick Start: Using Your ECS System

## The Complete Picture

```
┌─────────────────────────────────────────────────────────────┐
│                    GAME INITIALIZATION                      │
└─────────────────────────────────────────────────────────────┘
  GameManager::GameManager()
    └─ EntityManager::SetUp(blackboard)
         ├─ Create Player entity (ID 0)
         ├─ Create Enemy 1 via EntityFactory (ID 1)
         │   └─ RequestAddEnemy() extracts Actor data → Components
         ├─ Create Enemy 2 via EntityFactory (ID 2)
         │   └─ RequestAddEnemy() extracts Actor data → Components
         └─ UpdateShadowGrid() for spatial queries

┌─────────────────────────────────────────────────────────────┐
│                    GAME LOOP (per frame)                    │
└─────────────────────────────────────────────────────────────┘
  GameManager::HandleGameLogic()
    ├─ UpdateInput()
    ├─ UpdateState() (collision checks, ability updates)
    │
    ├─ entityManager.UpdateAI(blackboard)
    │   └─ AISystem::Update()
    │       └─ For each entity with AI:
    │           ├─ Entity 1: BehaviorTree.Tick() → decides "Chase at (5,6)"
    │           ├─ Entity 2: BehaviorTree.Tick() → decides "Patrol randomly"
    │           └─ Updates: movementComponents[id].goalX/goalY/isChasing
    │
    ├─ entityManager.UpdateMovement()
    │   └─ MovementSystem::Update()
    │       └─ For each entity with movement:
    │           ├─ Entity 1: Move smoothly from (4,5) toward (5,6)
    │           ├─ Entity 2: Continue patrol path
    │           └─ Updates: positionComponents[id].x/y/visualX/visualY
    │
    ├─ entityManager.UpdateState()
    │   └─ Handle entity removal (dead enemies, expired traps)
    │
    ├─ entityManager.UpdateShadowGrid()
    │   └─ Update collision detection grid
    │
    └─ Rendering happens in GameMenu::DrawWindow()
        └─ entityManager.UpdateRender(renderer, ...)
            └─ RenderSystem::Update()
                └─ For each entity with render:
                    ├─ Get position from positionComponents[id]
                    ├─ Get direction from movementComponents[id]
                    └─ Draw at screen position
```

## How to Find and Modify Entities

### Find all chasing enemies

```cpp
// In GameManager.cpp or anywhere with access to EntityManager
auto& moveComps = entityManager.GetMovementComponents();

for (auto& [entityID, move] : moveComps) {
    if (move.isChasing) {
        auto& pos = entityManager.GetPositionComponents()[entityID];
        std::cout << "Enemy " << entityID << " chasing at (" << pos.x << "," << pos.y << ")\n";
    }
}
```

### Stop an entity from chasing

```cpp
auto& moveComps = entityManager.GetMovementComponents();
moveComps[enemyID].isChasing = false;
moveComps[enemyID].LastSeenPlayerX = -1;
moveComps[enemyID].LastSeenPlayerY = -1;
```

### Apply a speed boost

```cpp
auto& moveComps = entityManager.GetMovementComponents();
moveComps[entityID].speedModifier = 2.0f;  // Double speed
```

### Check entity type

```cpp
auto& types = entityManager.GetTypeComponents();
if (types[entityID].enemyType == EnemyType::AdvancedEnemy) {
    std::cout << "This is an advanced enemy\n";
}
```

## Adding New Functionality: Example

### Add a "Sticker" trap that slows enemies

**Step 1:** Add component to Components.h
```cpp
struct StickerComponent {
    float slowFactor = 0.5f;  // Slows to 50% speed
    int duration = 50;         // Ticks
};
```

**Step 2:** Add system to Systems.h/cpp
```cpp
class StickerSystem {
public:
    void Update(map<int, StickerComponent>& stickers,
                map<int, MovementComponent>& movements) {
        for (auto& [id, sticker] : stickers) {
            movements[id].speedModifier *= sticker.slowFactor;
            sticker.duration--;
            if (sticker.duration <= 0) {
                stickers.erase(id);
            }
        }
    }
};
```

**Step 3:** Add to EntityManager
```cpp
// EntityManager.h private:
std::unordered_map<int, StickerComponent> stickerComponents;
StickerSystem stickerSystem;

// EntityManager.h public:
void UpdateStickers() {
    stickerSystem.Update(stickerComponents, movementComponents);
}
```

**Step 4:** Call in game loop
```cpp
// GameManager::HandleGameLogic()
entityManager.UpdateAI();
entityManager.UpdateStickers();  // NEW!
entityManager.UpdateMovement();
entityManager.UpdateRender();
```

**Step 5:** Create sticker trap
```cpp
// In EntityFactory or when placing trap
int stickerID = entityManager.CreateStaticEntity(...);
entityManager.GetStickerComponents()[stickerID] = {0.5f, 50};
```

## Different Rendering Per Entity Type

### Render enemies with red tint

```cpp
// In RenderSystem::Update(), add type-based coloring:
void RenderSystem::Update(...) {
    for (auto& [entityID, renderComp] : renderComponents) {
        // ... setup code ...
        
        auto& type = typeComponents[entityID];
        if (type.actorType == ActorType::DinamicActor && type.enemyType != EnemyType::BasicEnemy) {
            SDL_SetRenderDrawColor(renderer, 200, 0, 0, SDL_ALPHA_OPAQUE);  // Red for advanced
        } else if (type.actorType == ActorType::StaticActor) {
            SDL_SetRenderDrawColor(renderer, 0, 100, 200, SDL_ALPHA_OPAQUE);  // Blue for traps
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);  // Default
        }
        
        SDL_RenderFillRect(renderer, &renderComp.rect);
        // ... texture drawing ...
    }
}
```

## Performance Characteristics

### Memory
- Each component map stores only relevant data
- Entities don't exist without components
- Total memory = sum of used components per entity

### CPU Cache
- Systems iterate component arrays sequentially ✓
- CPU prefetches next data (cache hit ✓✓✓)
- Better than object-oriented (cache misses)

### Parallelization (potential future optimization)
```
// Could run independently in parallel:
UpdateAI()       (reads AI, writes Movement)
UpdatePowerUps() (reads PowerUp, writes Movement)
UpdateAI()       (reads AI, writes Movement)
// Then synchronize:
UpdateMovement() (reads Movement, writes Position)
UpdateRender()   (reads Position, writes to screen)
```

## Common Mistakes to Avoid

❌ **WRONG**: Modifying `enemies.push_back()` directly
```cpp
gameManager.entityManager.enemies.push_back(actor);  // ❌ enemies list doesn't exist!
```

✅ **RIGHT**: Use RequestAddEnemy()
```cpp
gameManager.entityManager.RequestAddEnemy(actor);  // ✓ Creates all components
```

---

❌ **WRONG**: Looking for entity in old vector
```cpp
auto enemy = gameManager.entityManager.enemies[5];  // ❌ No vector!
```

✅ **RIGHT**: Get components via maps
```cpp
auto& pos = gameManager.entityManager.GetPositionComponents()[5];
auto& move = gameManager.entityManager.GetMovementComponents()[5];
```

---

❌ **WRONG**: Forgetting systems run in order
```cpp
// AI runs first, decides to chase → movementComponent.goalX = 5
// Then Movement runs       → moves toward (5, y)
// Don't expect immediate position change in same frame!
```

✅ **RIGHT**: Understand frame pipelining
```cpp
// Frame N: AI decides
// Frame N+1: Movement executes decision
// Frame N+2: Rendering shows result
```

## Entity Lifecycle

```
1. CREATION
   ├─ EntityFactory.CreateEntity() builds Actor data
   └─ EntityManager.RequestAddEnemy() converts to components

2. UPDATE (each frame)
   ├─ AISystem reads state, writes decisions
   ├─ MovementSystem reads decisions, writes positions
   └─ RenderSystem reads positions, draws (no writing)

3. DESTRUCTION
   ├─ EntityManager.RequestRemoveEntityByID(id)
   └─ UpdateState() erases from all component maps
```

## Summary: Your ECS is Ready!

You now have:
✅ Pure data storage (components in maps)
✅ Logic systems (AI, Movement, Render)
✅ Entity factory (creates data from old Actor style)
✅ Setup system (initializes level)
✅ Parallel-friendly architecture
✅ Easy to extend (add components + systems)
