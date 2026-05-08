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

            if (posIt != posComponents.end() && moveIt != moveComponents.end() && typeIt != typeComponents.end()) {
                // For static entities, staticComp may not exist, so provide a dummy if not found
                StaticEntityComponent dummyStatic = {0, 0};
                StaticEntityComponent& staticComp = (staticIt != staticComponents.end()) ? staticIt->second : dummyStatic;

                aiComp.AI->Tick(entityID, posIt->second, moveIt->second, aiComp, typeIt->second, staticComp, blackboard);
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

void MovementSystem::SetTargetDirection(const PositionComponent& pos, MovementComponent& move, int targetX, int targetY) {
    int dx = targetX - pos.x;
    int dy = targetY - pos.y;

    if (dx > 0) move.direction = Direction::Right;
    else if (dx < 0) move.direction = Direction::Left;
    else if (dy > 0) move.direction = Direction::Down;
    else if (dy < 0) move.direction = Direction::Up;
}

void MovementSystem::Move(PositionComponent& pos, MovementComponent& move) {
    int dx = move.targetX - pos.x;
    int dy = move.targetY - pos.y;

    float step = 0.2f * move.speedModifier;
    float dirX = (move.direction == Direction::Right) - (move.direction == Direction::Left);
    float dirY = (move.direction == Direction::Down) - (move.direction == Direction::Up);
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
        auto moveIt = moveComponents.find(entityID);
        if (posIt != posComponents.end() && moveIt != moveComponents.end()) {
            PositionComponent& pos = posIt->second;
            MovementComponent& move = moveIt->second;
            renderComp.rect.x = heightMargin + pos.visualX * cellHeight + 5;
            renderComp.rect.y = widthMargin + pos.visualY * cellWidth + 5;
            renderComp.rect.w = squareSize - 10;
            renderComp.rect.h = squareSize - 10;
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, &renderComp.rect);
            SDL_RenderTextureRotated(renderer, renderComp.texture.get(), NULL, &renderComp.rect, 
                                     (0.0 + ((move.direction == Direction::Down) * 90.0) + 
                                      ((move.direction == Direction::Left) * 180.0) + 
                                      ((move.direction == Direction::Up) * 270.0)), NULL, SDL_FLIP_NONE);
        }
    }
}