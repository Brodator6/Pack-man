#pragma once
#include <vector>
#include <memory>
#include <stdlib.h> // Required for rand() and srand()
#include <ctime>   // Required if you use time(0) to seed rand
#include <iostream>

#include "AIUtils.h"
#include "../Pathfinding/Pathfinding.h"
#include "../Components.h"
#include "../Systems.h"
#include "../../DataStructs.h"
#include "../EntityManager.h"

enum NodeStatus{
    FAILURE,
    SUCCESS,
    RUNNING
};

class Node
{
public:
    virtual NodeStatus Tick(int entityID, const PositionComponent& position, MovementComponent& movement, AIComponent &Ai, const TypeComponent& type, StaticEntityComponent& staticComp, Blackboard& bb) = 0;
    virtual void Reset(){};
    virtual ~Node() = default;
};

class CompositeNode : public Node
{
protected:
    std::vector<std::unique_ptr<Node>> childNodes;
    size_t currentRunningNode = 0;
public:
    void AddChildNode(std::unique_ptr<Node> child){
        childNodes.push_back(std::move(child));
    };

    void Reset() override{
        currentRunningNode = 0;
        for (auto& child : childNodes){
            child->Reset();
        }
    }
};

class SequenceNode : public CompositeNode
{
public:
    NodeStatus Tick(int entityID, const PositionComponent& position, MovementComponent& movement, AIComponent &Ai, const TypeComponent& type, StaticEntityComponent& staticComp, Blackboard& bb) override {
        for (auto& child : childNodes) {
            NodeStatus status = child->Tick(entityID, position, movement, Ai, type, staticComp, bb);

            if (status == NodeStatus::FAILURE){
                Reset();
                return status;
            }
            if(status == NodeStatus::RUNNING) return status;
        }
        return NodeStatus::SUCCESS;
    }
};

class SelectorNode : public CompositeNode
{
public:
    NodeStatus Tick(int entityID, const PositionComponent& position, MovementComponent& movement, AIComponent &Ai, const TypeComponent& type, StaticEntityComponent& staticComp, Blackboard& bb) override {
        for (auto& child : childNodes) {
            NodeStatus status = child->Tick(entityID, position, movement, Ai, type, staticComp, bb);
            
            if (status == NodeStatus::SUCCESS){
                Reset();
                return status;
            }
            if(status == NodeStatus::RUNNING) return status;
        }
        return NodeStatus::FAILURE;
    }
};

class FindTarget : public Node
{
public:
    NodeStatus Tick(int entityID, const PositionComponent& position, MovementComponent& movement, AIComponent &Ai, const TypeComponent& type, StaticEntityComponent& staticComp, Blackboard& bb) override {
        int dirX =  (movement.direction == Direction::Right) - (movement.direction == Direction::Left);
        int dirY = (movement.direction == Direction::Down) - (movement.direction == Direction::Up);

        int checkX = position.x;
        int checkY = position.y;

        while (true) {//check till first wall
            checkX += dirX;
            checkY += dirY;
            
            if (checkX < 0 || checkX >= bb.columns || checkY < 0 || checkY >= bb.rows){ 
                break;// out of bounds?
            }
            if (!bb.level[checkY][checkX].isWalkable){ 
                break;// out of bounds?
            }

            for (int slot = 0; slot < 3; ++slot) {
                if (bb.entityManager.shadowGrid[checkY * bb.columns + checkX].entityIDs[slot] == 0) {
                    movement.goalX = checkX;
                    movement.goalY = checkY;
                    movement.LastSeenPlayerX = checkX;
                    movement.LastSeenPlayerY = checkY;
                    if(!movement.isChasing){
                        std::cout << "chasing player" << std::endl;
                    }
                    movement.isChasing = true;
                    return NodeStatus::SUCCESS;
                }
            }
        }
        return NodeStatus::FAILURE;
    }
};

class FindPathNode : public Node
{
public:
    NodeStatus Tick(int entityID, const PositionComponent& position, MovementComponent& movement, AIComponent &Ai, const TypeComponent& type, StaticEntityComponent& staticComp, Blackboard& bb) override {
        if(!Ai.currentPath.empty()) return NodeStatus::FAILURE;
        Grid gameGrid;
        gameGrid = Grid::GenerateGrid(&bb.level);

        int dirX = (movement.direction == Direction::Right) - (movement.direction == Direction::Left);
        int dirY = (movement.direction == Direction::Down) - (movement.direction == Direction::Up);

        APAthFinding pathFinding;
        Ai.currentPath = pathFinding.FindPath({position.x + dirX, position.y + dirY}, {movement.goalX, movement.goalY}, &gameGrid);
        
        return NodeStatus::SUCCESS;
    }
};

class JunctionDesitionNode : public Node {
private:
    struct Choice {
        std::pair<int, int> pos;
        int weight;
    };

    struct {
        int ForwardWeight;
        int SidewayWeight;
        int BackwardWeight;
    } Weights;

public:
    NodeStatus Tick(int entityID, const PositionComponent& position, MovementComponent& movement, AIComponent &Ai, const TypeComponent& type, StaticEntityComponent& staticComp, Blackboard& bb) override {
        // 1. If already moving, don't pick a new direction

        int x = position.x;
        int y = position.y;
        
        // Calculate current forward vector
        int dirX = (movement.direction == Direction::Right) - (movement.direction == Direction::Left);
        int dirY = (movement.direction == Direction::Down) - (movement.direction == Direction::Up);

        // This is your specific logic: If there's a side-path available...
        if (MovementSystem::HasReachedNode(position, movement) && (bb.level[y + dirX][x + dirY].isWalkable || bb.level[y + dirX * -1][x + dirY * -1].isWalkable) && !movement.isChasing) {
            std::cout << "turn?" << std::endl;
            const std::pair<int, int> dirs[4] = {{0,-1}, {1,0}, {0,1}, {-1,0}};

            std::vector<Choice> weightedChoices;
            int totalWeight = 0;

            for (auto [dx, dy] : dirs) {
                int nx = x + dx, ny = y + dy;

                // Validation
                if (nx >= 0 && nx < bb.columns && ny >= 0 && ny < bb.rows && bb.level[ny][nx].isWalkable) {
                    int weight = Weights.SidewayWeight; // Base weight for turns

                    if (dx == dirX && dy == dirY) {
                        weight = Weights.ForwardWeight; // Strong preference for forward
                    } else if (dx == -dirX && dy == -dirY) {
                        weight = Weights.BackwardWeight;   // Strong dislike for U-turns
                    }

                    weightedChoices.push_back({{nx, ny}, weight});
                    totalWeight += weight;
                }
            }

            if (weightedChoices.empty()) return NodeStatus::FAILURE;

            // 2. Weighted Random Selection
            int roll = std::rand() % totalWeight;
            int currentSum = 0;
            std::pair<int, int> selection;

            for (const auto& choice : weightedChoices) {
                currentSum += choice.weight;
                if (roll < currentSum) {
                    selection = choice.pos;
                    break;
                }
            }

            // 3. Apply results
            movement.goalX = selection.first;
            movement.goalY = selection.second;
            Ai.currentPath = {{selection.first, selection.second, true}};
            //actor.currentPath.push_back({selection.first, selection.second, true});
            return NodeStatus::SUCCESS;
        }
        
        if(!bb.level[y + dirY][x + dirX].isWalkable){
            movement.direction = static_cast<Direction>((movement.direction + 2) % 4);
            dirX *=-1;
            dirY *= -1;
        }

        if(Ai.currentPath.empty()){
            Ai.currentPath = {{x + dirX, y + dirY, true}};
            return NodeStatus::SUCCESS;   
            //actor.currentPath = AIUtils::GetPathToNextWalkableTile(bb, actor, dirX, dirY);
        }
        return NodeStatus::FAILURE;
    }

    JunctionDesitionNode(int forwardWeight, int sidewayWeight,  int backwardWeight){
        Weights.SidewayWeight = sidewayWeight;
        Weights.ForwardWeight = forwardWeight;
        Weights.BackwardWeight = backwardWeight;
    }
};

class PredictPlayersTurn : public Node
{
public:
    NodeStatus Tick(int entityID, const PositionComponent& position, MovementComponent& movement, AIComponent &Ai, const TypeComponent& type, StaticEntityComponent& staticComp, Blackboard& bb) override {
        if(MovementSystem::HasReachedNode(position, movement) && position.x == movement.LastSeenPlayerX && position.y == movement.LastSeenPlayerY && movement.isChasing){
            movement.LastSeenPlayerX = -1;
            movement.LastSeenPlayerY = -1;
            movement.isChasing = false;
            return NodeStatus::SUCCESS;
        }
        return NodeStatus::FAILURE;
    }
};
//needs reviewing

class ClaymoreTriggerNode : public Node
{
public:
    NodeStatus Tick(int entityID, const PositionComponent& position, MovementComponent& movement, AIComponent &Ai, const TypeComponent& type, StaticEntityComponent& staticComp, Blackboard& bb) override {
        int dirX = (movement.direction == Direction::Right) - (movement.direction == Direction::Left);
        int dirY = (movement.direction == Direction::Down) - (movement.direction == Direction::Up);
        int checkX = position.x + dirX;
        int checkY = position.y + dirY;

        if (checkX < 0 || checkX >= bb.columns || checkY < 0 || checkY >= bb.rows) {
            return NodeStatus::FAILURE;
        }

        const GridCell* cell = bb.entityManager.GetGridCell(checkX, checkY);
        if (cell == nullptr) {
            return NodeStatus::FAILURE;
        }

        std::vector<int> toRemove;
        for (int slot = 0; slot < 3; ++slot) {
            int id = cell->entityIDs[slot];
            if (id > bb.entityManager.GetPlayer().ID && bb.entityManager.GetEntityById(id)->actorType == ActorType::DinamicActor) {
                toRemove.push_back(id);
            }
        }

        if (toRemove.empty()) {
            return NodeStatus::FAILURE;
        }

        for (int id : toRemove) {
            bb.entityManager.RequestRemoveEntityByID(id);
        }
        
        bb.entityManager.RequestRemoveEntityByID(entityID);

        return NodeStatus::SUCCESS;
    }
};

class WallChargeDetonateNode : public Node
{
public:
    NodeStatus Tick(int entityID, const PositionComponent& position, MovementComponent& movement, AIComponent &Ai, const TypeComponent& type, StaticEntityComponent& staticComp, Blackboard& bb) override {
        if (staticComp.timer > 0) {
            staticComp.timer -= 1;
            return NodeStatus::RUNNING;
        }

        int originX = position.x;
        int originY = position.y;

        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                int x = originX + dx;
                int y = originY + dy;
                if (x < 0 || x >= bb.columns || y < 0 || y >= bb.rows) {
                    continue;
                }
                const GridCell* cell = bb.entityManager.GetGridCell(x, y);
                if (cell == nullptr) {
                    continue;
                }
                std::vector<int> toRemove;
                for (int slot = 0; slot < 3; ++slot) {
                    int id = cell->entityIDs[slot];
                    if (id >= 0 && id != entityID) {
                        toRemove.push_back(id);
                    }
                }
                for (int id : toRemove) {
                    bb.entityManager.RequestRemoveEntityByID(id);
                }
            }
        }

        if (originX >= 0 && originX < bb.columns && originY >= 0 && originY < bb.rows) {
            bb.level[originY][originX].type = FLOOR;
            bb.level[originY][originX].isWalkable = true;
        }

        if (entityID >= 0) {
            bb.entityManager.RequestRemoveEntityByID(entityID);
        }

        return NodeStatus::SUCCESS;
    }
};

//NEEDS CHECKING AND REWORKING

// ============================================================================
// SHARED TARGET INFORMATION NODES (For AdvancedEnemy and CommandoEnemy)
// ============================================================================

/// ShareTargetInformationNode
/// This node implements cooperative target sharing for advanced enemy types.
/// It allows enemies to communicate the player's last known location.
/// Extensible for CommandoType where one enemy can act as commander.
/*
class ShareTargetInformationNode : public Node
{
private:
    int tickCounter = 0;
    
public:
    NodeStatus Tick(Actor &actor, Blackboard& bb) override {
        tickCounter++;
        
        // Check if this enemy has visual on the player
        if (actor.DinamicEntity.isChasing) {
            actor.advancedEnemyBB->ShareTargetInformation(
            actor.entityID,
            actor.DinamicEntity.LastSeenPlayerX,
            actor.DinamicEntity.LastSeenPlayerY,
            tickCounter);
            return NodeStatus::SUCCESS;
        }
        
        return NodeStatus::FAILURE;
    }
};

/// UseSharedTargetInformationNode
/// This node allows advanced enemies to use target information shared by teammates.
/// If this enemy hasn't spotted the player but a teammate has, it uses that information.
class UseSharedTargetInformationNode : public Node
{
public:
    NodeStatus Tick(Actor &actor, Blackboard& bb) override {
        // Only use shared information if not already chasing
        if (!actor.DinamicEntity.isChasing) {
            SharedTargetInfo target = actor.advancedEnemyBB->GetLatestSharedTarget();
            
            // If teammates have spotted the player, use that information
            if (target.lastSeenX >= 0 && target.lastSeenY >= 0) {
                actor.DinamicEntity.goalX = target.lastSeenX;
                actor.DinamicEntity.goalY = target.lastSeenY;
                actor.DinamicEntity.LastSeenPlayerX = target.lastSeenX;
                actor.DinamicEntity.LastSeenPlayerY = target.lastSeenY;
                actor.DinamicEntity.isChasing = true;
                return NodeStatus::SUCCESS;
            }
        }
        
        return NodeStatus::FAILURE;
    }
};

*/