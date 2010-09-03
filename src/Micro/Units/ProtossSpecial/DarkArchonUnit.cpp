#include "DarkArchonUnit.h"

DarkArchonUnit::DarkArchonUnit(BWAPI::Unit* u, UnitsGroup* ug)
: SpecialUnit(u, ug)
{
}

DarkArchonUnit::~DarkArchonUnit()
{
}

void DarkArchonUnit::micro()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("DarkArchonUnit::micro non implémenté !");
#endif
}

void DarkArchonUnit::check()
{
}

bool DarkArchonUnit::canHit(BWAPI::Unit* enemy)
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("DarkArchonUnit::canHit non implémenté !");
#endif
    return false;
}

int DarkArchonUnit::getTimeToAttack()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("DarkArchonUnit::getTimeToAttack non implémenté !");
#endif
    return 0;
}

std::set<BWAPI::UnitType> DarkArchonUnit::getSetPrio()
{
    return std::set<BWAPI::UnitType>();
}