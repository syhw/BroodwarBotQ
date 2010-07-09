#include "HighTemplarUnit.h"

BWAPI::UnitType HighTemplarUnit::listPriorite[NUMBER_OF_PRIORITY] = {BWAPI::UnitTypes::Protoss_High_Templar,
                                                                BWAPI::UnitTypes::Protoss_Dragoon,
                                                                BWAPI::UnitTypes::Protoss_Reaver,
                                                                BWAPI::UnitTypes::Protoss_Zealot,
                                                                BWAPI::UnitTypes::Protoss_Probe};

HighTemplarUnit::HighTemplarUnit(BWAPI::Unit* u, UnitsGroup* ug):SpecialUnit(u, ug)
{
}

HighTemplarUnit::~HighTemplarUnit()
{
}

void HighTemplarUnit::micro()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("HighTemplarUnit::micro non implémenté !");
#endif
}

bool HighTemplarUnit::canHit(BWAPI::Unit* enemy)
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("HighTemplarUnit::canHit non implémenté !");
#endif
    return false;
}

int HighTemplarUnit::getTimeToAttack()
{
#ifdef __NON_IMPLEMENTE__
    BWAPI::Broodwar->printf("HighTemplarUnit::getTimeToAttack non implémenté !");
#endif
    return 0;
}

BWAPI::UnitType* HighTemplarUnit::getListPriorite()
{
    return HighTemplarUnit::listPriorite;
}