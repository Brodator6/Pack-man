#pragma once
#include <vector>
#include <memory>
#include <stdlib.h> // Required for rand() and srand()
#include <ctime>   // Required if you use time(0) to seed rand
#include <iostream>

#include "AIUtils.h"
#include "../Pathfinding/Pathfinding.h"
#include "../Enemy.h"
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
    virtual NodeStatus Tick(Enemy &enemy, Blackboard& bb) = 0;
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
    NodeStatus Tick(Enemy &enemy, Blackboard& bb) override {
        for (auto& child : childNodes) {
            NodeStatus status = child->Tick(enemy, bb);

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
    NodeStatus Tick(Enemy &enemy, Blackboard& bb) override {
        for (auto& child : childNodes) {
            NodeStatus status = child->Tick(enemy, bb);
            
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
    NodeStatus Tick(Enemy &enemy, Blackboard& bb) override {
        int dirX =  (enemy.direction == Direction::Right) - (enemy.direction == Direction::Left);
        int dirY = (enemy.direction == Direction::Down) - (enemy.direction == Direction::Up);

        int checkX = enemy.GetPositionX();
        int checkY = enemy.GetPositionY();

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
                    enemy.DinamicEntity.goalX = checkX;
                    enemy.DinamicEntity.goalY = checkY;
                    enemy.DinamicEntity.LastSeenPlayerX = checkX;
                    enemy.DinamicEntity.LastSeenPlayerY = checkY;
                    if(!enemy.DinamicEntity.isChasing){
                        std::cout << "chasing player" << std::endl;
                    }
                    enemy.DinamicEntity.isChasing = true;
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
    NodeStatus Tick(Enemy &enemy, Blackboard& bb) override {
        if(!enemy.currentPath.empty()) return NodeStatus::FAILURE;
        Grid gameGrid;
        gameGrid = Grid::GenerateGrid(&bb.level);

        int dirX = (enemy.direction == Direction::Right) - (enemy.direction == Direction::Left);
        int dirY = (enemy.direction == Direction::Down) - (enemy.direction == Direction::Up);

        APAthFinding pathFinding;
        enemy.currentPath = pathFinding.FindPath({enemy.GetPositionX() + dirX, enemy.GetPositionY() + dirY}, {enemy.DinamicEntity.goalX, enemy.DinamicEntity.goalY}, &gameGrid);
        
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
    NodeStatus Tick(Enemy &enemy, Blackboard& bb) override {
        // 1. If already moving, don't pick a new direction

        int x = enemy.GetPositionX();
        int y = enemy.GetPositionY();
        
        // Calculate current forward vector
        int dirX = (enemy.direction == Direction::Right) - (enemy.direction == Direction::Left);
        int dirY = (enemy.direction == Direction::Down) - (enemy.direction == Direction::Up);

        // This is your specific logic: If there's a side-path available...
        if (enemy.HasReachedNode() && (bb.level[y + dirX][x + dirY].isWalkable || bb.level[y + dirX * -1][x + dirY * -1].isWalkable) && !enemy.DinamicEntity.isChasing) {
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
            enemy.DinamicEntity.goalX = selection.first;
            enemy.DinamicEntity.goalY = selection.second;
            enemy.currentPath = {{selection.first, selection.second, true}};
            //enemy.currentPath.push_back({selection.first, selection.second, true});
            return NodeStatus::SUCCESS;
        }
        
        if(!bb.level[y + dirY][x + dirX].isWalkable){
            enemy.direction = static_cast<Direction>((enemy.direction + 2) % 4);
            dirX *=-1;
            dirY *= -1;
        }

        if(enemy.currentPath.empty()){
            enemy.currentPath = {{x + dirX, y + dirY, true}};
            return NodeStatus::SUCCESS;   
            //enemy.currentPath = AIUtils::GetPathToNextWalkableTile(bb, enemy, dirX, dirY);
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
    NodeStatus Tick(Enemy &enemy, Blackboard& bb) override {
        if(enemy.HasReachedNode() && enemy.GetPositionX() == enemy.DinamicEntity.LastSeenPlayerX && enemy.GetPositionY() == enemy.DinamicEntity.LastSeenPlayerY && enemy.DinamicEntity.isChasing){
            enemy.DinamicEntity.LastSeenPlayerX = -1;
            enemy.DinamicEntity.LastSeenPlayerY = -1;
            enemy.DinamicEntity.isChasing = false;
            return NodeStatus::SUCCESS;
        }
        return NodeStatus::FAILURE;
    }
};