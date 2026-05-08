#include "AbilityMenu.h"

AbilityMenu::AbilityMenu(MenuManager* menus, MenuBlackboard& mBB, TimeBlackboard& tBB, AbilityMenuBlackboard& aBB)
    : GUI(menus, mBB, tBB), abilityBlackboard(aBB),
      abilityButtons{
          Button(100, 100, 200, 100, [this]() { CreateAbilitySelectionHandler(0)(); }, "Ability 1", font, textColor, abilityBlackboard.renderer),
          Button(100, 250, 200, 100, [this]() { CreateAbilitySelectionHandler(1)(); }, "Ability 2", font, textColor, abilityBlackboard.renderer),
          Button(100, 400, 200, 100, [this]() { CreateAbilitySelectionHandler(2)(); }, "Ability 3", font, textColor, abilityBlackboard.renderer)
      },
      abilityDescriptions{
          Text(320, 100, 600, 100, "Description 1", font, textColor, abilityBlackboard.renderer),
          Text(320, 250, 600, 100, "Description 2", font, textColor, abilityBlackboard.renderer),
          Text(320, 400, 600, 100, "Description 3", font, textColor, abilityBlackboard.renderer)
      }
{
    ProposeAbilities();
    UpdateAbilityButtonsUI();
}

bool AbilityMenu::HasEmptySlot() const {
    if (!abilityBlackboard.playerAbilities) return false;
    
    for (int i = 0; i < 4; ++i) {
        if (abilityBlackboard.playerAbilities[i].type == AbilityType::None) {
            return true;
        }
    }
    return false;
}

int AbilityMenu::GetEmptySlotIndex() const {
    if (!abilityBlackboard.playerAbilities) return -1;
    
    for (int i = 0; i < 4; ++i) {
        if (abilityBlackboard.playerAbilities[i].type == AbilityType::None) {
            return i;
        }
    }
    return -1;
}

bool AbilityMenu::HasConsumableAbility() const {
    if (!abilityBlackboard.playerAbilities) return false;
    
    for (int i = 0; i < 4; ++i) {
        if (abilityBlackboard.playerAbilities[i].isConsumable && 
            abilityBlackboard.playerAbilities[i].type != AbilityType::None) {
            return true;
        }
    }
    return false;
}

std::vector<int> AbilityMenu::GetConsumableAbilityIndices() const {
    std::vector<int> indices;
    
    if (!abilityBlackboard.playerAbilities) return indices;
    
    for (int i = 0; i < 4; ++i) {
        if (abilityBlackboard.playerAbilities[i].isConsumable && 
            abilityBlackboard.playerAbilities[i].type != AbilityType::None) {
            indices.push_back(i);
        }
    }
    return indices;
}

void AbilityMenu::ProposeAbilities() {
    std::vector<AbilityType> availableAbilities{
        AbilityType::Claymore,
        AbilityType::WallCharge,
        AbilityType::RoadBlocker,
        AbilityType::SpeedBoost,
        AbilityType::Invisibility
    };
    
    // Determine what to propose based on current abilities
    if (HasEmptySlot()) {
        // Can propose new ability types
        for (int i = 0; i < 3; ++i) {
            std::uniform_int_distribution<> dist(0, availableAbilities.size() - 1);
            int randomIndex = dist(rng);
            proposedAbilities[i] = availableAbilities[randomIndex];
            proposedDescriptions[i] = GetAbilityDescription(proposedAbilities[i]);
        }
    } else if (HasConsumableAbility()) {
        // Propose increasing charges for consumable abilities
        auto consumableIndices = GetConsumableAbilityIndices();
        
        for (int i = 0; i < 3; ++i) {
            std::uniform_int_distribution<> indexDist(0, consumableIndices.size() - 1);
            int selectedIndex = consumableIndices[indexDist(rng)];
            proposedAbilities[i] = abilityBlackboard.playerAbilities[selectedIndex].type;
            proposedDescriptions[i] = "Add charges to: " + GetAbilityDescription(proposedAbilities[i]);
        }
    } else {
        // No consumable abilities and no empty slots - should not happen in normal gameplay
        // Fallback: propose new abilities anyway
        for (int i = 0; i < 3; ++i) {
            std::uniform_int_distribution<> dist(0, availableAbilities.size() - 1);
            int randomIndex = dist(rng);
            proposedAbilities[i] = availableAbilities[randomIndex];
            proposedDescriptions[i] = GetAbilityDescription(proposedAbilities[i]);
        }
    }
}

std::string AbilityMenu::GetAbilityDescription(AbilityType type) const {
    switch (type) {
        case AbilityType::Claymore:
            return "Claymore: Explosion ability\nCharges: 5, Cooldown: 5s";
        case AbilityType::WallCharge:
            return "Wall Charge: Ram through walls\nCharges: 5, Cooldown: 50s";
        case AbilityType::RoadBlocker:
            return "Road Blocker: Place obstacle\nCharges: 3, Cooldown: 50s";
        case AbilityType::SpeedBoost:
            return "Speed Boost: Increase movement speed\nDuration: 3s, Cooldown: 40s";
        case AbilityType::Invisibility:
            return "Invisibility: Hide from enemies\nDuration: 5s, Cooldown: 40s";
        case AbilityType::None:
        default:
            return "None";
    }
}

void AbilityMenu::UpdateAbilityButtonsUI() {
    if (!abilityBlackboard.playerAbilities || !abilityBlackboard.abilityFactory) {
        return;
    }
    
    // Update button text and descriptions
    for (int i = 0; i < 3; ++i) {
        abilityDescriptions[i].UpdateTextTexture(proposedDescriptions[i], font, textColor);
    }
}

std::function<void()> AbilityMenu::CreateAbilitySelectionHandler(int proposalIndex) {
    return [this, proposalIndex]() {
        if (!abilityBlackboard.playerAbilities || !abilityBlackboard.abilityFactory) {
            return;
        }
        
        AbilityType selectedAbility = proposedAbilities[proposalIndex];
        
        if (HasEmptySlot()) {
            // Add new ability to empty slot
            int emptySlot = GetEmptySlotIndex();
            if (emptySlot >= 0) {
                abilityBlackboard.playerAbilities[emptySlot] = 
                    abilityBlackboard.abilityFactory->CreateAbility(selectedAbility);
            }
        } else {
            // Find and increase charges for selected consumable ability
            for (int i = 0; i < 4; ++i) {
                if (abilityBlackboard.playerAbilities[i].type == selectedAbility && 
                    abilityBlackboard.playerAbilities[i].isConsumable) {
                    // Add random charges (1 to 3)
                    std::uniform_int_distribution<> chargesDist(1, 3);
                    int chargesToAdd = chargesDist(rng);
                    abilityBlackboard.playerAbilities[i].charges += chargesToAdd;
                    break;
                }
            }
        }
        
        // Close this menu and return to previous menu
        if (!menus->IsEmpty()) {
            menus->RequestMenuPop();
        }
    };
}

void AbilityMenu::DrawWindow() {
    // Draw background
    SDL_SetRenderDrawColor(abilityBlackboard.renderer, 200, 200, 200, 255);
    SDL_FRect bgRect{0, 0, (float)menuBlackboard.windowWidth, (float)menuBlackboard.windowHight};
    SDL_RenderFillRect(abilityBlackboard.renderer, &bgRect);
    
    // Draw title
    Text titleText(50, 10, 900, 50, "Select an Ability to Add", font, textColor, abilityBlackboard.renderer);
    titleText.DrawElement(abilityBlackboard.renderer);
    
    // Draw buttons and descriptions
    for (int i = 0; i < 3; ++i) {
        abilityButtons[i].DrawElement(abilityBlackboard.renderer);
        abilityDescriptions[i].DrawElement(abilityBlackboard.renderer);
    }
}

SDL_AppResult AbilityMenu::HandleEvents(SDL_Event* Event) {
    for (int i = 0; i < 3; ++i) {
        abilityButtons[i].HandleKey(*Event);
    }
    
    return SDL_APP_CONTINUE;
}

void AbilityMenu::HandleGameLogic() {
    // No specific game logic needed for ability menu
}
