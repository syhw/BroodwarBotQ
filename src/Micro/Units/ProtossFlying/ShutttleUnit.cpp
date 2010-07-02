#include "ShuttleUnit.h"

ShuttleUnit::ShuttleUnit(BWAPI::Unit* u, UnitsGroup* ug):FlyingUnit(u, ug)
{
}

ShuttleUnit::~ShuttleUnit()
{
}

void ShuttleUnit::micro()
{
    BWAPI::Broodwar->printf("ShuttleUnit::micro non implémenté !");
}

const std::list<BWAPI::UnitType> ShuttleUnit::getListePrio() const
{
    return listPriorite;
}

bool ShuttleUnit::canHit(BWAPI::Unit* enemy)
{
    return false;
}