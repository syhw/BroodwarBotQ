#include "ArbiterUnit.h"

ArbiterUnit::ArbiterUnit(BWAPI::Unit* u, UnitsGroup* ug):SpecialUnit(u, ug)
{
}

ArbiterUnit::~ArbiterUnit()
{
}

void ArbiterUnit::micro()
{
    BWAPI::Broodwar->printf("ArbiterUnit::micro non implémenté !");
}

bool ArbiterUnit::canHit(BWAPI::Unit* enemy)
{
    BWAPI::Broodwar->printf("ArbiterUnit::canHit non implémenté !");
    return false;
}

int ArbiterUnit::getTimeToAttack()
{
    BWAPI::Broodwar->printf("ArbiterUnit::getTimeToAttack non implémenté !");
    return 0;
}