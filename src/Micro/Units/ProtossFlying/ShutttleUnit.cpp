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

bool ShuttleUnit::canHit(BWAPI::Unit* enemy)
{
    return false;
}