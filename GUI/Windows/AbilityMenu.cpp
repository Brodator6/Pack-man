#include "AbilityMenu.h"
#include <algorithm>

AbilityMenu::AbilityMenu(MenuManager* menus, MenuBlackboard& mBB, TimeBlackboard& tBB, AbilityMenuBlackboard& aBB)
    : GUI(menus, mBB, tBB), abilityBlackboard(aBB),
      abilityButtons{
          Button(600, 100, 200, 200, [this]() { CreateAbilitySelectionHandler(0)(); }, "Ability 1", font, textColor, abilityBlackboard.renderer),
          Button(600, 450, 200, 200, [this]() { CreateAbilitySelectionHandler(1)(); }, "Ability 2", font, textColor, abilityBlackboard.renderer)
      },
      abilityDescriptions{
          Text(820, 100, 600, 200, "Description 1", font, {255, 255, 255, 0}, abilityBlackboard.renderer),
          Text(820, 450, 600, 200, "Description 2", font, {255, 255, 255, 0}, abilityBlackboard.renderer)
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
        AbilityType::Mine,
        AbilityType::WallCharge,
        AbilityType::RoadBlocker,
        AbilityType::SpeedBoost,
        AbilityType::Invisibility
    };
    
    // Determine what to propose based on current abilities
    if (HasEmptySlot()) {
        // Can propose new ability types
        // Exclude any ability types the player already has, including consumables
        std::vector<AbilityType> candidates = availableAbilities;
        if (abilityBlackboard.playerAbilities) {
            for (int p = 0; p < 4; ++p) {
                AbilityType pt = abilityBlackboard.playerAbilities[p].type;
                if (pt != AbilityType::None) {
                    candidates.erase(std::remove(candidates.begin(), candidates.end(), pt), candidates.end());
                }
            }
        }
        if (candidates.empty()) {
            candidates = availableAbilities;
        }

        for (int i = 0; i < 2; ++i) {
            if (candidates.empty()) {
                proposedAbilities[i] = AbilityType::None;
                proposedDescriptions[i] = "None";
                continue;
            }
            std::uniform_int_distribution<> dist(0, candidates.size() - 1);
            int randomIndex = dist(rng);
            proposedAbilities[i] = candidates[randomIndex];
            proposedDescriptions[i] = GetAbilityDescription(proposedAbilities[i]);
            if (candidates.size() > 1) {
                candidates.erase(candidates.begin() + randomIndex);
            }
        }
    } else if (HasConsumableAbility()) {
        // Propose increasing charges for consumable abilities
        auto consumableIndices = GetConsumableAbilityIndices();
        
        for (int i = 0; i < 2; ++i) {
            if (consumableIndices.empty()) break;
            std::uniform_int_distribution<> indexDist(0, consumableIndices.size() - 1);
            int selectedIndex = consumableIndices[indexDist(rng)];
            proposedAbilities[i] = abilityBlackboard.playerAbilities[selectedIndex].type;
            proposedDescriptions[i] = "Add charges to: " + GetAbilityDescription(proposedAbilities[i]);
        }
    } else {
        // No consumable abilities and no empty slots - should not happen in normal gameplay
        // Fallback: propose new abilities anyway
        for (int i = 0; i < 2; ++i) {
            std::uniform_int_distribution<> dist(0, availableAbilities.size() - 1);
            int randomIndex = dist(rng);
            proposedAbilities[i] = availableAbilities[randomIndex];
            proposedDescriptions[i] = GetAbilityDescription(proposedAbilities[i]);
        }
    }
}

std::string AbilityMenu::GetAbilityDescription(AbilityType type) const {
    switch (type) {
        case AbilityType::Mine:
            return "Mine: Explosion ability\nCharges: 5, Cooldown: 5s";
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
    for (int i = 0; i < 2; ++i) {
        abilityDescriptions[i].UpdateTextTexture(proposedDescriptions[i], font, textColor);

        // Choose texture for the button: prefer player's existing ability texture if matching, otherwise create a temp ability
        std::shared_ptr<SDL_Texture> tex = nullptr;
        for (int p = 0; p < 4; ++p) {
            if (abilityBlackboard.playerAbilities[p].type == proposedAbilities[i] && abilityBlackboard.playerAbilities[p].texture) {
                tex = abilityBlackboard.playerAbilities[p].texture;
                break;
            }
        }
        if (!tex && proposedAbilities[i] != AbilityType::None) {
            Ability tmp = abilityBlackboard.abilityFactory->CreateAbility(proposedAbilities[i]);
            tex = tmp.texture;
        }

        // Replace button with one that shows texture (keep same handler)
        int y = (i == 0) ? 100 : 450;
        abilityButtons[i] = Button(600, y, 200, 200, CreateAbilitySelectionHandler(i), tex, abilityBlackboard.renderer);
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
    menus->GetRootMenu()->DrawWindow();

    SDL_FRect rect;
    rect = {500, 0, 1000, (float)menuBlackboard.windowHight};

    SDL_SetRenderDrawBlendMode(*menuBlackboard.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(*menuBlackboard.renderer, 100, 100, 100, 250);
    SDL_RenderFillRect(*menuBlackboard.renderer, &rect);
    SDL_SetRenderDrawBlendMode(*menuBlackboard.renderer, SDL_BLENDMODE_NONE);
    
    // Draw title
    titleText.DrawElement(abilityBlackboard.renderer);
    
    // Draw buttons and descriptions
    for (int i = 0; i < 2; ++i) {
        abilityButtons[i].DrawElement(abilityBlackboard.renderer);
        abilityDescriptions[i].DrawElement(abilityBlackboard.renderer);
    }
}

SDL_AppResult AbilityMenu::HandleEvents(SDL_Event* Event) {
    for (int i = 0; i < 2; ++i) {
        abilityButtons[i].HandleKey(*Event);
    }
    
    return SDL_APP_CONTINUE;
}

void AbilityMenu::HandleGameLogic() {
    // No specific game logic needed for ability menu
}
