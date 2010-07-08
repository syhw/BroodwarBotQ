#include "ProbeUnit.h"

ProbeUnit::ProbeUnit(BWAPI::Unit* u, UnitsGroup* ug):GroundUnit(u, ug)
{
}

ProbeUnit::~ProbeUnit()
{
}

void ProbeUnit::micro()
{
    BWAPI::Broodwar->printf("ProbeUnit::micro non implémenté !");
}

bool ProbeUnit::canHit(BWAPI::Unit* enemy)
{
    //BWAPI::Broodwar->printf("ProbeUnit::canHit non implémenté !");
    return false;
}

int ProbeUnit::getTimeToAttack()
{
    //BWAPI::Broodwar->printf("ProbeUnit::getTimeToAttack non implémenté !");
    return 0;
}