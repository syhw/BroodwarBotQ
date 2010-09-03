#include "ShuttleUnit.h"

ShuttleUnit::ShuttleUnit(BWAPI::Unit* u, UnitsGroup* ug)
: FlyingUnit(u, ug)
{
}

ShuttleUnit::~ShuttleUnit()
{
}

void ShuttleUnit::micro()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ShuttleUnit::micro non implémenté !");
#endif
}

void ShuttleUnit::check()
{
}

bool ShuttleUnit::canHit(BWAPI::Unit* enemy)
{
    return false;
}

int ShuttleUnit::getTimeToAttack()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ShuttleUnit::getTimeToAttack non implémenté !");
#endif
    return 0;
}

std::set<BWAPI::UnitType> ShuttleUnit::getSetPrio()
{
    return std::set<BWAPI::UnitType>();
}