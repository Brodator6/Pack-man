#pragma once
#include <vector>
#include <memory>

enum NodeStatus{
    FALURE,
    SUCCESS,
    RUNNING
};

class Node
{
public:
    virtual NodeStatus Tick() = 0;
    virtual ~Node() = default;
};

class CompositeNode : Node
{
protected:
    std::vector<std::unique_ptr<Node>> childNodes;
public:
    void AddChildNode(std::unique_ptr<Node> child){
        childNodes.push_back(std::move(child));
    };
};

class SequenceNode : CompositeNode
{
private:
    /* data */
public:
    NodeStatus Tick() override {
        for (auto& child : childNodes) {
            NodeStatus status = child->Tick();
            if (status != NodeStatus::SUCCESS) return status;
        }
        return NodeStatus::SUCCESS;
    }
};

class SelectorNode : CompositeNode
{
private:
    /* data */
public:
    NodeStatus Tick() override {
        for (auto& child : childNodes) {
            NodeStatus status = child->Tick();
            if (status != NodeStatus::SUCCESS) return status;
        }
        return NodeStatus::FALURE;
    }
};


