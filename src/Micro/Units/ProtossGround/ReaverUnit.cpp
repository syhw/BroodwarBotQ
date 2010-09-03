#include "ReaverUnit.h"

std::set<BWAPI::UnitType> ReaverUnit::setPrio;

ReaverUnit::ReaverUnit(BWAPI::Unit* u, UnitsGroup* ug)
: GroundUnit(u, ug)
{
    if (setPrio.empty())
    {
        setPrio.insert(BWAPI::UnitTypes::Protoss_High_Templar);
        setPrio.insert(BWAPI::UnitTypes::Zerg_Defiler);
    }
}

ReaverUnit::~ReaverUnit()
{
}

void ReaverUnit::micro()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ReaverUnit::micro non implémenté !");
#endif
}

void ReaverUnit::check()
{
}

bool ReaverUnit::canHit(BWAPI::Unit* enemy)
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ReaverUnit::canHit non implémenté !");
#endif
    return false;
}

int ReaverUnit::getTimeToAttack()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ReaverUnit::getTimeToAttack non implémenté !");
#endif
    return 0;
}

std::set<BWAPI::UnitType> ReaverUnit::getSetPrio()
{
    return ReaverUnit::setPrio;
}