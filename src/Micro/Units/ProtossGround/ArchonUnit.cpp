#include "ArchonUnit.h"

ArchonUnit::ArchonUnit(BWAPI::Unit* u, UnitsGroup* ug):GroundUnit(u, ug)
{
}

ArchonUnit::~ArchonUnit()
{
}

void ArchonUnit::micro()
{
    BWAPI::Broodwar->printf("ArchonUnit::micro non implémenté !");
}

bool ArchonUnit::canHit(BWAPI::Unit* enemy)
{
    BWAPI::Broodwar->printf("ArchonUnit::canHit non implémenté !");
    return false;
}