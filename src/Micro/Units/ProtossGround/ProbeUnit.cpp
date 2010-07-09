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
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ProbeUnit::micro non implémenté !");
#endif
}

bool ProbeUnit::canHit(BWAPI::Unit* enemy)
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ProbeUnit::canHit non implémenté !");
#endif
    return false;
}

int ProbeUnit::getTimeToAttack()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("ProbeUnit::getTimeToAttack non implémenté !");
#endif
    return 0;
}

BWAPI::UnitType* ProbeUnit::getListPriorite()
{
    return ProbeUnit::listPriorite;
}