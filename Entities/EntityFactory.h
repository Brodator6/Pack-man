#pragma once
#include "./AI/BehaviorTree.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "Player.h"

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
public:
    Actor CreateEntity(int x, int y, Direction direction,EntityType type) {
        Actor actor = Actor(x, y, IMG_LoadTexture(renderer, "./Assets/Sprites/testSprite.png"));
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
            actor.enemyType = EnemyType::AdvancedEnemy;
            
            // PRIORITY 2: Find and share target information (direct vision)
            auto findAndShare = std::make_unique<SequenceNode>();
            findAndShare->AddChildNode(std::make_unique<FindTarget>());
            findAndShare->AddChildNode(std::make_unique<ShareTargetInformationNode>());
            findAndShare->AddChildNode(std::make_unique<FindPathNode>());
            root->AddChildNode(std::move(findAndShare));
            
            // PRIORITY 3: Use shared target information from teammates
            auto useSharedTarget = std::make_unique<SequenceNode>();
            useSharedTarget->AddChildNode(std::make_unique<UseSharedTargetInformationNode>());
            useSharedTarget->AddChildNode(std::make_unique<FindPathNode>());
            root->AddChildNode(std::move(useSharedTarget));

            // PRIORITY 4: Predictive behavior when chasing
            auto SearchForPlayer = std::make_unique<SequenceNode>();
            SearchForPlayer->AddChildNode(std::make_unique<PredictPlayersTurn>());
            SearchForPlayer->AddChildNode(std::make_unique<JunctionDesitionNode>(0, 100, 0));
            root->AddChildNode(std::move(SearchForPlayer));

            // PRIORITY 5: Patrol
            root->AddChildNode(std::make_unique<JunctionDesitionNode>(100, 80, 1));
            break;
        }
        default:
            break;
        }

        actor.AI = std::move(root);
        return actor;
    };


    Actor CreateStaticEntity(int x, int y, Direction direction, EntityType type) {
        Actor actor = Actor(x, y, IMG_LoadTexture(renderer, "./Assets/Sprites/testSprite.png"));
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
        player.SetTexture(IMG_LoadTexture(renderer, "./Assets/Sprites/testSprite.png"));

        return player;
    };
    EntityFactory(SDL_Renderer *rend){
        renderer = rend;
        std::cout << "done3" << std::endl;
    };
    ~EntityFactory() = default;
};