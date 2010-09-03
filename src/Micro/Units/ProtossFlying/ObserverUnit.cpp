#include "ObserverUnit.h"

ObserverUnit::ObserverUnit(BWAPI::Unit* u, UnitsGroup* ug):FlyingUnit(u, ug)
{
}

ObserverUnit::~ObserverUnit()
{
}

void ObserverUnit::micro()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ObserverUnit::micro non implémenté !");
#endif
}

void ObserverUnit::check()
{
}

bool ObserverUnit::canHit(BWAPI::Unit* enemy)
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ObserverUnit::canHit non implémenté !");
#endif
    return false;
}

int ObserverUnit::getTimeToAttack()
{
    return 0;
}

std::set<BWAPI::UnitType> ObserverUnit::getSetPrio()
{
    return std::set<BWAPI::UnitType>();
}