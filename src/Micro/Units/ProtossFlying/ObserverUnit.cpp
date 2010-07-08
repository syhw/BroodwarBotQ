#include "ObserverUnit.h"

ObserverUnit::ObserverUnit(BWAPI::Unit* u, UnitsGroup* ug):FlyingUnit(u, ug)
{
}

ObserverUnit::~ObserverUnit()
{
}

void ObserverUnit::micro()
{
    BWAPI::Broodwar->printf("ObserverUnit::micro non implémenté !");
}

bool ObserverUnit::canHit(BWAPI::Unit* enemy)
{
    BWAPI::Broodwar->printf("ObserverUnit::canHit non implémenté !");
    return false;
}

int ObserverUnit::getTimeToAttack()
{
    return 0;
}