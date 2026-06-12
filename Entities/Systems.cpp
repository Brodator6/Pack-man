#include "Systems.h"
#include "AI/BehaviorTree.h"
#include <iostream>
#include <cmath>

// ============================================================================
// AISystem - Runs behavior trees for all entities with AI
// ============================================================================
// This system processes decision-making for all entities.
// Each behavior tree reads current state and writes decisions
// (target positions, whether to chase, etc.)

void AISystem::Update(std::unordered_map<int, AIComponent>& aiComponents,
                      std::unordered_map<int, PositionComponent>& posComponents,
                      std::unordered_map<int, MovementComponent>& moveComponents,
                      std::unordered_map<int, TypeComponent>& typeComponents,
                      std::unordered_map<int, StaticEntityComponent>& staticComponents,
                      std::unordered_map<int, BlackboardComponent>& bbComponents,
                      Blackboard& blackboard) {
    for (auto& [entityID, aiComp] : aiComponents) {
        if (aiComp.AI) {
            auto posIt = posComponents.find(entityID);
            auto moveIt = moveComponents.find(entityID);
            auto typeIt = typeComponents.find(entityID);
            auto staticIt = staticComponents.find(entityID);
            auto bbIt = bbComponents.find(entityID);

            if (posIt != posComponents.end() && typeIt != typeComponents.end()) {
                StaticEntityComponent dummyStatic = {0, 0};
                MovementComponent dummyMove = {0, 0, 1.0f, 0, 0, -1, -1, EntityDirection::Down, false};

                EnemyBlackboard enemyBB;
                enemyBB.entityID = entityID;
                enemyBB.position = &posIt->second;
                enemyBB.movement = (moveIt != moveComponents.end()) ? &moveIt->second : &dummyMove;
                enemyBB.AI = &aiComp;
                enemyBB.type = &typeIt->second;
                enemyBB.staticComp = (staticIt != staticComponents.end()) ? &staticIt->second : &dummyStatic;
                enemyBB.blackboardComponent = (bbIt != bbComponents.end()) ? &bbIt->second : nullptr;

                aiComp.AI->Tick(enemyBB, blackboard);
            }
        }
    }
}

void MovementSystem::Update(std::unordered_map<int, PositionComponent>& posComponents,
                            std::unordered_map<int, MovementComponent>& moveComponents,
                            std::unordered_map<int, AIComponent>& aiComponents) {
    for (auto& [entityID, moveComp] : moveComponents) {
        auto posIt = posComponents.find(entityID);
        auto aiIt = aiComponents.find(entityID);

        if (posIt != posComponents.end() && aiIt != aiComponents.end()) {
            PositionComponent& pos = posIt->second;
            MovementComponent& move = moveComp;
            AIComponent& ai = aiIt->second;

            // Update movement similar to Actor::UpdateMovement and Actor::Move
            if (HasReachedNode(pos, move) && !ai.currentPath.empty()) {
                auto nextNode = ai.currentPath.back();
                ai.currentPath.pop_back();
                move.targetX = nextNode.x;
                move.targetY = nextNode.y;
                SetTargetDirection(pos, move, nextNode.x, nextNode.y);
            }

            Move(pos, move);
        }
    }
}

bool MovementSystem::HasReachedNode(const PositionComponent& pos, MovementComponent& move) {
    return pos.visualX == float(pos.x) && pos.visualY == float(pos.y);
}

void MovementSystem::SetTargetDirection(PositionComponent& pos, MovementComponent& move, int targetX, int targetY) {
    int dx = targetX - pos.x;
    int dy = targetY - pos.y;

    if (dx > 0) pos.direction = EntityDirection::Right;
    else if (dx < 0) pos.direction = EntityDirection::Left;
    else if (dy > 0) pos.direction = EntityDirection::Down;
    else if (dy < 0) pos.direction = EntityDirection::Up;
}

void MovementSystem::Move(PositionComponent& pos, MovementComponent& move) {
    int dx = move.targetX - pos.x;
    int dy = move.targetY - pos.y;

    float step = 0.2f * move.speedModifier;
    float dirX = (pos.direction == EntityDirection::Right) - (pos.direction == EntityDirection::Left);
    float dirY = (pos.direction == EntityDirection::Down) - (pos.direction == EntityDirection::Up);
    pos.visualX += dirX * step;
    pos.visualY += dirY * step;

    if (dx == 0) pos.visualX = float(pos.x);
    if (dy == 0) pos.visualY = float(pos.y);

    if (dx != 0 && std::abs(pos.visualX - move.targetX) < 0.2f) {
        pos.visualX = float(move.targetX);
        pos.x = move.targetX;
    }
    if (dy != 0 && std::abs(pos.visualY - move.targetY) < 0.2f) {
        pos.visualY = float(move.targetY);
        pos.y = move.targetY;
    }
}

void RenderSystem::Update(std::unordered_map<int, PositionComponent>& posComponents,
                          std::unordered_map<int, MovementComponent>& moveComponents,
                          std::unordered_map<int, RenderComponent>& renderComponents,
                          SDL_Renderer* renderer, int cellWidth, int cellHeight, int widthMargin, int heightMargin, int squareSize) {
    for (auto& [entityID, renderComp] : renderComponents) {
        auto posIt = posComponents.find(entityID);
        if (posIt != posComponents.end()) {
            PositionComponent& pos = posIt->second;
            renderComp.rect.x = heightMargin + pos.visualX * cellHeight + 5;
            renderComp.rect.y = widthMargin + pos.visualY * cellWidth + 5;
            renderComp.rect.w = squareSize - 10;
            renderComp.rect.h = squareSize - 10;

            Animation &animationData = renderComp.animation;
            animationData.sourceRect.x = animationData.spriteSequence[animationData.spriteIndex].first;
            animationData.sourceRect.y = animationData.spriteSequence[animationData.spriteIndex].second;

            SDL_RenderTextureRotated(renderer, renderComp.texture.get(), &animationData.sourceRect, &renderComp.rect,
                                     (0.0 + ((pos.direction == EntityDirection::Up) * 90.0) +
                                      ((pos.direction == EntityDirection::Right) * 180.0) +
                                      ((pos.direction == EntityDirection::Down) * 270.0)), NULL, SDL_FLIP_NONE);

            animationData.timer++;
            if (animationData.timer % animationData.frameTime == 0) {
                animationData.spriteIndex = (animationData.spriteIndex + 1) % animationData.spriteSequence.size();
            }
            if (animationData.timer >= animationData.frameTime) {
                animationData.timer = 0;
            }
        }
    }
}