#include "DarkTemplarUnit.h"

BWAPI::UnitType DarkTemplarUnit::listPriorite[NUMBER_OF_PRIORITY] = {BWAPI::UnitTypes::Protoss_High_Templar,
                                                                BWAPI::UnitTypes::Protoss_Dragoon,
                                                                BWAPI::UnitTypes::Protoss_Reaver,
                                                                BWAPI::UnitTypes::Protoss_Zealot,
                                                                BWAPI::UnitTypes::Protoss_Probe};

DarkTemplarUnit::DarkTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug):GroundUnit(u, ug)
{
}

DarkTemplarUnit::~DarkTemplarUnit()
{
}

void DarkTemplarUnit::micro()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("DarkTemplarUnit::micro non implémenté !");
#endif
}

void DarkTemplarUnit::check()
{
}

bool DarkTemplarUnit::canHit(BWAPI::Unit* enemy)
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("DarkTemplarUnit::canHit non implémenté !");
#endif
    return false;
}

int DarkTemplarUnit::getTimeToAttack()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("DarkTemplarUnit::getTimeToAttack non implémenté !");
#endif
    return 0;
}

BWAPI::UnitType* DarkTemplarUnit::getListPriorite()
{
    return DarkTemplarUnit::listPriorite;
}