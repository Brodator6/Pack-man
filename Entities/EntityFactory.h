#pragma once
#include "../DataStructs.h"
#include "./AI/BehaviorTree.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "Player.h"
#include "ResourceManager.h"

class EntityFactory
{
private:
    SDL_Renderer *renderer;
    Blackboard *blackboard;
    ResourceManager textureManager;

    std::unique_ptr<SelectorNode> CreateBasicAI(){
        auto root = std::make_unique<SelectorNode>();

        auto increaseTickCounter = std::make_unique<IncreaseTickCounter>();
        root->AddChildNode(std::move(increaseTickCounter));

        auto findAndPlan = std::make_unique<SequenceNode>();
        findAndPlan->AddChildNode(std::make_unique<FindTarget>());
        findAndPlan->AddChildNode(std::make_unique<FindPathNode>());
        root->AddChildNode(std::move(findAndPlan));

        auto SearchForPlayer = std::make_unique<SequenceNode>();
        SearchForPlayer->AddChildNode(std::make_unique<PredictPlayersTurn>());
        SearchForPlayer->AddChildNode(std::make_unique<JunctionDesitionNode>(0, 100, 0));
        root->AddChildNode(std::move(SearchForPlayer));

        root->AddChildNode(std::make_unique<JunctionDesitionNode>(100, 80, 1));

        return root;
    }

    std::unique_ptr<SelectorNode> CreateAdvancedAI(){
        
        auto root = std::make_unique<SelectorNode>();

        auto increaseTickCounter = std::make_unique<IncreaseTickCounter>();
        root->AddChildNode(std::move(increaseTickCounter));

        auto findAndPlan = std::make_unique<SequenceNode>();
        findAndPlan->AddChildNode(std::make_unique<FindTarget>());
        findAndPlan->AddChildNode(std::make_unique<ShareTargetInformationNode>());
        findAndPlan->AddChildNode(std::make_unique<FindPathNode>());
        root->AddChildNode(std::move(findAndPlan));
        
        auto checkSharedData = std::make_unique<SequenceNode>();
        checkSharedData->AddChildNode(std::make_unique<UseSharedTargetInformationNode>());
        checkSharedData->AddChildNode(std::make_unique<FindPathNode>());
        root->AddChildNode(std::move(checkSharedData));
        
        auto SearchForPlayer = std::make_unique<SequenceNode>();
        SearchForPlayer->AddChildNode(std::make_unique<PredictPlayersTurn>());
        SearchForPlayer->AddChildNode(std::make_unique<UpdateSharedData>());
        SearchForPlayer->AddChildNode(std::make_unique<JunctionDesitionNode>(0, 100, 0));
        root->AddChildNode(std::move(SearchForPlayer));

        root->AddChildNode(std::make_unique<JunctionDesitionNode>(100, 80, 1));

        return root;
    }

    std::unique_ptr<SelectorNode> CreateBasicCommandoAI(){
        
        auto root = std::make_unique<SelectorNode>();

        auto increaseTickCounter = std::make_unique<IncreaseTickCounter>();
        root->AddChildNode(std::move(increaseTickCounter));

        auto findAndPlan = std::make_unique<SequenceNode>();
        findAndPlan->AddChildNode(std::make_unique<FindTarget>());        findAndPlan->AddChildNode(std::make_unique<FindPathNode>());
        root->AddChildNode(std::move(findAndPlan));
        
        auto SearchForPlayer = std::make_unique<SequenceNode>();
        SearchForPlayer->AddChildNode(std::make_unique<PredictPlayersTurn>());
        SearchForPlayer->AddChildNode(std::make_unique<JunctionDesitionNode>(0, 100, 0));
        root->AddChildNode(std::move(SearchForPlayer));

        root->AddChildNode(std::make_unique<JunctionDesitionNode>(100, 80, 1));

        return root;
    }

    std::unique_ptr<SelectorNode> CreateLeaderCommandoAI(){
        
        auto root = std::make_unique<SelectorNode>();

        auto increaseTickCounter = std::make_unique<IncreaseTickCounter>();
        root->AddChildNode(std::move(increaseTickCounter));

        auto findAndPlan = std::make_unique<SequenceNode>();
        findAndPlan->AddChildNode(std::make_unique<FindTarget>());        findAndPlan->AddChildNode(std::make_unique<FindPathNode>());
        root->AddChildNode(std::move(findAndPlan));
        
        auto SearchForPlayer = std::make_unique<SequenceNode>();
        SearchForPlayer->AddChildNode(std::make_unique<PredictPlayersTurn>());
        SearchForPlayer->AddChildNode(std::make_unique<JunctionDesitionNode>(0, 100, 0));
        root->AddChildNode(std::move(SearchForPlayer));

        root->AddChildNode(std::make_unique<JunctionDesitionNode>(100, 80, 1));

        return root;
    }

public:
    void CreateAndAddDynamicEnemy(int x, int y, EntityDirection direction, EntityType type) {
        if (!blackboard) return;

        auto& entityManager = blackboard->entityManager;
        int entityID = entityManager.AllocateEntityID();
        auto& positionComponents = entityManager.GetPositionComponents();
        auto& movementComponents = entityManager.GetMovementComponents();
        auto& renderComponents = entityManager.GetRenderComponents();
        auto& typeComponents = entityManager.GetTypeComponents();
        auto& aiComponents = entityManager.GetAIComponents();
        auto& blackboardComponents = entityManager.GetBlackboardComponents();

        positionComponents[entityID] = {x, y, static_cast<float>(x), static_cast<float>(y), direction};
        renderComponents[entityID] = {textureManager.getTexture("./Assets/Sprites/testSprite.png"), SDL_FRect{0, 0, 0, 0}};
        movementComponents[entityID] = {x, y, 1.0f, x, y, -1, -1, EntityDirection::Down, false};

        EnemyType enemyType = EnemyType::None;
        std::unique_ptr<SelectorNode> AI;
        switch(type) {
            case EntityType::BasicEnemy: {
                enemyType = EnemyType::BasicEnemy; 
                AI = CreateBasicAI();
                break;
            }
            case EntityType::AdvancedEnemy: {
                enemyType = EnemyType::AdvancedEnemy; 
                AI = CreateAdvancedAI();
                break;
            }
            case EntityType::CommandoEnemy: {
                enemyType = EnemyType::CommandoEnemy; 
                break;
            }
            default: break;
        }

        typeComponents[entityID] = {ActorType::DinamicActor, enemyType};
        aiComponents[entityID] = {{}, std::move(AI), enemyType};

        if (enemyType == EnemyType::AdvancedEnemy) {
            blackboardComponents[entityID] = {&entityManager.GetAdvancedEnemyBlackboard(), nullptr};
            entityManager.GetAdvancedEnemyBlackboard().GetOrCreateEnemyIndex(entityID);
        } else if (enemyType == EnemyType::CommandoEnemy) {
            blackboardComponents[entityID] = {nullptr, &entityManager.GetCommandoEnemyBlackboard()};
            entityManager.GetCommandoEnemyBlackboard().GetOrCreateEnemyIndex(entityID);
        }
    }

    void CreateAndAddStaticEntity(int x, int y, EntityDirection direction, EntityType type) {
        if (!blackboard) return;

        auto& entityManager = blackboard->entityManager;
        int entityID = entityManager.AllocateEntityID();
        auto& positionComponents = entityManager.GetPositionComponents();
        auto& renderComponents = entityManager.GetRenderComponents();
        auto& typeComponents = entityManager.GetTypeComponents();
        auto& staticComponents = entityManager.GetStaticComponents();
        auto& aiComponents = entityManager.GetAIComponents();

        positionComponents[entityID] = {x, y, static_cast<float>(x), static_cast<float>(y), direction};
        renderComponents[entityID] = {textureManager.getTexture("./Assets/Sprites/testSprite.png"), SDL_FRect{0, 0, 0, 0}};
        typeComponents[entityID] = {ActorType::StaticActor, EnemyType::None};
        staticComponents[entityID] = {0, 0};

        auto root = std::make_unique<SelectorNode>();
        switch(type){
            case EntityType::Claymore: {
                root->AddChildNode(std::make_unique<ClaymoreTriggerNode>());
                break;
            }
            case EntityType::WallCharge: {
                staticComponents[entityID].timer = 50;
                root->AddChildNode(std::make_unique<WallChargeDetonateNode>());
                break;
            }
            default:
                break;
        }

        aiComponents[entityID] = {{}, std::move(root), EnemyType::None};
    }

    Player CreatePlayer(int x, int y, EntityDirection direction, Blackboard &bb){
        Player player = Player (x, y, &bb);
        player.direction = direction;
        player.SetTexture(textureManager.getTexture("./Assets/Sprites/testSprite.png"));

        return player;
    };

    EntityFactory(SDL_Renderer *rend, Blackboard *bb)
        : renderer(rend), textureManager(rend), blackboard{bb}
    {
    };

    EntityFactory()
        : renderer(nullptr), textureManager(nullptr), blackboard{nullptr}
    {
    };

    ~EntityFactory() = default;
};