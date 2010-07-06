#include "DarkTemplarUnit.h"

DarkTemplarUnit::DarkTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug):GroundUnit(u, ug)
{
}

DarkTemplarUnit::~DarkTemplarUnit()
{
}

void DarkTemplarUnit::micro()
{
    BWAPI::Broodwar->printf("DarkTemplarUnit::micro non implémenté !");
}

bool DarkTemplarUnit::canHit(BWAPI::Unit* enemy)
{
    BWAPI::Broodwar->printf("DarkTemplarUnit::canHit non implémenté !");
    return false;
}