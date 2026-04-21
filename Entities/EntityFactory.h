#pragma once
#include "./AI/BehaviorTree.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "Player.h"

enum EntityType{
    BasicEnemy,
    AdvancedEnemy,
    Claymore
};

class EntityFactory
{
private:
    SDL_Renderer *renderer;
public:
    Enemy CreateEntity(int x, int y, Direction direction,EntityType type) {
        Enemy enemy = Enemy(x, y, IMG_LoadTexture(renderer, "./Assets/Sprites/testSprite.png"));
        enemy.direction = direction;
        enemy.DinamicEntity.goalX = x;
        enemy.DinamicEntity.goalY = y;
        enemy.DinamicEntity.LastSeenPlayerX = -1;
        enemy.DinamicEntity.LastSeenPlayerY = -1;
        enemy.DinamicEntity.speedModifier = 1;
        enemy.DinamicEntity.isChasing = false;
        
        auto root = std::make_unique<SelectorNode>();

        switch(type){
        case EntityType::BasicEnemy : {
            // PRIORITY 1: Keep moving if we already have a path

            // PRIORITY 2: Search for Player (Vision)
            auto findAndPlan = std::make_unique<SequenceNode>();
            findAndPlan->AddChildNode(std::make_unique<FindTarget>());
            findAndPlan->AddChildNode(std::make_unique<FindPathNode>());
            root->AddChildNode(std::move(findAndPlan));

            auto SearchForPlayer = std::make_unique<SequenceNode>();
            SearchForPlayer->AddChildNode(std::make_unique<PredictPlayersTurn>());
            SearchForPlayer->AddChildNode(std::make_unique<JunctionDesitionNode>(30, 100, 0));
            root->AddChildNode(std::move(SearchForPlayer));

            root->AddChildNode(std::make_unique<JunctionDesitionNode>(100, 80, 1));
            break;
        }
        default:
            break;
        }

        enemy.AI = std::move(root);
        return enemy;
    };


    Enemy CreateStaticEntity(int x, int y, Direction direction, EntityType type) {
        Enemy enemy = Enemy(x, y, IMG_LoadTexture(renderer, "./Assets/Sprites/testSprite.png"));

        auto root = std::make_unique<SelectorNode>();

        switch(type){
        case EntityType::Claymore : {

            break;
        }
        default:
            break;
        }

        enemy.AI = std::move(root);
        return enemy;
    };

    Player CreatePlayer(int x, int y, Direction direction, Blackboard &bb){
        Player player = Player (x, y, &bb);

        player.SetTexture(IMG_LoadTexture(renderer, "./Assets/Sprites/testSprite.png"));

        return player;
    };
    EntityFactory(SDL_Renderer *rend){
        renderer = rend;
        std::cout << "done3" << std::endl;
    };
    ~EntityFactory() = default;
};