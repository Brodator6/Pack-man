#pragma once
#include "GUI.h"
#include "../../Entities/Abilities.h"
#include "../../DataStructs.h"
#include "../Elements/Button.h"
#include "../Elements/Text.h"
#include <random> 
#include <array> 

class AbilityMenu : public GUI {
private:
    AbilityMenuBlackboard abilityBlackboard;
    
    // Proposed abilities
    std::array<AbilityType, 2> proposedAbilities;
    std::array<std::string, 2> proposedDescriptions;
    
    // UI Elements
    std::array<Button, 2> abilityButtons;
    std::array<Text, 2> abilityDescriptions;

    Text titleText = Text(800, 10, 900, 50, "You may chose only one ability", font, {255, 255, 255, 0}, abilityBlackboard.renderer);
    
    // Random number generator
    std::mt19937 rng{std::random_device{}()};
    
    // Helper methods
    bool HasEmptySlot() const;
    int GetEmptySlotIndex() const;
    bool HasConsumableAbility() const;
    std::vector<int> GetConsumableAbilityIndices() const;
    
    void ProposeAbilities();
    void UpdateAbilityButtonsUI();
    std::string GetAbilityDescription(AbilityType type) const;
    
    // Button action handlers
    std::function<void()> CreateAbilitySelectionHandler(int proposalIndex);
    
public:
    void DrawWindow() override;
    SDL_AppResult HandleEvents(SDL_Event* Event) override;
    void HandleGameLogic() override;
    
    AbilityMenu(MenuManager* menus, MenuBlackboard& mBB, TimeBlackboard& tBB, AbilityMenuBlackboard& aBB);
    ~AbilityMenu() = default;
};
