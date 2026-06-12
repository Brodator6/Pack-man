#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <climits>
#include <stdlib.h>
#include <ctime>
#include <iostream>


#include "../Pathfinding/Pathfinding.h"
#include "../Components.h"
#include "../Systems.h"
#include "../../DataStructs.h"
#include "../EntityManager.h"

enum NodeStatus{
    FAILURE = -1,
    RUNNING = 0,
    SUCCESS = 1
};

class Node
{
public:
    virtual NodeStatus Tick(EnemyBlackboard& enemyBB, Blackboard& bb) = 0;
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
    NodeStatus Tick(EnemyBlackboard& enemyBB, Blackboard& bb) override {
        for (auto& child : childNodes) {
            NodeStatus status = child->Tick(enemyBB, bb);

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
    NodeStatus Tick(EnemyBlackboard& enemyBB, Blackboard& bb) override {
        for (auto& child : childNodes) {
            NodeStatus status = child->Tick(enemyBB, bb);
            
            if (status == NodeStatus::SUCCESS){
                Reset();
                return status;
            }
            if(status == NodeStatus::RUNNING) return status;
        }
        return NodeStatus::FAILURE;
    }
};

class IncreaseTickCounter : public Node
{
public:
    NodeStatus Tick(EnemyBlackboard& enemyBB, Blackboard& bb) override {
        enemyBB.AI->tickCounter++;
        return NodeStatus::FAILURE;
    }
};

class FindTarget : public Node
{
public:
    NodeStatus Tick(EnemyBlackboard& enemyBB, Blackboard& bb) override {
        int dirX =  (enemyBB.position->direction == EntityDirection::Right) - (enemyBB.position->direction == EntityDirection::Left);
        int dirY = (enemyBB.position->direction == EntityDirection::Down) - (enemyBB.position->direction == EntityDirection::Up);

        int checkX = enemyBB.position->x;
        int checkY = enemyBB.position->y;

        while (true) {//check till first wall
            checkX += dirX;
            checkY += dirY;
            
            if (checkX < 0 || checkX >= bb.columns || checkY < 0 || checkY >= bb.rows || !bb.level[checkY][checkX].isWalkable){ 
                break;// out of bounds?
            }

            for (int slot = 0; slot < 3; ++slot) {
                if (bb.entityManager.shadowGrid[checkY * bb.columns + checkX].entityIDs[slot] == 0 ) {
                    enemyBB.movement->goalX = checkX;
                    enemyBB.movement->goalY = checkY;
                    enemyBB.movement->LastSeenPlayerX = checkX;
                    enemyBB.movement->LastSeenPlayerY = checkY;
                    enemyBB.movement->lastSeenDirection = bb.entityManager.GetPlayer().positionComponent.direction;
                    enemyBB.movement->isChasing = true;

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
    NodeStatus Tick(EnemyBlackboard& enemyBB, Blackboard& bb) override {
        if(enemyBB.AI->tickCounter < 15 && !enemyBB.AI->currentPath.empty()) return NodeStatus::FAILURE;
        enemyBB.AI->tickCounter = 0;

        Grid gameGrid;
        gameGrid = Grid::GenerateGrid(&bb.level);

        int dirX = (enemyBB.position->direction == EntityDirection::Right) - (enemyBB.position->direction == EntityDirection::Left);
        int dirY = (enemyBB.position->direction == EntityDirection::Down) - (enemyBB.position->direction == EntityDirection::Up);

        APAthFinding pathFinding;
        enemyBB.AI->currentPath = pathFinding.FindPath({enemyBB.position->x + dirX, enemyBB.position->y + dirY}, {enemyBB.movement->goalX, enemyBB.movement->goalY}, &gameGrid);
        
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
    NodeStatus Tick(EnemyBlackboard& enemyBB, Blackboard& bb) override {

        int x = enemyBB.position->x;
        int y = enemyBB.position->y;
        
        int dirX = (enemyBB.position->direction == EntityDirection::Right) - (enemyBB.position->direction == EntityDirection::Left);
        int dirY = (enemyBB.position->direction == EntityDirection::Down) - (enemyBB.position->direction == EntityDirection::Up);

        if (MovementSystem::HasReachedNode(*enemyBB.position, *enemyBB.movement) && (bb.level[y + dirX][x + dirY].isWalkable || bb.level[y + dirX * -1][x + dirY * -1].isWalkable) && !enemyBB.movement->isChasing) {
            const std::pair<int, int> dirs[4] = {{0,-1}, {1,0}, {0,1}, {-1,0}};

            std::vector<Choice> weightedChoices;
            int totalWeight = 0;

            for (auto [dx, dy] : dirs) {
                int nx = x + dx, ny = y + dy;

                // Validation
                if (nx >= 0 && nx < bb.columns && ny >= 0 && ny < bb.rows && bb.level[ny][nx].isWalkable) {
                    int weight = Weights.SidewayWeight; // Base weight for turns

                    if (dx == dirX && dy == dirY) {
                        weight = Weights.ForwardWeight;
                    } else if (dx == -dirX && dy == -dirY) {
                        weight = Weights.BackwardWeight;
                    }

                    weightedChoices.push_back({{nx, ny}, weight});
                    totalWeight += weight;
                }
            }

            if (weightedChoices.empty()) return NodeStatus::FAILURE;

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

            enemyBB.movement->goalX = selection.first;
            enemyBB.movement->goalY = selection.second;
            enemyBB.AI->currentPath = {{selection.first, selection.second, true}};
            return NodeStatus::SUCCESS;
        }
        
        if(!bb.level[y + dirY][x + dirX].isWalkable){
            enemyBB.position->direction = static_cast<EntityDirection>((enemyBB.position->direction + 2) % 4);
            dirX *=-1;
            dirY *= -1;
        }

        if(enemyBB.AI->currentPath.empty()){
            enemyBB.AI->currentPath = {{x + dirX, y + dirY, true}};
            return NodeStatus::SUCCESS;   
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
    NodeStatus Tick(EnemyBlackboard& enemyBB, Blackboard& bb) override {
        if(MovementSystem::HasReachedNode(*enemyBB.position, *enemyBB.movement) && enemyBB.position->x == enemyBB.movement->LastSeenPlayerX && enemyBB.position->y == enemyBB.movement->LastSeenPlayerY && enemyBB.movement->isChasing){

            int dirX = (enemyBB.movement->lastSeenDirection == EntityDirection::Right) - (enemyBB.movement->lastSeenDirection == EntityDirection::Left);
            int dirY = (enemyBB.movement->lastSeenDirection == EntityDirection::Down) - (enemyBB.movement->lastSeenDirection == EntityDirection::Up);

            enemyBB.AI->currentPath = {{enemyBB.position->x + dirX, enemyBB.position->y + dirY, true}};

            enemyBB.movement->LastSeenPlayerX = -1;
            enemyBB.movement->LastSeenPlayerY = -1;
            enemyBB.movement->isChasing = false;
            return NodeStatus::SUCCESS;
        }
        return NodeStatus::FAILURE;
    }
};

class MineTriggerNode : public Node
{
public:
    NodeStatus Tick(EnemyBlackboard& enemyBB, Blackboard& bb) override {
        int originX = enemyBB.position->x;
        int originY = enemyBB.position->y;

        const GridCell* cell = bb.entityManager.GetGridCell(originX, originY);
        if (cell == nullptr) {
            return NodeStatus::FAILURE;
        }

        std::vector<int> toRemove;
        for (int slot = 0; slot < 3; ++slot) {
            int id = cell->entityIDs[slot];
            if (id < 0 || id == enemyBB.entityID) {
                continue;
            }
            
            toRemove.push_back(id);
        }

        if (toRemove.empty()) {
            return NodeStatus::FAILURE;
        }

        for (int id : toRemove) {
            bb.entityManager.RequestRemoveEntityByID(id);
        }

        bb.entityManager.RequestRemoveEntityByID(enemyBB.entityID);

        return NodeStatus::SUCCESS;
    }
};

class WallChargeDetonateNode : public Node
{
public:
    NodeStatus Tick(EnemyBlackboard& enemyBB, Blackboard& bb) override {
        if (enemyBB.staticComp->timer > 0) {
            enemyBB.staticComp->timer -= 1;
            return NodeStatus::RUNNING;
        }

        int originX = enemyBB.position->x;
        int originY = enemyBB.position->y;

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
                    if (id >= 0 && id != enemyBB.entityID) {
                        toRemove.push_back(id);
                    }
                }
                for (int id : toRemove) {
                    bb.entityManager.RequestRemoveEntityByID(id);
                }
                bb.entityManager.RequestRemoveEntityByID(enemyBB.entityID);
            }
        }

        if (originX >= 0 && originX < bb.columns && originY >= 0 && originY < bb.rows) {
            bb.level[originY][originX].type = FLOOR;
            bb.level[originY][originX].isWalkable = true;
        }

        if (enemyBB.entityID >= 0) {
            bb.entityManager.RequestRemoveEntityByID(enemyBB.entityID);
        }

        return NodeStatus::SUCCESS;
    }
};

class ShareTargetInformationNode : public Node
{
public:
    NodeStatus Tick(EnemyBlackboard& enemyBB, Blackboard& bb) override {
        if (!enemyBB.blackboardComponent || !enemyBB.blackboardComponent->sharedBB) {
            return NodeStatus::FAILURE;
        }

        auto* sharedBB = enemyBB.blackboardComponent->sharedBB;

        if (enemyBB.movement->isChasing) {
            sharedBB->ShareTargetInformation(
                enemyBB.entityID,
                enemyBB.movement->LastSeenPlayerX,
                enemyBB.movement->LastSeenPlayerY,
                enemyBB.movement->lastSeenDirection,
                true);
            return NodeStatus::SUCCESS;
        }
        
        return NodeStatus::FAILURE;
    }
};

class UseSharedTargetInformationNode : public Node
{
public:
    NodeStatus Tick(EnemyBlackboard& enemyBB, Blackboard& bb) override {
        if (!enemyBB.blackboardComponent || !enemyBB.blackboardComponent->sharedBB) {
            return NodeStatus::FAILURE;
        }

        auto* sharedBB = enemyBB.blackboardComponent->sharedBB;

        SharedTargetInfo target = sharedBB->GetLatestSharedTarget();

        if (target.lastSeenX != enemyBB.movement->goalX && target.lastSeenY != enemyBB.movement->goalY && sharedBB->sharedTarget.isValidInfo) {
            enemyBB.movement->goalX = target.lastSeenX;
            enemyBB.movement->goalY = target.lastSeenY;
            enemyBB.movement->LastSeenPlayerX = target.lastSeenX;
            enemyBB.movement->LastSeenPlayerY = target.lastSeenY;
            enemyBB.movement->isChasing = true;
            return NodeStatus::SUCCESS;
        }
        
        return NodeStatus::FAILURE;
    }
};

class UpdateSharedData : public Node
{
public:
    NodeStatus Tick(EnemyBlackboard& enemyBB, Blackboard& bb) override {
        if (!enemyBB.blackboardComponent || !enemyBB.blackboardComponent->sharedBB) {
            return NodeStatus::FAILURE;
        }

        auto* sharedBB = enemyBB.blackboardComponent->sharedBB;

        sharedBB->ShareTargetInformation(
            enemyBB.entityID,
            enemyBB.movement->LastSeenPlayerX,
            enemyBB.movement->LastSeenPlayerY,
            enemyBB.movement->lastSeenDirection,
            false);

        if (sharedBB == &bb.entityManager.commandoEnemyBlackboard) {
            bb.entityManager.commandoEnemyBlackboard.ClearOrders();
            bb.entityManager.commandoEnemyBlackboard.commandTickCount = 0;
        }

        std::cout << "no target on the position" << std::endl;
        return NodeStatus::SUCCESS;
    }
};

class ExecuteCommandoOrderNode : public Node {
public:
    NodeStatus Tick(EnemyBlackboard& enemyBB, Blackboard& bb) override {
        if (!enemyBB.blackboardComponent || !enemyBB.blackboardComponent->sharedBB) {
            return NodeStatus::FAILURE;
        }

        CommandoOrder order = bb.entityManager.commandoEnemyBlackboard.GetOrder(enemyBB.entityID);

        if (!order.isAssigned || order.targetX < 0 || order.targetY < 0 || order.targetX >= bb.columns || order.targetY >= bb.rows) {
            bb.entityManager.commandoEnemyBlackboard.orders.erase(enemyBB.entityID);
            return NodeStatus::FAILURE;
        }

        int dirX =  (enemyBB.position->direction == EntityDirection::Right) - (enemyBB.position->direction == EntityDirection::Left);
        int dirY = (enemyBB.position->direction == EntityDirection::Down) - (enemyBB.position->direction == EntityDirection::Up);

        int checkX = enemyBB.position->x;
        int checkY = enemyBB.position->y;

        while (true) {//check till first wall
            checkX += dirX;
            checkY += dirY;
            
            if (checkX < 0 || checkX >= bb.columns || checkY < 0 || checkY >= bb.rows || !bb.level[checkY][checkX].isWalkable){ 
                break;// out of bounds?
            }

            for (int slot = 0; slot < 3; ++slot) {
                if (bb.entityManager.shadowGrid[checkY * bb.columns + checkX].entityIDs[slot] == 0) {
                    enemyBB.movement->goalX = checkX;
                    enemyBB.movement->goalY = checkY;
                    enemyBB.movement->LastSeenPlayerX = checkX;
                    enemyBB.movement->LastSeenPlayerY = checkY;
                    enemyBB.movement->lastSeenDirection = bb.entityManager.GetPlayer().positionComponent.direction;
                    enemyBB.movement->isChasing = true;

                    enemyBB.blackboardComponent->sharedBB->ShareTargetInformation(
                        enemyBB.entityID,
                        enemyBB.movement->LastSeenPlayerX,
                        enemyBB.movement->LastSeenPlayerY,
                        enemyBB.movement->lastSeenDirection,
                        true);
                    break;
                }
            }
        }

        if (enemyBB.position->x == order.targetX && enemyBB.position->y == order.targetY) {
            if (order.holdPosition) {
                enemyBB.movement->targetX = enemyBB.position->x;
                enemyBB.movement->targetY = enemyBB.position->y;
                enemyBB.movement->isChasing = false;
                enemyBB.AI->currentPath.clear();
                return NodeStatus::SUCCESS;
            } else if (order.chase){
                enemyBB.AI->currentPath.clear();
                bb.entityManager.commandoEnemyBlackboard.orders.erase(enemyBB.entityID);
                return NodeStatus::FAILURE;
            }
            return NodeStatus::FAILURE;
        }

        // Only recalculate path if agent has reached their current node (not mid-movement)
        bool hasReachedNode = MovementSystem::HasReachedNode(*enemyBB.position, *enemyBB.movement);
        
        if (hasReachedNode && (enemyBB.movement->goalX != order.targetX || enemyBB.movement->goalY != order.targetY || enemyBB.AI->currentPath.empty())) {
            enemyBB.movement->goalX = order.targetX;
            enemyBB.movement->goalY = order.targetY;
            enemyBB.movement->isChasing = false;

            Grid gameGrid = Grid::GenerateGrid(&bb.level);
            APAthFinding pathFinding;
            // Start pathfinding from the nearest tile (the one agent has reached)
            enemyBB.AI->currentPath = pathFinding.FindPath({enemyBB.position->x, enemyBB.position->y}, {enemyBB.movement->goalX, enemyBB.movement->goalY}, &gameGrid);
        }

        return NodeStatus::SUCCESS;
    }
};

class PlanCommandoAmbushNode : public Node {
private:
    int cooldown = 30;
    int interceptionChance = 40;

    std::pair<int,int> DirFromDirection(int direction) const {
        switch (direction) {
            case EntityDirection::Left: return {-1, 0};
            case EntityDirection::Right: return {1, 0};
            case EntityDirection::Up: return {0, -1};
            case EntityDirection::Down: return {0, 1};
            default: return {0, 0};
        }
    }

    bool IsWalkable(const Blackboard& bb, int x, int y) const {
        return x >= 0 && x < bb.columns && y >= 0 && y < bb.rows && bb.level[y][x].isWalkable;
    }

    std::pair<int,int> GetPerpendicularLeft(std::pair<int,int> dir) const {
        return {-dir.second, dir.first};
    }

    // Generate blocking points at distance range [minDist, maxDist] from target
    void GenerateBlockingPoints(
        std::vector<std::pair<int,int>>& blockingPoints,
        const Blackboard& bb,
        int targetX, int targetY,
        std::pair<int,int> forward,
        std::pair<int,int> left,
        std::pair<int,int> right,
        int minDist = 2, int maxDist = 7)
    {
        auto pushCandidate = [&](int x, int y) {
            if (IsWalkable(bb, x, y)) {
                for (auto& candidate : blockingPoints) {
                    if (candidate.first == x && candidate.second == y) return;
                }
                blockingPoints.emplace_back(x, y);
            }
        };

        // Generate front blocking positions (straight ahead, 2-7 tiles)
        for (int dist = minDist; dist <= maxDist; ++dist) {
            pushCandidate(targetX + forward.first * dist, targetY + forward.second * dist);
        }

        // Generate left flank positions
        for (int dist = minDist; dist <= maxDist; ++dist) {
            pushCandidate(targetX + left.first * dist, targetY + left.second * dist);
        }

        // Generate right flank positions
        std::pair<int,int> right_dir = {-left.first, -left.second};
        for (int dist = minDist; dist <= maxDist; ++dist) {
            pushCandidate(targetX + right_dir.first * dist, targetY + right_dir.second * dist);
        }

        // Fallback: generate closer positions if needed
        if (blockingPoints.empty()) {
            for (int dist = 1; dist < minDist; ++dist) {
                pushCandidate(targetX + forward.first * dist, targetY + forward.second * dist);
                pushCandidate(targetX + left.first * dist, targetY + left.second * dist);
                pushCandidate(targetX + right_dir.first * dist, targetY + right_dir.second * dist);
            }
        }
    }

    // Manhattan distance helper
    int ManhattanDistance(int x1, int y1, int x2, int y2) const {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    }

    // Check if agent is behind or to the side of player (not in front)
    bool IsAgentBehindOrSide(int agentX, int agentY, int playerX, int playerY, std::pair<int,int> forward) const {
        int distAlongForward = (agentX - playerX) * forward.first + (agentY - playerY) * forward.second;
        return distAlongForward <= 0;
    }

public:
    NodeStatus Tick(EnemyBlackboard& enemyBB, Blackboard& bb) override {
        auto& commandoBB = bb.entityManager.commandoEnemyBlackboard;

        // Update shared target info if leader is chasing
        if (enemyBB.movement->isChasing) {
            commandoBB.ShareTargetInformation(
                enemyBB.entityID,
                enemyBB.movement->LastSeenPlayerX,
                enemyBB.movement->LastSeenPlayerY,
                enemyBB.movement->lastSeenDirection,
                true);
        }

        SharedTargetInfo target = commandoBB.GetLatestSharedTarget();
        
        if (!target.isValidInfo) {
            commandoBB.ClearOrders();
            commandoBB.commandTickCount = 0;
            return NodeStatus::FAILURE;
        }

        std::pair<int,int> forward = DirFromDirection(target.lastSeenDirection);
        std::pair<int,int> left = GetPerpendicularLeft(forward);
        std::pair<int,int> right = {-left.first, -left.second};

        // Generate blocking positions at distance 2-4 tiles
        std::vector<std::pair<int,int>> blockingPoints;
        GenerateBlockingPoints(blockingPoints, bb, target.lastSeenX, target.lastSeenY, forward, left, right, 2, 4);

        if (blockingPoints.empty()) {
            commandoBB.ClearOrders();
            commandoBB.commandTickCount = 0;
            return NodeStatus::FAILURE;
        }

        auto& positions = bb.entityManager.GetPositionComponents();

        // Collect only the registered commando members, excluding the leader planning this ambush.
        std::vector<int> commandoAgents;
        for (int memberID : commandoBB.MemberIDs) {
            if (memberID == commandoBB.commanderID) continue;
            if (positions.find(memberID) == positions.end()) continue;
            commandoAgents.push_back(memberID);
        }

        if (commandoAgents.empty()) {
            commandoBB.ClearOrders();
            commandoBB.commandTickCount = 0;
            return NodeStatus::FAILURE;
        }

        std::vector<std::pair<int,int>> availableTargets = blockingPoints;

        if (commandoBB.commandTickCount >= cooldown || commandoBB.orders.empty()) {
            commandoBB.orders.clear();
            commandoBB.commandTickCount = 0;
            enemyBB.AI->currentPath = {};

            std::vector<int> blockerAgents; // Track agents assigned as blockers for interception

            // STAGE 1: Assign reporter to chase the target
            int reporterID = target.reporterID;
            if (reporterID >= 0 && std::find(commandoAgents.begin(), commandoAgents.end(), reporterID) != commandoAgents.end()) {
                commandoBB.AssignOrder(reporterID, CommandoOrder{true, target.lastSeenX, target.lastSeenY, false, true});
                commandoAgents.erase(std::remove(commandoAgents.begin(), commandoAgents.end(), reporterID), commandoAgents.end());
            }

            // STAGE 2: Send agents that are behind or to the side to chase (flanking attempts)
            for (auto it = commandoAgents.begin(); it != commandoAgents.end(); ) {
                int commandoID = *it;
                auto posIt = positions.find(commandoID);
                
                if (posIt == positions.end()) {
                    ++it;
                    continue;
                }

                if (IsAgentBehindOrSide(posIt->second.x, posIt->second.y, target.lastSeenX, target.lastSeenY, forward)) {
                    commandoBB.AssignOrder(commandoID, CommandoOrder{true, target.lastSeenX, target.lastSeenY, false, true});
                    it = commandoAgents.erase(it);
                } else {
                    ++it;
                }
            }

            // STAGE 3: Assign remaining agents to blocking positions
            for (int commandoID : commandoAgents) {
                auto posIt = positions.find(commandoID);
                if (posIt == positions.end()) continue;

                int agentX = posIt->second.x;
                int agentY = posIt->second.y;

                if (!availableTargets.empty()) {
                    // Find nearest blocking position
                    int bestDistance = INT_MAX;
                    int bestIndex = 0;
                    for (int index = 0; index < static_cast<int>(availableTargets.size()); ++index) {
                        auto [tx, ty] = availableTargets[index];
                        int dist = ManhattanDistance(agentX, agentY, tx, ty);
                        if (dist < bestDistance) {
                            bestDistance = dist;
                            bestIndex = index;
                        }
                    }

                    auto [targetX, targetY] = availableTargets[bestIndex];
                    commandoBB.AssignOrder(commandoID, CommandoOrder{true, targetX, targetY, true, false});
                    blockerAgents.push_back(commandoID);
                    availableTargets.erase(availableTargets.begin() + bestIndex);
                } else {
                    commandoBB.AssignOrder(commandoID, CommandoOrder{true, target.lastSeenX, target.lastSeenY, false, true});
                }
            }

            // STAGE 4: Occasionally send some blockers on longer interception routes to confuse player
            if (!blockerAgents.empty() && std::rand() % 100 < interceptionChance) {
                int randomBlockerIdx = std::rand() % blockerAgents.size();
                int blockerId = blockerAgents[randomBlockerIdx];
                commandoBB.AssignOrder(blockerId, CommandoOrder{true, target.lastSeenX, target.lastSeenY, false, true});
            }

        } else {
            commandoBB.commandTickCount++;
        }
        return NodeStatus::SUCCESS;
    }
};