#include "ProbeUnit.h"

BWAPI::UnitType ProbeUnit::listPriorite[NUMBER_OF_PRIORITY] = {BWAPI::UnitTypes::Protoss_High_Templar,
                                                                BWAPI::UnitTypes::Protoss_Dragoon,
                                                                BWAPI::UnitTypes::Protoss_Reaver,
                                                                BWAPI::UnitTypes::Protoss_Zealot,
                                                                BWAPI::UnitTypes::Protoss_Probe};

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

BWAPI::UnitType* ProbeUnit::getListPriorite()
{
    return ProbeUnit::listPriorite;
}