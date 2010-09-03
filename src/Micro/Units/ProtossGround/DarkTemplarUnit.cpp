#include "DarkTemplarUnit.h"

std::set<BWAPI::UnitType> DarkTemplarUnit::setPrio;

DarkTemplarUnit::DarkTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug)
: GroundUnit(u, ug)
{
    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Terran_Missile_Turret);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Spore_Colony);
        setPrio.insert(BWAPI::UnitTypes::Protoss_High_Templar);
        setPrio.insert(BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Defiler);
        setPrio.insert(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode);
    }
}

DarkTemplarUnit::~DarkTemplarUnit()
{
}

void DarkTemplarUnit::micro()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("DarkTemplarUnit::micro non implémenté !");
#endif
}

void DarkTemplarUnit::check()
{
}

bool DarkTemplarUnit::canHit(BWAPI::Unit* enemy)
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("DarkTemplarUnit::canHit non implémenté !");
#endif
    return false;
}

int DarkTemplarUnit::getTimeToAttack()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("DarkTemplarUnit::getTimeToAttack non implémenté !");
#endif
    return 0;
}

std::set<BWAPI::UnitType> DarkTemplarUnit::getSetPrio()
{
    return DarkTemplarUnit::setPrio;
}