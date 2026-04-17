#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include "../../Entities/Pathfinding/Pathfinding.h"
#include "../../Entities/Enemy.h"
#include "../../DataStructs.h"

struct Blackboard {
    Enemy &enemy;
    std::vector<std::vector<TileData>> &level;
    std::vector<GridCell> &shadowGrid;
};

enum NodeStatus{
    FALURE,
    SUCCESS,
    RUNNING
};

class Node
{
public:
    virtual NodeStatus Tick(Blackboard& bb) = 0;
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
    NodeStatus Tick(Blackboard& bb) override {
        for (auto& child : childNodes) {
            NodeStatus status = child->Tick(bb);

            if (status == NodeStatus::FALURE){
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
    NodeStatus Tick(Blackboard& bb) override {
        for (auto& child : childNodes) {
            NodeStatus status = child->Tick(bb);
            
            if (status == NodeStatus::SUCCESS){
                Reset();
                return status;
            }
            if(status == NodeStatus::RUNNING) return status;
        }
        return NodeStatus::FALURE;
    }
};

class FindTarget : public Node
{
public:
    NodeStatus Tick(Blackboard& bb) override {
        int dx =  (bb.enemy.direction == Direction::Right) - (bb.enemy.direction == Direction::Left);
        int dy = (bb.enemy.direction == Direction::Down) - (bb.enemy.direction == Direction::Up);
        
        int rows = 20, columns = 20;

        int checkX = bb.enemy.GetPositionX();
        int checkY = bb.enemy.GetPositionY();

        while (true) {//check till first wall
            checkX += dx;
            checkY += dy;
            
            if (checkX < 0 || checkX >= columns || checkY < 0 || checkY >= rows){ 
                break;// out of bounds?
            }
            if (!bb.level[checkY][checkX].isWalkable){ 
                break;// out of bounds?
            }

            for (int slot = 0; slot < 3; ++slot) {
                if (bb.shadowGrid[checkY * 20 + checkX].entityIDs[slot] == 0) {
                    bb.enemy.goalX = checkX;
                    bb.enemy.goalY = checkY;
                    return NodeStatus::SUCCESS;
                }
            }
        }

        std::cout << "no target in sight" << std:: endl;
        return NodeStatus::FALURE;
    }
};

class FindPathNode : public Node
{
public:
    NodeStatus Tick(Blackboard& bb) override {
        if(!bb.enemy.currentPath.empty()) return NodeStatus::FALURE;
        Grid gameGrid;
        gameGrid = Grid::GenerateGrid(&bb.level);

        APAthFinding pathFinding;
        bb.enemy.currentPath = pathFinding.FindPath({bb.enemy.GetPositionX(), bb.enemy.GetPositionY()}, {bb.enemy.goalX, bb.enemy.goalY}, &gameGrid);
        
        return NodeStatus::SUCCESS;
    }
};


class MoveNode : public Node
{
public:
    NodeStatus Tick(Blackboard& bb) override {
        if (!bb.enemy.HasReachedNode()) {
            return NodeStatus::RUNNING;
        }
        if (bb.enemy.currentPath.empty()) {
            return NodeStatus::FALURE;
        }
        return NodeStatus::RUNNING;
    }
};


inline std::unique_ptr<Node> BuildAI(){
    auto root = std::make_unique<SelectorNode>();

    // 1. First priority: If we have a path, keep moving.
    // If MoveNode returns RUNNING, the Selector stays here next frame.
    root->AddChildNode(std::make_unique<MoveNode>());

    // 2. Second priority: Find a target and plan a path.
    auto findAndPlan = std::make_unique<SequenceNode>();
    findAndPlan->AddChildNode(std::make_unique<FindTarget>());
    findAndPlan->AddChildNode(std::make_unique<FindPathNode>());

    root->AddChildNode(std::move(findAndPlan));

    return root;
}