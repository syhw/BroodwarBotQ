#include "GroundUnit.h"

GroundUnit::GroundUnit(BWAPI::Unit* u, UnitsGroup* ug):BayesianUnit(u, ug)
{
}

GroundUnit::~GroundUnit()
{
}

int GroundUnit::damagesOn(BWAPI::Unit* enemy)
{
    BWAPI::WeaponType groundW = unit->getType().groundWeapon();
    int damages = groundW.damageAmount() + BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Ground_Weapons)*groundW.damageFactor();
    damages -= (enemy->getType().armor() + enemy->getPlayer()->getUpgradeLevel(enemy->getType().armorUpgrade()));
    if (groundW.damageType() == BWAPI::DamageTypes::Concussive)
    {
        if (enemy->getType().size() == BWAPI::UnitSizeTypes::Medium)
        {
            damages = static_cast<int>(damages * 0.5);
        }
        else if (enemy->getType().size() == BWAPI::UnitSizeTypes::Large)
        {
            damages = static_cast<int>(damages * 0.25);
        }
    }
    else if (groundW.damageType() == BWAPI::DamageTypes::Explosive)
    {
        if (enemy->getType().size() == BWAPI::UnitSizeTypes::Medium)
        {
            damages = static_cast<int>(damages * 0.75);
        }
        else if (enemy->getType().size() == BWAPI::UnitSizeTypes::Small)
        {
            damages = static_cast<int>(damages * 0.5);
        }
    }
    // L'unité fait au minimum 1 dégât
    return damages < 1 ? 1 : damages;
}