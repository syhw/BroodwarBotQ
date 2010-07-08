#include "DarkArchonUnit.h"

DarkArchonUnit::DarkArchonUnit(BWAPI::Unit* u, UnitsGroup* ug):SpecialUnit(u, ug)
{
}

DarkArchonUnit::~DarkArchonUnit()
{
}

void DarkArchonUnit::micro()
{
    BWAPI::Broodwar->printf("DarkArchonUnit::micro non implémenté !");
}

bool DarkArchonUnit::canHit(BWAPI::Unit* enemy)
{
    BWAPI::Broodwar->printf("DarkArchonUnit::canHit non implémenté !");
    return false;
}

int DarkArchonUnit::getTimeToAttack()
{
    BWAPI::Broodwar->printf("DarkArchonUnit::getTimeToAttack non implémenté !");
    return 0;
}