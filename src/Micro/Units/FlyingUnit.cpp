#include "FlyingUnit.h"

FlyingUnit::FlyingUnit(BWAPI::Unit* u, UnitsGroup* ug):BayesianUnit(u, ug)
{
}

FlyingUnit::~FlyingUnit()
{
}

int FlyingUnit::damagesOn(BWAPI::Unit* enemy)
{
    BWAPI::WeaponType airW = unit->getType().airWeapon();
    int damages = airW.damageAmount() + BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Air_Weapons)*airW.damageFactor();
    damages -= (enemy->getType().armor() + enemy->getPlayer()->getUpgradeLevel(enemy->getType().armorUpgrade()));
    if (airW.damageType() == BWAPI::DamageTypes::Concussive)
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
    else if (airW.damageType() == BWAPI::DamageTypes::Explosive)
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