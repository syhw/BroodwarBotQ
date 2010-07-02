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
    int damages = airW.damageAmount();
    damages += BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Air_Weapons);
    if (airW.damageType() == BWAPI::DamageTypes::Concussive)
    {
        if (enemy->getType().size() == BWAPI::UnitSizeTypes::Medium)
        {
            damages *= 0.5;
        }
        else if (enemy->getType().size() == BWAPI::UnitSizeTypes::Large)
        {
            damages *= 0.25;
        }
    }
    else if (airW.damageType() == BWAPI::DamageTypes::Explosive)
    {
        if (enemy->getType().size() == BWAPI::UnitSizeTypes::Medium)
        {
            damages *= 0.75;
        }
        else if (enemy->getType().size() == BWAPI::UnitSizeTypes::Small)
        {
            damages *= 0.5;
        }
    }
    return damages;
}