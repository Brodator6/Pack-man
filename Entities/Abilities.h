#pragma once


enum class AbilityType{
    None,
    Claymore,
    WallCharge,
    RoadBlocker,
    SpeedBoost,
    Invisibility
};

enum AbilityID{
    permanentAbility1 = 0,
    permanentAbility2 = 1,
    consumableAbility1 = 2,
    consumableAbility2 = 3
};

struct Ability{
    AbilityType type;
    float cooldownRemaining;
    float cooldownDuration;
    float durationRemaining;
    float duration;
    int charges;
    bool isConsumable;
};