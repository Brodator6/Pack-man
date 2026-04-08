#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include "../../Entities/Pathfinding/Pathfinding.h"
#include "../../Entities/Enemy.h"

enum NodeStatus{
    FALURE,
    SUCCESS,
    RUNNING
};

class Node
{
public:
    virtual NodeStatus Tick(Enemy *Enemy) = 0;
    virtual ~Node() = default;
};

class CompositeNode : public Node
{
protected:
    std::vector<std::unique_ptr<Node>> childNodes;
public:
    void AddChildNode(std::unique_ptr<Node> child){
        childNodes.push_back(std::move(child));
    };
};

class SequenceNode : public CompositeNode
{
public:
    NodeStatus Tick(Enemy *Enemy) override {
        for (auto& child : childNodes) {
            NodeStatus status = child->Tick(Enemy);
            if (status != NodeStatus::SUCCESS) return status;
        }
        return NodeStatus::SUCCESS;
    }
};

class SelectorNode : public CompositeNode
{
public:
    NodeStatus Tick(Enemy *Enemy) override {
        for (auto& child : childNodes) {
            NodeStatus status = child->Tick(Enemy);
            if (status != NodeStatus::FALURE) return status;
        }
        return NodeStatus::FALURE;
    }
};

class FindPathNode : public Node
{
public:
    NodeStatus Tick(Enemy *Enemy) override {
        std::cout << "finding path" << std::endl;
        if(Enemy->currentPath.empty()){
            Grid gameGrid;
            gameGrid = Grid::GenerateGrid(Enemy->level);

            APAthFinding pathFinding;
            Enemy->currentPath = pathFinding.FindPath({Enemy->GetPositionX(), Enemy->GetPositionY()}, {Enemy->goalX, Enemy->goalX}, &gameGrid);
            
            std::cout << "found" << std::endl;
            return NodeStatus::SUCCESS;
        };
        return NodeStatus::FALURE;
    }
};


class MoveNode : public Node
{
public:
    NodeStatus Tick(Enemy *Enemy) override {
        std::cout << "moving" << std::endl;
        if(!Enemy->currentPath.empty()){
            int x = Enemy->currentPath.back().x;
            int y = Enemy->currentPath.back().y;
            Enemy->SetPosition(x, y);

            Enemy->currentPath.pop_back();
            return NodeStatus::RUNNING;
        };
        std::cout << "at the target" << std::endl;
        return NodeStatus::FALURE;
    }
};


inline std::unique_ptr<Node> BuildAI(){
    auto root = std::make_unique<SelectorNode>();

    root->AddChildNode(std::make_unique<MoveNode>());
    root->AddChildNode(std::make_unique<FindPathNode>());

    return root;
}