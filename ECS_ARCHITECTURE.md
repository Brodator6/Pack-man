# ECS Architecture Visualization

## System Architecture Diagram

```
┌────────────────────────────────────────────────────────────────────────────┐
│                         ENTITY COMPONENT SYSTEM                            │
├────────────────────────────────────────────────────────────────────────────┤
│
│  ENTITY STORAGE LAYER (EntityManager)
│  ╔══════════════════════════════════════════════════════════════════════╗
│  ║  Component Maps (indexed by entity ID)                              ║
│  ║  ┌────────────────────────────────────────────────────────────────┐ ║
│  ║  │ positionComponents[0] = {x:8, y:8, visualX:8.0, visualY:8.0}  │ ║
│  ║  │ positionComponents[1] = {x:2, y:2, visualX:2.0, visualY:2.0}  │ ║
│  ║  │ positionComponents[2] = {x:9, y:10, visualX:9.0, visualY:10}  │ ║
│  ║  └────────────────────────────────────────────────────────────────┘ ║
│  ║  ┌────────────────────────────────────────────────────────────────┐ ║
│  ║  │ movementComponents[0] = {direction:UP, speedModifier:1.0, ...} │ ║
│  ║  │ movementComponents[1] = {direction:DOWN, isChasing:false, ...} │ ║
│  ║  │ movementComponents[2] = {direction:DOWN, isChasing:false, ...} │ ║
│  ║  └────────────────────────────────────────────────────────────────┘ ║
│  ║  ┌────────────────────────────────────────────────────────────────┐ ║
│  ║  │ aiComponents[1] = {AI: BehaviorTree, currentPath: {...}}       │ ║
│  ║  │ aiComponents[2] = {AI: BehaviorTree, currentPath: {...}}       │ ║
│  ║  └────────────────────────────────────────────────────────────────┘ ║
│  ║  ┌────────────────────────────────────────────────────────────────┐ ║
│  ║  │ renderComponents[0] = {texture: playerTex, rect: {...}}        │ ║
│  ║  │ renderComponents[1] = {texture: enemyTex, rect: {...}}         │ ║
│  ║  │ renderComponents[2] = {texture: enemyTex, rect: {...}}         │ ║
│  ║  └────────────────────────────────────────────────────────────────┘ ║
│  ║  ┌────────────────────────────────────────────────────────────────┐ ║
│  ║  │ typeComponents[0] = {actorType:Dynamic, enemyType:None}        │ ║
│  ║  │ typeComponents[1] = {actorType:Dynamic, enemyType:Advanced}    │ ║
│  ║  │ typeComponents[2] = {actorType:Dynamic, enemyType:Advanced}    │ ║
│  ║  └────────────────────────────────────────────────────────────────┘ ║
│  ║  ┌────────────────────────────────────────────────────────────────┐ ║
│  ║  │ blackboardComponents[1] = {advancedBB: ptr, ...}               │ ║
│  ║  │ blackboardComponents[2] = {advancedBB: ptr, ...}               │ ║
│  ║  └────────────────────────────────────────────────────────────────┘ ║
│  ╚══════════════════════════════════════════════════════════════════════╝
│
│
│  SYSTEM EXECUTION LAYER
│  ╔══════════════════════════════════════════════════════════════════════╗
│  ║  AISystem::Update()                                                  ║
│  ║  ┌────────────────────────────────────────────────────────────────┐ ║
│  ║  │ For each (id, ai) in aiComponents:                             │ ║
│  ║  │   ai.AI->Tick(id, pos[id], move[id], ai, type[id], ...)       │ ║
│  ║  │   // Decision making: update goals, chasing state              │ ║
│  ║  └────────────────────────────────────────────────────────────────┘ ║
│  ║  Result: movementComponents updated with new decisions             ║
│  ║                                                                     ║
│  ║  MovementSystem::Update()                                           ║
│  ║  ┌────────────────────────────────────────────────────────────────┐ ║
│  ║  │ For each (id, move) in movementComponents:                     │ ║
│  ║  │   // Follow path from AI                                       │ ║
│  ║  │   Smoothly interpolate pos[id].visualX/Y toward target         │ ║
│  ║  │   Update pos[id].x/y when reached waypoint                     │ ║
│  ║  └────────────────────────────────────────────────────────────────┘ ║
│  ║  Result: positionComponents updated with actual movement           ║
│  ║                                                                     ║
│  ║  RenderSystem::Update()                                             ║
│  ║  ┌────────────────────────────────────────────────────────────────┐ ║
│  ║  │ For each (id, render) in renderComponents:                     │ ║
│  ║  │   SDL_RenderTextureRotated(                                    │ ║
│  ║  │       renderer,                                                │ ║
│  ║  │       render.texture,                                          │ ║
│  ║  │       screen_pos = (pos[id].visualX, pos[id].visualY),        │ ║
│  ║  │       rotation = direction[id]                                 │ ║
│  ║  │   )                                                             │ ║
│  ║  └────────────────────────────────────────────────────────────────┘ ║
│  ║  Result: All entities rendered to screen                           ║
│  ╚══════════════════════════════════════════════════════════════════════╝
│
└────────────────────────────────────────────────────────────────────────────┘
```

## Data Flow: Component Creation

```
                    EntityFactory
                          │
                 CreateEntity(x, y, type)
                          │
          ┌───────────────┼───────────────┐
          │               │               │
      Creates         Creates         Creates
      Position         Movement         AI
      Data             Data            Data
          │               │               │
    ┌─────────────────────────────────────────┐
    │     Returns: Actor object (temporary)   │
    │  - xPosition, yPosition, visualX/Y      │
    │  - targetX, targetY, direction          │
    │  - DinamicEntity.speedModifier, etc     │
    │  - currentPath                          │
    │  - AI (behavior tree)                   │
    │  - texture                              │
    └─────────────────────────────────────────┘
                          │
                    RequestAddEnemy()
                          │
          ┌───────────────┼───────────────┐
          │               │               │
    Decompose         Extract         Store in
    Actor data        Components      Component Maps
          │               │               │
        ┌─┴────┬──────┬───┴──┬─────┬───┬───┘
        │      │      │      │     │   │
        ▼      ▼      ▼      ▼     ▼   ▼
    Position Movement AI   Render Type Blackboard
    Component Component Component Component Component Component
        │      │      │      │     │   │
        └─────────────┼──────┴─────┴───┘
                      │
            Stored in EntityManager
            with key = entityID
```

## Game Loop Execution Order

```
Frame N begins
    ↓
┌─────────────────────────────┐
│ Input & Game State Update   │
│ - HandleInput()             │
│ - UpdateState() (collisions)│
└──────────┬──────────────────┘
           ↓
┌─────────────────────────────────────────┐
│ AISystem::Update()                      │
│ ─────────────────────────────────────   │
│ For each AI entity:                     │
│   Behavior tree makes decision          │
│   Updates goalX, goalY, isChasing       │
│                                         │
│ Input: AIComponent, PositionComponent   │
│ Output: MovementComponent updated       │
└──────────┬──────────────────────────────┘
           ↓
┌──────────────────────────────────────┐
│ MovementSystem::Update()             │
│ ──────────────────────────────────   │
│ For each moving entity:              │
│   Follow path from AI                │
│   Smooth visual interpolation        │
│   Snap to actual grid tile           │
│                                      │
│ Input: MovementComponent, AIComp     │
│ Output: PositionComponent updated    │
└──────────┬───────────────────────────┘
           ↓
┌──────────────────────────────────────┐
│ UpdateState()                        │
│ ──────────────────────────────────   │
│ - Process removals                   │
│ - Handle collisions                  │
│ - Apply status effects               │
│                                      │
│ Input: All components                │
│ Output: Some components removed      │
└──────────┬───────────────────────────┘
           ↓
┌──────────────────────────────────────┐
│ UpdateShadowGrid()                   │
│ ──────────────────────────────────   │
│ - Update collision grid              │
│ - Allow efficient spatial queries    │
└──────────┬───────────────────────────┘
           ↓
┌────────────────────────────────────────────┐
│ RenderSystem::Update()                     │
│ ────────────────────────────────────────   │
│ For each renderable entity:                │
│   Get visual position from PositionComp    │
│   Get rotation from MovementComponent      │
│   Get texture from RenderComponent         │
│   Call SDL_RenderTextureRotated()          │
│                                            │
│ Input: Pos, Move, Render components        │
│ Output: Pixels on screen                   │
└────────────┬─────────────────────────────┘
             ↓
       Frame N displayed
```

## Component Dependency Graph

```
PositionComponent
    │
    ├─ Used by: AISystem (reads), MovementSystem (writes), RenderSystem
    │
    └─ Dependencies: None (pure data)

MovementComponent  
    │
    ├─ Used by: AISystem (writes), MovementSystem (reads/writes), RenderSystem
    │
    └─ Dependencies: None (pure data)

AIComponent
    │
    ├─ Used by: AISystem (reads/writes)
    │
    └─ Dependencies: BehaviorTree (Node class)

RenderComponent
    │
    ├─ Used by: RenderSystem (reads)
    │
    └─ Dependencies: SDL3 (texture/rect)

TypeComponent
    │
    ├─ Used by: AISystem (reads for type-specific behavior)
    │
    └─ Dependencies: None (pure data)

BlackboardComponent
    │
    ├─ Used by: AISystem (reads for multi-agent comm)
    │
    └─ Dependencies: AdvancedEnemyBlackboard, CommandoEnemyBlackboard

StaticEntityComponent
    │
    ├─ Used by: AISystem (reads timers for static entities)
    │
    └─ Dependencies: None (pure data)
```

## Adding a New Game Mechanic Example: Shield Component

```
1. Define Component (Components.h)
   ┌─────────────────────────────────┐
   │ struct ShieldComponent {        │
   │    float durability = 100.0f;  │
   │    float regenRate = 5.0f;     │
   │ };                              │
   └─────────────────────────────────┘

2. Create System (Systems.h/.cpp)
   ┌─────────────────────────────────┐
   │ class ShieldSystem {            │
   │ public:                         │
   │   void Update(                  │
   │     map<int, Shield...> shields │
   │   ) {                           │
   │     for each shield {           │
   │       durability += regenRate   │
   │     }                           │
   │   }                             │
   │ };                              │
   └─────────────────────────────────┘

3. Add to EntityManager
   ┌──────────────────────────────────────────┐
   │ private:                                 │
   │   map<int, ShieldComponent> shields;     │
   │   ShieldSystem shieldSystem;             │
   │                                          │
   │ public:                                  │
   │   void UpdateShields() {                 │
   │     shieldSystem.Update(shields);        │
   │   }                                      │
   └──────────────────────────────────────────┘

4. Call in Game Loop
   ┌────────────────────────────────────┐
   │ void GameManager::HandleGameLogic()│
   │ {                                  │
   │   entityManager.UpdateAI();        │
   │   entityManager.UpdateShield();    │ ← NEW!
   │   entityManager.UpdateMovement();  │
   │   entityManager.UpdateRender();    │
   │ }                                  │
   └────────────────────────────────────┘

5. Attach Shield to Entity
   ┌─────────────────────────┐
   │ auto& shields =         │
   │   entityManager         │
   │   .GetShieldComponents()│
   │                         │
   │ shields[playerID] =     │
   │   {100.0f, 5.0f};       │
   └─────────────────────────┘

Result: Every frame, shields regenerate!
```

## When NOT to Use ECS

ECS is optimal for games with:
✓ Many entities (100+)
✓ Varied component combinations
✓ Performance-critical code
✓ Future scalability important

Don't use ECS if:
✗ Only 5-10 actors total
✗ All actors behave identically
✗ Simple game logic
✗ Prototyping speed > performance

For your Pac-Man, ECS is **perfect** because:
- Multiple enemy types (BasicEnemy, AdvancedEnemy, CommandoEnemy)
- Multiple entity types (Enemies, Traps, Player)
- Future expansion (PowerUps, more trap types)
- Performance matters (potentially many on-screen entities)
