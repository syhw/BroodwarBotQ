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

const std::list<BWAPI::UnitType> ObserverUnit::getListePrio() const
{
    return listPriorite;
}

bool ObserverUnit::canHit(BWAPI::Unit* enemy)
{
    BWAPI::Broodwar->printf("ObserverUnit::canHit non implémenté !");
    return false;
}