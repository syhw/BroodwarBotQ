#include "DarkArchonUnit.h"

BWAPI::UnitType DarkArchonUnit::listPriorite[NUMBER_OF_PRIORITY] = {BWAPI::UnitTypes::Protoss_High_Templar,
                                                                BWAPI::UnitTypes::Protoss_Dragoon,
                                                                BWAPI::UnitTypes::Protoss_Reaver,
                                                                BWAPI::UnitTypes::Protoss_Zealot,
                                                                BWAPI::UnitTypes::Protoss_Probe};

DarkArchonUnit::DarkArchonUnit(BWAPI::Unit* u, UnitsGroup* ug):SpecialUnit(u, ug)
{
}

DarkArchonUnit::~DarkArchonUnit()
{
}

void DarkArchonUnit::micro()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("DarkArchonUnit::micro non implémenté !");
#endif
}

bool DarkArchonUnit::canHit(BWAPI::Unit* enemy)
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("DarkArchonUnit::canHit non implémenté !");
#endif
    return false;
}

int DarkArchonUnit::getTimeToAttack()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("DarkArchonUnit::getTimeToAttack non implémenté !");
#endif
    return 0;
}

BWAPI::UnitType* DarkArchonUnit::getListPriorite()
{
    return DarkArchonUnit::listPriorite;
}