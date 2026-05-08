#pragma once
#include "./AI/BehaviorTree.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "Player.h"
#include "ResourceManager.h"

enum EntityType{
    BasicEnemy,
    AdvancedEnemy,
    CommandoEnemy,
    Claymore,
    WallCharge
};

class EntityFactory
{
private:
    SDL_Renderer *renderer;
    ResourceManager textureManager;
public:
    Actor CreateEntity(int x, int y, Direction direction, EntityType type) {
        auto texture = textureManager.getTexture("./Assets/Sprites/testSprite.png");
        Actor actor = Actor(x, y, texture);
        actor.actorType = ActorType::DinamicActor;
        actor.direction = direction;
        actor.DinamicEntity.goalX = x;
        actor.DinamicEntity.goalY = y;
        actor.DinamicEntity.LastSeenPlayerX = -1;
        actor.DinamicEntity.LastSeenPlayerY = -1;
        actor.DinamicEntity.speedModifier = 1;
        actor.DinamicEntity.isChasing = false;
        
        auto root = std::make_unique<SelectorNode>();

        switch(type){
        case EntityType::BasicEnemy : {
            actor.enemyType = EnemyType::BasicEnemy;
            // PRIORITY 1: Keep moving if we already have a path

            // PRIORITY 2: Search for Player (Vision)
            auto findAndPlan = std::make_unique<SequenceNode>();
            findAndPlan->AddChildNode(std::make_unique<FindTarget>());
            findAndPlan->AddChildNode(std::make_unique<FindPathNode>());
            root->AddChildNode(std::move(findAndPlan));

            auto SearchForPlayer = std::make_unique<SequenceNode>();
            SearchForPlayer->AddChildNode(std::make_unique<PredictPlayersTurn>());
            SearchForPlayer->AddChildNode(std::make_unique<JunctionDesitionNode>(0, 100, 0));
            root->AddChildNode(std::move(SearchForPlayer));

            root->AddChildNode(std::make_unique<JunctionDesitionNode>(100, 80, 1));
            break;
        }
        case EntityType::AdvancedEnemy : {
            break;
        }
        default:
            break;
        }

        actor.AI = std::move(root);
        return actor;
    };


    Actor CreateStaticEntity(int x, int y, Direction direction, EntityType type) {
        auto texture = textureManager.getTexture("./Assets/Sprites/testSprite.png");
        Actor actor = Actor(x, y, texture);
        actor.actorType = ActorType::StaticActor;
        actor.direction = direction;
        actor.DinamicEntity.speedModifier = 1;
        actor.DinamicEntity.isChasing = false;
        actor.StaticEntity.timer = 0;
        actor.StaticEntity.cooldown = 0;

        auto root = std::make_unique<SelectorNode>();

        switch(type){
        case EntityType::Claymore : {
            root->AddChildNode(std::make_unique<ClaymoreTriggerNode>());
            break;
        }
        case EntityType::WallCharge : {
            actor.StaticEntity.timer = 50; // 5 seconds at 0.1s ticks
            root->AddChildNode(std::make_unique<WallChargeDetonateNode>());
            break;
        }
        default:
            break;
        }

        actor.AI = std::move(root);
        return actor;
    };

    Player CreatePlayer(int x, int y, Direction direction, Blackboard &bb){
        Player player = Player (x, y, &bb);
        player.direction = direction;
        player.SetTexture(textureManager.getTexture("./Assets/Sprites/testSprite.png"));

        return player;
    };

    EntityFactory(SDL_Renderer *rend)
        : renderer(rend), textureManager(rend)
    {
    };

    EntityFactory()
        : renderer(nullptr), textureManager(nullptr)
    {
    };

    ~EntityFactory() = default;
};