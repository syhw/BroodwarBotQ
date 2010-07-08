#include "MutaliskUnit.h"

MutaliskUnit::MutaliskUnit(BWAPI::Unit* u, UnitsGroup* ug):FlyingUnit(u, ug)
{
}

MutaliskUnit::~MutaliskUnit()
{
}

void MutaliskUnit::micro()
{
    BWAPI::Broodwar->printf("MutaliskUnit::micro non implémenté !");
}

bool MutaliskUnit::canHit(BWAPI::Unit* enemy)
{
    BWAPI::Broodwar->printf("MutaliskUnit::canHit non implémenté !");
    return false;
}

int MutaliskUnit::getTimeToAttack()
{
    BWAPI::Broodwar->printf("MutaliskUnit::getTimeToAttack non implémenté !");
    return 0;
}